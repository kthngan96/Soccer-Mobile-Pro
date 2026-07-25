# Unity 6 Upgrade Notes

## Upgrade path

- Baseline: Unity 2022.3.62f3
- Compatibility checkpoint: Unity 6000.0.80f1
- Target: Unity 6000.5.4f1

The upgrade keeps URP compatibility mode enabled. Migrating the project to the
Render Graph path is intentionally outside this upgrade.

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
- Android minimum SDK is API 23, the minimum supported by Unity 6.
- Android and iOS application identifiers and the build-scene order are unchanged.

## Known environment requirements

- Unity 6000.5.4f1 must have Android Build Support and iOS Build Support
  installed before final mobile builds can run.
- An exported iOS project must be compiled, signed, archived, and smoke-tested
  on a compatible macOS/Xcode host.
- Device FPS, memory, visual regression, and soak testing require representative
  Android and iOS hardware and are not covered by batch-mode validation.
