using System;
using System.IO;
using System.Linq;
using UnityEditor;
using UnityEditor.AddressableAssets.Settings;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

internal static class UpgradeValidation
{
    private static readonly string[] EnabledScenes = EditorBuildSettings.scenes
        .Where(scene => scene.enabled)
        .Select(scene => scene.path)
        .ToArray();

    public static void ValidateScenes()
    {
        if (EnabledScenes.Length == 0)
            throw new InvalidOperationException("No enabled scenes are configured in Build Settings.");

        var missingScriptCount = 0;
        foreach (var scenePath in EnabledScenes)
        {
            var scene = EditorSceneManager.OpenScene(scenePath, OpenSceneMode.Single);
            foreach (var root in scene.GetRootGameObjects())
            {
                foreach (var transform in root.GetComponentsInChildren<Transform>(true))
                    missingScriptCount += GameObjectUtility.GetMonoBehavioursWithMissingScriptCount(transform.gameObject);
            }
        }

        if (missingScriptCount != 0)
            throw new InvalidOperationException(
                $"Scene validation found {missingScriptCount} missing MonoBehaviour reference(s).");

        Debug.Log($"Upgrade validation opened {EnabledScenes.Length} scenes with no missing scripts.");
    }

    public static void BuildAddressables()
    {
        AddressableAssetSettings.BuildPlayerContent(out var result);
        if (!string.IsNullOrEmpty(result.Error))
            throw new InvalidOperationException($"Addressables build failed: {result.Error}");

        Debug.Log("Addressables player content built successfully.");
    }

    public static void BuildAndroid()
    {
        EditorUserBuildSettings.buildAppBundle = true;
        Build(
            BuildTarget.Android,
            Path.Combine(BuildRoot(), "Android", "SoccerMobilePro.aab"));
    }

    public static void BuildIOS()
    {
        Build(
            BuildTarget.iOS,
            Path.Combine(BuildRoot(), "iOS", "SoccerMobilePro"));
    }

    private static void Build(BuildTarget target, string location)
    {
        var directory = target == BuildTarget.iOS ? location : Path.GetDirectoryName(location);
        Directory.CreateDirectory(directory);

        var report = BuildPipeline.BuildPlayer(new BuildPlayerOptions
        {
            scenes = EnabledScenes,
            target = target,
            locationPathName = location,
            options = BuildOptions.None
        });

        if (report.summary.result != BuildResult.Succeeded)
            throw new InvalidOperationException(
                $"{target} build failed with result {report.summary.result} and " +
                $"{report.summary.totalErrors} error(s).");

        Debug.Log(
            $"{target} build succeeded: {report.summary.totalSize} bytes in " +
            $"{report.summary.totalTime}.");
    }

    private static string BuildRoot()
    {
        var configuredRoot = Environment.GetEnvironmentVariable("UPGRADE_BUILD_ROOT");
        return string.IsNullOrWhiteSpace(configuredRoot)
            ? Path.GetFullPath(Path.Combine(Application.dataPath, "..", "Builds", "UpgradeValidation"))
            : Path.GetFullPath(configuredRoot);
    }
}
