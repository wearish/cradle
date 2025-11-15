// https://imtheo.lol/Offsets/Offsets.hpp
#pragma once
#include <cstdint>
#include <string>

namespace Offsets {
    inline std::string ClientVersion = "version-b7e223555d904550";

    namespace AnimationTrack {
         inline constexpr uintptr_t Animation = 0xc8;
         inline constexpr uintptr_t Animator = 0x110;
         inline constexpr uintptr_t IsPlaying = 0xed;
         inline constexpr uintptr_t Looped = 0xec;
         inline constexpr uintptr_t Speed = 0xdc;
    }

    namespace BasePart {
         inline constexpr uintptr_t AssemblyAngularVelocity = 0x134;
         inline constexpr uintptr_t AssemblyLinearVelocity = 0x128;
         inline constexpr uintptr_t Color3 = 0x18c;
         inline constexpr uintptr_t Material = 0x268;
         inline constexpr uintptr_t Position = 0x11c;
         inline constexpr uintptr_t Primitive = 0x140;
         inline constexpr uintptr_t PrimitiveFlags = 0x294;
         inline constexpr uintptr_t PrimitiveOwner = 0x210;
         inline constexpr uintptr_t Rotation = 0xf8;
         inline constexpr uintptr_t Shape = 0x1a9;
         inline constexpr uintptr_t Size = 0x1d0;
         inline constexpr uintptr_t Transparency = 0xe8;
         inline constexpr uintptr_t ValidatePrimitive = 0x6;
    }

    namespace BaseScript {
         inline constexpr uintptr_t IsCoreScript = 0x180;
         inline constexpr uintptr_t ModuleFlags = 0x17c;
    }

    namespace ByteCode {
         inline constexpr uintptr_t Pointer = 0x10;
         inline constexpr uintptr_t Size = 0x20;
    }

    namespace Camera {
         inline constexpr uintptr_t CameraSubject = 0xe0;
         inline constexpr uintptr_t CameraType = 0x150;
         inline constexpr uintptr_t FieldOfView = 0x158;
         inline constexpr uintptr_t Position = 0x114;
         inline constexpr uintptr_t Rotation = 0xf0;
    }

    namespace ClickDetector {
         inline constexpr uintptr_t MaxActivationDistance = 0xf8;
         inline constexpr uintptr_t MouseIcon = 0xd8;
    }

    namespace DataModel {
         inline constexpr uintptr_t CreatorId = 0x180;
         inline constexpr uintptr_t GameId = 0x188;
         inline constexpr uintptr_t GameLoaded = 0x5e8;
         inline constexpr uintptr_t JobId = 0x130;
         inline constexpr uintptr_t PlaceId = 0x190;
         inline constexpr uintptr_t PlaceVersion = 0x1ac;
         inline constexpr uintptr_t PrimitiveCount = 0x428;
         inline constexpr uintptr_t ScriptContext = 0x3e0;
         inline constexpr uintptr_t ServerIP = 0x5d0;
         inline constexpr uintptr_t Workspace = 0x170;
    }

    namespace FFlags {
         inline constexpr uintptr_t DebugDisableTimeoutDisconnect = 0x6542420;
         inline constexpr uintptr_t EnableLoadModule = 0x6531780;
         inline constexpr uintptr_t PartyPlayerInactivityTimeoutInSeconds = 0x64f7d90;
         inline constexpr uintptr_t TaskSchedulerTargetFps = 0x6e54ccc;
         inline constexpr uintptr_t WebSocketServiceEnableClientCreation = 0x654f028;
    }

    namespace FakeDataModel {
         inline constexpr uintptr_t Pointer = 0x76b46b8;
         inline constexpr uintptr_t RealDataModel = 0x1c0;
    }

    namespace GuiObject {
         inline constexpr uintptr_t BackgroundColor3 = 0x548;
         inline constexpr uintptr_t BorderColor3 = 0x554;
         inline constexpr uintptr_t Image = 0xa18;
         inline constexpr uintptr_t LayoutOrder = 0x584;
         inline constexpr uintptr_t Position = 0x518;
         inline constexpr uintptr_t RichText = 0xad8;
         inline constexpr uintptr_t Rotation = 0x180;
         inline constexpr uintptr_t ScreenGui_Enabled = 0x4cc;
         inline constexpr uintptr_t Size = 0x538;
         inline constexpr uintptr_t Text = 0xe38;
         inline constexpr uintptr_t TextColor3 = 0xee8;
         inline constexpr uintptr_t Visible = 0x5b1;
    }

