using System;
using System.IO;
using System.Linq;
using System.Reflection;
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

    private static readonly string[] AllProjectScenes = AssetDatabase.FindAssets("t:Scene")
        .Select(AssetDatabase.GUIDToAssetPath)
        .Where(path => path.StartsWith("Assets/", StringComparison.Ordinal))
        .OrderBy(path => path, StringComparer.Ordinal)
        .ToArray();

    public static void ValidateScenes()
    {
        if (AllProjectScenes.Length == 0)
            throw new InvalidOperationException("No project scenes were found under Assets.");

        var missingScriptCount = 0;
        var originalSetup = EditorSceneManager.GetSceneManagerSetup();
        try
        {
            foreach (var scenePath in AllProjectScenes)
            {
                var scene = EditorSceneManager.OpenScene(scenePath, OpenSceneMode.Single);
                foreach (var root in scene.GetRootGameObjects())
                {
                    foreach (var transform in root.GetComponentsInChildren<Transform>(true))
                        missingScriptCount += GameObjectUtility.GetMonoBehavioursWithMissingScriptCount(transform.gameObject);
                }
            }
        }
        finally
        {
            EditorSceneManager.RestoreSceneManagerSetup(originalSetup);
        }

        if (missingScriptCount != 0)
            throw new InvalidOperationException(
                $"Scene validation found {missingScriptCount} missing MonoBehaviour reference(s).");

        Debug.Log($"Upgrade validation opened {AllProjectScenes.Length} scenes with no missing scripts.");
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
        ConfigureAndroidExternalTools();
        ConfigureAndroidSigning();
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
            options = string.Equals(
                Environment.GetEnvironmentVariable("UPGRADE_DEVELOPMENT_BUILD"),
                "1",
                StringComparison.Ordinal)
                ? BuildOptions.Development
                : BuildOptions.None
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

    private static void ConfigureAndroidExternalTools()
    {
        SetAndroidExternalToolPath("sdkRootPath", "UNITY_ANDROID_SDK_ROOT");
        SetAndroidExternalToolPath("ndkRootPath", "UNITY_ANDROID_NDK_ROOT");
        SetAndroidExternalToolPath("jdkRootPath", "UNITY_ANDROID_JDK_ROOT");
    }

    private static void SetAndroidExternalToolPath(string propertyName, string environmentName)
    {
        var configuredPath = Environment.GetEnvironmentVariable(environmentName);
        if (string.IsNullOrWhiteSpace(configuredPath))
            return;

        configuredPath = Path.GetFullPath(configuredPath);
        if (!Directory.Exists(configuredPath))
            throw new DirectoryNotFoundException(
                $"{environmentName} points to a missing directory: {configuredPath}");

        var settingsType = AppDomain.CurrentDomain.GetAssemblies()
            .Select(assembly => assembly.GetType("UnityEditor.Android.AndroidExternalToolsSettings"))
            .FirstOrDefault(type => type != null);
        var property = settingsType?.GetProperty(
            propertyName,
            BindingFlags.Public | BindingFlags.Static);
        if (property == null || !property.CanWrite)
            throw new InvalidOperationException(
                $"Unity Android external tools property is unavailable: {propertyName}");

        property.SetValue(null, configuredPath);
        Debug.Log($"{environmentName} configured for this validation run.");
    }

    private static void ConfigureAndroidSigning()
    {
        var keystore = Environment.GetEnvironmentVariable("UNITY_ANDROID_KEYSTORE");
        var keystorePassword = Environment.GetEnvironmentVariable("UNITY_ANDROID_KEYSTORE_PASSWORD");
        var alias = Environment.GetEnvironmentVariable("UNITY_ANDROID_KEYALIAS");
        var aliasPassword = Environment.GetEnvironmentVariable("UNITY_ANDROID_KEYALIAS_PASSWORD");
        var requireSigning = string.Equals(
            Environment.GetEnvironmentVariable("UPGRADE_REQUIRE_ANDROID_SIGNING"),
            "1",
            StringComparison.Ordinal);

        var supplied = new[] { keystore, keystorePassword, alias, aliasPassword };
        if (supplied.All(string.IsNullOrWhiteSpace))
        {
            if (requireSigning)
                throw new InvalidOperationException(
                    "Release signing is required, but Android signing environment variables are absent.");
            return;
        }

        if (supplied.Any(string.IsNullOrWhiteSpace))
            throw new InvalidOperationException(
                "Android signing is partially configured; all four signing environment variables are required.");

        keystore = Path.GetFullPath(keystore);
        if (!File.Exists(keystore))
            throw new FileNotFoundException("Android upload keystore was not found.", keystore);

        PlayerSettings.Android.useCustomKeystore = true;
        PlayerSettings.Android.keystoreName = keystore;
        PlayerSettings.Android.keystorePass = keystorePassword;
        PlayerSettings.Android.keyaliasName = alias;
        PlayerSettings.Android.keyaliasPass = aliasPassword;
        Debug.Log("Android release signing configured from environment variables.");
    }
}
