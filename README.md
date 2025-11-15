# cradle
an actively maintained free and open-source roblox external for version `version-b7e223555d904550`. if a newer roblox version is released, offsets will be updated and a new version of the external will be published.

## requirements
- cmake 3.20+
- visual studio 2022 (or any c++17-compatible compiler)
- vcpkg

## dependencies
this project uses `spdlog` and `imgui`. we use [vcpkg](https://github.com/microsoft/vcpkg) to manage dependencies:
```powershell
.\vcpkg.exe install spdlog imgui:x64-windows
```

## building
clone the repo and build in release mode:
```powershell
git clone https://github.com/wearish/cradle.git
cd cradle
cmake -s . -b build
cmake --build build --config release
```
after a successful build, the executable will be located at build\bin\release\cradle.exe.
a working build will also always be published in the [releases](https://github.com/wearish/cradle/releases) tab.

## why this project exists
there aren’t many open-source externals for roblox. most of the ones that do exist are unmaintained forks of leaked sources and often don’t work properly. wall check is especially rare, and when it does exist, it’s usually broken or low quality. i’m not claiming my external is better in terms of code or fewer bugs than theirs, just sharing what i’ve noticed. the goal here is to have everything someone who cheats in roblox needs and nothing they don’t.

also, honestly, i was bored and wanted to build something. this project is pretty basic for now—it doesn’t do anything fancy yet. any messy code you see will get cleaned up over time :)

## contributing
contributions are welcome! open a pr or an issue. please base your prs on existing code; if a feature doesn’t exist, add it to the sdk.