    namespace Humanoid {
         inline constexpr uintptr_t AutoRotate = 0x1d1;
         inline constexpr uintptr_t FloorMaterial = 0x188;
         inline constexpr uintptr_t Health = 0x18c;
         inline constexpr uintptr_t HipHeight = 0x198;
         inline constexpr uintptr_t HumanoidState = 0x858;
         inline constexpr uintptr_t HumanoidStateID = 0x20;
         inline constexpr uintptr_t Jump = 0x1d5;
         inline constexpr uintptr_t JumpHeight = 0x1a4;
         inline constexpr uintptr_t JumpPower = 0x1a8;
         inline constexpr uintptr_t MaxHealth = 0x1ac;
         inline constexpr uintptr_t MaxSlopeAngle = 0x1b0;
         inline constexpr uintptr_t RigType = 0x1c0;
         inline constexpr uintptr_t Walkspeed = 0x1cc;
         inline constexpr uintptr_t WalkspeedCheck = 0x3a0;
    }

    namespace Instance {
         inline constexpr uintptr_t AttributeContainer = 0x40;
         inline constexpr uintptr_t AttributeList = 0x18;
         inline constexpr uintptr_t AttributeToNext = 0x58;
         inline constexpr uintptr_t AttributeToValue = 0x18;
         inline constexpr uintptr_t ChildrenEnd = 0x8;
         inline constexpr uintptr_t ChildrenStart = 0x68;
         inline constexpr uintptr_t ClassBase = 0xc98;
         inline constexpr uintptr_t ClassDescriptor = 0x18;
         inline constexpr uintptr_t ClassName = 0x8;
         inline constexpr uintptr_t Name = 0xa8;
         inline constexpr uintptr_t Parent = 0x60;
    }

    namespace Lighting {
         inline constexpr uintptr_t Ambient = 0xd0;
         inline constexpr uintptr_t Brightness = 0x118;
         inline constexpr uintptr_t ClockTime = 0x1b0;
         inline constexpr uintptr_t ColorShift_Bottom = 0xe8;
         inline constexpr uintptr_t ColorShift_Top = 0xdc;
         inline constexpr uintptr_t ExposureCompensation = 0x124;
         inline constexpr uintptr_t FogColor = 0xf4;
         inline constexpr uintptr_t FogEnd = 0x12c;
         inline constexpr uintptr_t FogStart = 0x130;
         inline constexpr uintptr_t GeographicLatitude = 0x188;
         inline constexpr uintptr_t OutdoorAmbient = 0x100;
    }

    namespace LocalScript {
         inline constexpr uintptr_t ByteCode = 0x1a0;
         inline constexpr uintptr_t Hash = 0x1b0;
    }

    namespace MeshPart {
         inline constexpr uintptr_t MeshId = 0x2d8;
         inline constexpr uintptr_t Texture = 0x308;
    }

    namespace Misc {
         inline constexpr uintptr_t Adornee = 0x100;
         inline constexpr uintptr_t AnimationId = 0xc8;
         inline constexpr uintptr_t RequireLock = 0x914;
         inline constexpr uintptr_t StringLength = 0x10;
         inline constexpr uintptr_t Value = 0xc8;
    }

    namespace Model {
         inline constexpr uintptr_t PrimaryPart = 0x240;
         inline constexpr uintptr_t Scale = 0x14c;
    }

    namespace ModuleScript {
         inline constexpr uintptr_t ByteCode = 0x148;
         inline constexpr uintptr_t Hash = 0x160;
    }

    namespace MouseService {
         inline constexpr uintptr_t InputObject = 0xf8;
         inline constexpr uintptr_t MousePosition = 0xe4;
         inline constexpr uintptr_t SensitivityPointer = 0x778c890;
    }

    namespace Player {
         inline constexpr uintptr_t CameraMode = 0x2f0;
         inline constexpr uintptr_t Country = 0x108;
         inline constexpr uintptr_t DisplayName = 0x128;
         inline constexpr uintptr_t Gender = 0xe58;
         inline constexpr uintptr_t LocalPlayer = 0x128;
         inline constexpr uintptr_t MaxZoomDistance = 0x2e8;
         inline constexpr uintptr_t MinZoomDistance = 0x2ec;
         inline constexpr uintptr_t ModelInstance = 0x358;
         inline constexpr uintptr_t Mouse = 0xcc8;
         inline constexpr uintptr_t Team = 0x268;
         inline constexpr uintptr_t UserId = 0x290;
    }

