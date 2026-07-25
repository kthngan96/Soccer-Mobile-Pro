# Unity 6 Upgrade Notes

## Upgrade path

- Baseline: Unity 2022.3.62f3
- Compatibility checkpoint: Unity 6000.0.80f1
- Target: Unity 6000.5.4f1

The Unity 6000.0 checkpoint kept URP compatibility mode enabled. URP 17.5 in
Unity 6000.5.4f1 migrated the global settings and no longer serializes the old
compatibility-mode flag. The project has no custom renderer features; adding or
modernizing custom Render Graph passes remains outside this upgrade.

## Automated validation

Run the following methods with Unity in batch mode:

- `UpgradeValidation.ValidateScenes`
- `UpgradeValidation.BuildAddressables`
- `UpgradeValidation.BuildAndroid`
- `UpgradeValidation.BuildIOS`

Build output defaults to `Builds/UpgradeValidation`. Set `UPGRADE_BUILD_ROOT`
to use a different output directory.

## Platform settings

- Android scripting backend remains IL2CPP.
- Android architectures remain ARMv7 and ARM64.
- Android minimum SDK is API 26. The 6000.0 checkpoint accepted API 23, but
  Unity 6000.5.4f1 upgraded the serialized minimum to API 26.
- Android and iOS application identifiers and the build-scene order are unchanged.

## Known environment requirements

- Unity 6000.5.4f1 must have Android Build Support and iOS Build Support
  installed before final mobile builds can run.
- An exported iOS project must be compiled, signed, archived, and smoke-tested
  on a compatible macOS/Xcode host.
- Device FPS, memory, visual regression, and soak testing require representative
  Android and iOS hardware and are not covered by batch-mode validation.

## Validation record

- Unity 6000.0.80f1: clean compilation; all 11 build scenes opened with no
  missing scripts; Addressables content built; EditMode tests passed; Android
  IL2CPP ARMv7/ARM64 AAB built; iOS Xcode project exported.
- Unity 6000.5.4f1: clean compilation; all 11 build scenes opened with no
  missing scripts; Addressables content built; EditMode tests passed.
- Unity 6000.5.4f1 mobile player builds are pending installation of Android
  Build Support and iOS Build Support for that Editor version.

The target package set includes URP/VFX Graph 17.5.0, ProBuilder 6.1.2,
Test Framework 1.7.0, uGUI 2.5.0, Input System 1.19.0, Cinemachine 3.1.6,
Addressables 2.9.1, and AI Navigation 2.0.13.
