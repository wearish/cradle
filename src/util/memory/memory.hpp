// https://github.com/manualmapped/UserMode-Memory-R-W/blob/main/memory.hpp
#pragma once

#include <windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#include <winternl.h>
#include <cstdint>

#pragma comment(lib, "ntdll.lib")

namespace cradle::memory
{
    inline INT32 processPid = 0;
    inline HANDLE processHandle = nullptr;
    inline uintptr_t baseAddress = 0;

    inline void *syscallReadStubPtr = nullptr;
    inline void *syscallWriteStubPtr = nullptr;
    inline bool syscallInitialized = false;

    using SyscallReadFn = NTSTATUS(__fastcall *)(HANDLE, PVOID, PVOID, ULONG, PULONG);
    using SyscallWriteFn = NTSTATUS(__fastcall *)(HANDLE, PVOID, PVOID, ULONG, PULONG);

    inline SyscallReadFn syscall_NtReadVirtualMemory = nullptr;
    inline SyscallWriteFn syscall_NtWriteVirtualMemory = nullptr;

    inline NTSTATUS(NTAPI *NtReadVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;
    inline NTSTATUS(NTAPI *NtWriteVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;

    inline NTSTATUS(NTAPI *ZwReadVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;
    inline NTSTATUS(NTAPI *ZwWriteVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;

    __forceinline INT32 FindProcess(LPCTSTR processName) noexcept
    {
        PROCESSENTRY32 pe{};
        pe.dwSize = sizeof(pe);
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE)
            return 0;

        for (BOOL ok = Process32First(snap, &pe); ok; ok = Process32Next(snap, &pe))
            if (!lstrcmpi(pe.szExeFile, processName))
            {
                CloseHandle(snap);
                return pe.th32ProcessID;
            }

        CloseHandle(snap);
        return 0;
    }

    __forceinline uintptr_t GetProcessBase() noexcept
    {
        if (processPid == 0)
            return 0;
        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processPid);
        if (!hProc)
            return 0;

        HMODULE mods[1024];
        DWORD bytesNeeded = 0;
        uintptr_t baseAddr = 0;

        if (EnumProcessModules(hProc, mods, sizeof(mods), &bytesNeeded) && bytesNeeded > 0)
            baseAddr = reinterpret_cast<uintptr_t>(mods[0]);

        CloseHandle(hProc);
        return baseAddr;
    }

    __forceinline bool IsValid(uintptr_t addr) noexcept
    {
        return addr >= 0x10000 && addr < 0x7FFFFFFFFFFF;
    }

    __forceinline void *CreateSyscallStub(uint32_t syscallId) noexcept
    {
        constexpr uint8_t stubTemplate[] = {
            0x4C, 0x8B, 0xD1,
            0xB8, 0x00, 0x00, 0x00, 0x00,
            0x0F, 0x05,
            0xC3};

        uint8_t stub[sizeof(stubTemplate)];
        memcpy(stub, stubTemplate, sizeof(stubTemplate));
        *reinterpret_cast<uint32_t *>(stub + 4) = syscallId;

        void *page = VirtualAlloc(nullptr, sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!page)
            return nullptr;

        memcpy(page, stub, sizeof(stub));
        DWORD oldProtect;
        VirtualProtect(page, sizeof(stub), PAGE_EXECUTE_READ, &oldProtect);

        return page;
    }

    __forceinline uint32_t GetSyscallID(const char *funcName) noexcept
    {
        static HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll)
            return 0;

        const auto *funcAddr = reinterpret_cast<const uint8_t *>(GetProcAddress(ntdll, funcName));
        if (!funcAddr)
            return 0;

        for (int i = 0; i < 20; ++i)
            if (funcAddr[i] == 0xB8)
                return *reinterpret_cast<const uint32_t *>(funcAddr + i + 1);

        return 0;
    }

    inline void EnsureSyscallInit() noexcept
    {
        if (syscallInitialized || processPid == 0)
            return;

        if (!processHandle)
            processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processPid);

        if (!processHandle)
            return;

        static HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll)
            return;