    namespace PlayerConfigurer {
         inline constexpr uintptr_t OverrideDuration = 0x1f8;
         inline constexpr uintptr_t Pointer = 0x7690a98;
    }

    namespace PlayerMouse {
         inline constexpr uintptr_t Icon = 0xd8;
         inline constexpr uintptr_t Workspace = 0x160;
    }

    namespace PrimitiveFlags {
         inline constexpr uintptr_t Anchored = 0x2;
         inline constexpr uintptr_t CanCollide = 0x8;
         inline constexpr uintptr_t CanTouch = 0x10;
    }

    namespace ProximityPrompt {
         inline constexpr uintptr_t ActionText = 0xc8;
         inline constexpr uintptr_t Enabled = 0x14e;
         inline constexpr uintptr_t GamepadKeyCode = 0x134;
         inline constexpr uintptr_t HoldDuration = 0x138;
         inline constexpr uintptr_t KeyCode = 0x13c;
         inline constexpr uintptr_t MaxActivationDistance = 0x140;
         inline constexpr uintptr_t ObjectText = 0xe8;
         inline constexpr uintptr_t RequiresLineOfSight = 0x14f;
    }

    namespace RenderView {
         inline constexpr uintptr_t DeviceD3D11 = 0x8;
         inline constexpr uintptr_t VisualEngine = 0x10;
    }

    namespace RunService {
         inline constexpr uintptr_t HeartbeatFPS = 0xb8;
         inline constexpr uintptr_t HeartbeatTask = 0xe0;
    }

    namespace Sky {
         inline constexpr uintptr_t MoonAngularSize = 0x214;
         inline constexpr uintptr_t MoonTextureId = 0xd0;
         inline constexpr uintptr_t SkyboxBk = 0xf8;
         inline constexpr uintptr_t SkyboxDn = 0x120;
         inline constexpr uintptr_t SkyboxFt = 0x148;
         inline constexpr uintptr_t SkyboxLf = 0x170;
         inline constexpr uintptr_t SkyboxOrientation = 0x208;
         inline constexpr uintptr_t SkyboxRt = 0x198;
         inline constexpr uintptr_t SkyboxUp = 0x1c0;
         inline constexpr uintptr_t StarCount = 0x218;
         inline constexpr uintptr_t SunAngularSize = 0x20c;
         inline constexpr uintptr_t SunTextureId = 0x1e8;
    }

    namespace SpecialMesh {
         inline constexpr uintptr_t MeshId = 0x100;
         inline constexpr uintptr_t Scale = 0x14c;
    }

    namespace StatsItem {
         inline constexpr uintptr_t Value = 0x2ec0;
    }

    namespace TaskScheduler {
         inline constexpr uintptr_t FakeDataModelToDataModel = 0x1b0;
         inline constexpr uintptr_t JobEnd = 0x1d8;
         inline constexpr uintptr_t JobName = 0x18;
         inline constexpr uintptr_t JobStart = 0x1d0;
         inline constexpr uintptr_t MaxFPS = 0x1b0;
         inline constexpr uintptr_t Pointer = 0x77f3158;
         inline constexpr uintptr_t RenderJobToFakeDataModel = 0x38;
         inline constexpr uintptr_t RenderJobToRenderView = 0x218;
    }

    namespace Team {
         inline constexpr uintptr_t BrickColor = 0xc8;
    }

    namespace Textures {
         inline constexpr uintptr_t Decal_Texture = 0x190;
         inline constexpr uintptr_t Texture_Texture = 0x190;
    }

    namespace VisualEngine {
         inline constexpr uintptr_t Dimensions = 0x720;
         inline constexpr uintptr_t Pointer = 0x743bdd0;
         inline constexpr uintptr_t ToDataModel1 = 0x700;
         inline constexpr uintptr_t ToDataModel2 = 0x1c0;
         inline constexpr uintptr_t ViewMatrix = 0x4b0;
    }

    namespace Workspace {
         inline constexpr uintptr_t CurrentCamera = 0x418;
         inline constexpr uintptr_t DistributedGameTime = 0x438;
         inline constexpr uintptr_t Gravity = 0x1d8;
         inline constexpr uintptr_t GravityContainer = 0x390;
         inline constexpr uintptr_t PrimitivesPointer1 = 0x390;
         inline constexpr uintptr_t PrimitivesPointer2 = 0x238;
         inline constexpr uintptr_t ReadOnlyGravity = 0x960;
    }

}