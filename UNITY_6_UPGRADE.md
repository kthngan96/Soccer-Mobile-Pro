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

Android external tools can be supplied without machine-specific paths in Git:

- `UNITY_ANDROID_SDK_ROOT`
- `UNITY_ANDROID_NDK_ROOT`
- `UNITY_ANDROID_JDK_ROOT`

Release signing is read only from `UNITY_ANDROID_KEYSTORE`,
`UNITY_ANDROID_KEYSTORE_PASSWORD`, `UNITY_ANDROID_KEYALIAS`, and
`UNITY_ANDROID_KEYALIAS_PASSWORD`. Set `UPGRADE_REQUIRE_ANDROID_SIGNING=1` in
release CI so a build fails instead of silently falling back when credentials
are absent.

## Platform settings

- Android scripting backend remains IL2CPP.
- Android architectures remain ARMv7 and ARM64.
- Android minimum SDK is API 26. The 6000.0 checkpoint accepted API 23, but
  Unity 6000.5.4f1 upgraded the serialized minimum to API 26.
- Android and iOS application identifiers and the build-scene order are unchanged.

## Known environment requirements

- Unity 6000.5.4f1 has Android Build Support and iOS Build Support installed on
  the Windows validation host. Its validated Android toolchain is OpenJDK
  17.0.18, NDK r27c, Build Tools 36.0.0, and SDK Platforms 34, 35, and 36.
- An exported iOS project must be compiled, signed, archived, and smoke-tested
  on a compatible macOS/Xcode host.
- Device FPS, memory, visual regression, and soak testing require representative
  Android and iOS hardware and are not covered by batch-mode validation.

## Validation record

- Unity 6000.0.80f1: clean compilation; all 11 build scenes opened with no
  missing scripts; Addressables content built; EditMode tests passed; Android
  IL2CPP ARMv7/ARM64 AAB built; iOS Xcode project exported.
- Unity 6000.5.4f1 final validation (2026-07-25): imported from a new Library;
  clean compilation; all 11 build scenes opened with no missing scripts;
  Addressables content rebuilt; EditMode tests passed 1/1. The PlayMode runner
  passed, but the project currently contains no PlayMode test cases.
- Android IL2CPP ARMv7/ARM64 validation AAB built successfully at 302,265,822
  bytes with SHA-256
  `40445D15227D3B21F50639D910AC1640916C64168598BA41B03E4C06A58A09DC`.
  This artifact is not the release artifact because upload-keystore
  credentials were not available on the validation host.
- A clean iOS Xcode project exported successfully (3,548 files,
  1,766,638,009 bytes). Its `project.pbxproj` SHA-256 is
  `4220AE9193943266AA1B99119546C1CD2BA6C3B969CBA3961D49AF378779B393`.
  Archive/sign/install remains pending on macOS with Apple credentials.
- No Android device was connected to ADB. Android API 26-28/API 35+ smoke,
  logcat, FPS, memory, visual comparisons, and all iOS device gates remain
  pending. The branch must not be merged until those gates pass.
- `Assets/_Recovery/0.unity` was backed up outside the project and inspected.
  It contained zero scene roots, so there was no gameplay data to merge; the
  recovery artifact was removed from `Assets`.

The target package set includes URP/VFX Graph 17.5.0, ProBuilder 6.1.2,
Test Framework 1.7.0, uGUI 2.5.0, Input System 1.19.0, Cinemachine 3.1.6,
Addressables 2.9.1, and AI Navigation 2.0.13.