        NtReadVirtualMemory = reinterpret_cast<decltype(NtReadVirtualMemory)>(GetProcAddress(ntdll, "NtReadVirtualMemory"));
        NtWriteVirtualMemory = reinterpret_cast<decltype(NtWriteVirtualMemory)>(GetProcAddress(ntdll, "NtWriteVirtualMemory"));
        ZwReadVirtualMemory = reinterpret_cast<decltype(ZwReadVirtualMemory)>(GetProcAddress(ntdll, "ZwReadVirtualMemory"));
        ZwWriteVirtualMemory = reinterpret_cast<decltype(ZwWriteVirtualMemory)>(GetProcAddress(ntdll, "ZwWriteVirtualMemory"));

        if (!NtReadVirtualMemory || !NtWriteVirtualMemory || !ZwReadVirtualMemory || !ZwWriteVirtualMemory)
            return;

        uint32_t readSyscallId = GetSyscallID("NtReadVirtualMemory");
        uint32_t writeSyscallId = GetSyscallID("NtWriteVirtualMemory");
        if (readSyscallId == 0 || writeSyscallId == 0)
            return;

        syscallReadStubPtr = CreateSyscallStub(readSyscallId);
        syscallWriteStubPtr = CreateSyscallStub(writeSyscallId);
        if (!syscallReadStubPtr || !syscallWriteStubPtr)
            return;

        syscall_NtReadVirtualMemory = reinterpret_cast<SyscallReadFn>(syscallReadStubPtr);
        syscall_NtWriteVirtualMemory = reinterpret_cast<SyscallWriteFn>(syscallWriteStubPtr);

        syscallInitialized = true;
    }

    template <typename T>
    __forceinline T read(uint64_t address) noexcept
    {
        EnsureSyscallInit();
        T buffer{};
        SIZE_T bytesRead{};
        if (processHandle && NtReadVirtualMemory)
            NtReadVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), &buffer, sizeof(T), &bytesRead);
        return buffer;
    }

    template <typename T>
    __forceinline T syscall_read(uint64_t address) noexcept
    {
        EnsureSyscallInit();
        T buffer{};
        SIZE_T bytesRead{};
        if (processHandle && syscall_NtReadVirtualMemory)
            syscall_NtReadVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), &buffer, sizeof(T), reinterpret_cast<PULONG>(&bytesRead));
        return buffer;
    }

    template <typename T>
    __forceinline T zw_read(uint64_t address) noexcept
    {
        EnsureSyscallInit();
        T buffer{};
        SIZE_T bytesRead{};
        if (processHandle && ZwReadVirtualMemory)
            ZwReadVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), &buffer, sizeof(T), &bytesRead);
        return buffer;
    }

    __forceinline bool write_bytes(uint64_t address, const void *buffer, size_t size) noexcept
    {
        EnsureSyscallInit();
        if (!processHandle || !NtWriteVirtualMemory)
            return false;

        SIZE_T bytesWritten{};
        NTSTATUS writeStatus = NtWriteVirtualMemory(processHandle, reinterpret_cast<PVOID>(address), const_cast<void *>(buffer), size, &bytesWritten);
        return (writeStatus >= 0 && bytesWritten == size);
    }

    template <typename T>
    __forceinline bool write(uint64_t address, const T &buffer) noexcept
    {
        return write_bytes(address, &buffer, sizeof(T));
    }

    template <typename T>
    __forceinline bool syscall_write(uint64_t address, const T &buffer) noexcept
    {
        EnsureSyscallInit();
        if (!processHandle || !syscall_NtWriteVirtualMemory)
            return false;

        SIZE_T bytesWritten{};
        NTSTATUS writeStatus = syscall_NtWriteVirtualMemory(processHandle, reinterpret_cast<PVOID>(address),
                                                            const_cast<T *>(&buffer), sizeof(T), reinterpret_cast<PULONG>(&bytesWritten));
        return (writeStatus >= 0 && bytesWritten == sizeof(T));
    }

    template <typename T>
    __forceinline bool zw_write(uint64_t address, const T &buffer) noexcept
    {
        EnsureSyscallInit();
        if (!processHandle || !ZwWriteVirtualMemory)
            return false;

        SIZE_T bytesWritten{};
        NTSTATUS writeStatus = ZwWriteVirtualMemory(processHandle, reinterpret_cast<PVOID>(address),
                                                    const_cast<T *>(&buffer), sizeof(T), &bytesWritten);
        return (writeStatus >= 0 && bytesWritten == sizeof(T));
    }
}