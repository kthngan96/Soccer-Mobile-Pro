using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using UnityEditor;
using UnityEditor.AddressableAssets.Settings;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

internal static class UpgradeValidation
{
    private static readonly string[] ProductionScenes =
    {
        "Assets/Scenes/SplashScene.unity",
        "Assets/Scenes/MainMenu.unity",
        "Assets/Scenes/MatchScene.unity",
        "Assets/Scenes/MatchesScene.unity",
        "Assets/Scenes/KickOffScene.unity",
        "Assets/Scenes/IntroScene.unity",
        "Assets/Scenes/GroupsScene.unity",
        "Assets/Scenes/GameSelectionScene.unity",
        "Assets/Scenes/FinalCeleberation.unity",
        "Assets/Scenes/1stTeamSelection.unity",
        "Assets/Scenes/2ndTeamSelection.unity"
    };

    private static readonly string[] ExpectedProjectScenes =
    {
        "Assets/testScene.unity",
        "Assets/_LifeBar.unity",
        "Assets/Scene/Test_Scene.unity",
        "Assets/Scenes/1stTeamSelection.unity",
        "Assets/Scenes/2ndTeamSelection.unity",
        "Assets/Scenes/FinalCeleberation.unity",
        "Assets/Scenes/GameSelectionScene.unity",
        "Assets/Scenes/GroupsScene.unity",
        "Assets/Scenes/IntroScene.unity",
        "Assets/Scenes/KickOffScene.unity",
        "Assets/Scenes/MainMenu.unity",
        "Assets/Scenes/MatchesScene.unity",
        "Assets/Scenes/MatchScene.unity",
        "Assets/Scenes/SplashScene.unity"
    };

    public static void ValidateScenes()
    {
        var allProjectScenes = AssetDatabase.FindAssets("t:Scene", new[] { "Assets" })
            .Select(AssetDatabase.GUIDToAssetPath)
            .Where(path => path.StartsWith("Assets/", StringComparison.Ordinal))
            .Where(path => !Path.GetFileName(path).StartsWith("InitTestScene", StringComparison.Ordinal))
            .OrderBy(path => path, StringComparer.Ordinal)
            .ToArray();

        var expectedScenes = ExpectedProjectScenes
            .OrderBy(path => path, StringComparer.Ordinal)
            .ToArray();

        if (!allProjectScenes.SequenceEqual(expectedScenes, StringComparer.Ordinal))
        {
            var missing = expectedScenes.Except(allProjectScenes, StringComparer.Ordinal);
            var unexpected = allProjectScenes.Except(expectedScenes, StringComparer.Ordinal);
            throw new InvalidOperationException(
                "Project scene catalog does not contain exactly the expected 14 scenes. " +
                $"Missing: [{string.Join(", ", missing)}]. " +
                $"Unexpected: [{string.Join(", ", unexpected)}].");
        }

        var enabledScenes = EditorBuildSettings.scenes
            .Where(scene => scene.enabled)
            .Select(scene => scene.path)
            .ToArray();
        if (!enabledScenes.SequenceEqual(ProductionScenes, StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                "Enabled Build Settings scenes differ from the expected production scene order. " +
                $"Expected: [{string.Join(", ", ProductionScenes)}]. " +
                $"Actual: [{string.Join(", ", enabledScenes)}].");
        }

        var availableSceneNames = new HashSet<string>(
            allProjectScenes.Select(Path.GetFileNameWithoutExtension),
            StringComparer.Ordinal);
        var productionSceneNames = new HashSet<string>(
            ProductionScenes.Select(Path.GetFileNameWithoutExtension),
            StringComparer.Ordinal);
        ValidateScriptSceneTargets(availableSceneNames, productionSceneNames);

        var missingScriptCount = 0;
        var missingScriptScenes = new List<string>();
        var invalidSerializedSceneTargets = new List<string>();
        var originalSetup = EditorSceneManager.GetSceneManagerSetup();
        try
        {
            foreach (var scenePath in allProjectScenes)
            {
                var scene = EditorSceneManager.OpenScene(scenePath, OpenSceneMode.Single);
                var sceneMissingScriptCount = 0;
                foreach (var root in scene.GetRootGameObjects())
                {
                    foreach (var transform in root.GetComponentsInChildren<Transform>(true))
                    {
                        sceneMissingScriptCount +=
                            GameObjectUtility.GetMonoBehavioursWithMissingScriptCount(transform.gameObject);
                        ValidateSerializedSceneTargets(
                            scenePath,
                            transform.gameObject,
                            availableSceneNames,
                            productionSceneNames,
                            invalidSerializedSceneTargets);
                    }
                }

                missingScriptCount += sceneMissingScriptCount;
                if (sceneMissingScriptCount != 0)
                    missingScriptScenes.Add($"{scenePath} ({sceneMissingScriptCount})");
            }
        }
        finally
        {
            if (originalSetup.Any(scene => scene.isLoaded))
                EditorSceneManager.RestoreSceneManagerSetup(originalSetup);
            else
                EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);
        }

        if (missingScriptCount != 0)
            throw new InvalidOperationException(
                $"Scene validation found {missingScriptCount} missing MonoBehaviour reference(s): " +
                string.Join(", ", missingScriptScenes));

        if (invalidSerializedSceneTargets.Count != 0)
            throw new InvalidOperationException(
                "Scene validation found invalid serialized scene target(s): " +
                string.Join("; ", invalidSerializedSceneTargets));

        Debug.Log(
            $"Upgrade validation opened all {allProjectScenes.Length} scenes with no missing scripts " +
            "and confirmed all runtime scene targets and the 11-scene production Build Settings order.");
    }

    private static void ValidateScriptSceneTargets(
        HashSet<string> availableSceneNames,
        HashSet<string> productionSceneNames)
    {
        var invalidTargets = new List<string>();
        var sceneLoadPattern = new Regex(
            @"(?:SceneManager\s*\.\s*LoadScene|Application\s*\.\s*LoadLevel|LoadSceneWithReset)" +
            @"\s*\(\s*""(?<scene>[^""]+)""",
            RegexOptions.CultureInvariant);

        foreach (var scriptPath in AssetDatabase.FindAssets("t:Script", new[] { "Assets" })
                     .Select(AssetDatabase.GUIDToAssetPath)
                     .Where(path => path.EndsWith(".cs", StringComparison.OrdinalIgnoreCase)))
        {
            var source = File.ReadAllText(scriptPath);
            source = Regex.Replace(source, @"/\*.*?\*/", string.Empty, RegexOptions.Singleline);
            source = Regex.Replace(source, @"//.*$", string.Empty, RegexOptions.Multiline);

            foreach (Match match in sceneLoadPattern.Matches(source))
            {
                var target = match.Groups["scene"].Value;
                if (!availableSceneNames.Contains(target))
                    invalidTargets.Add($"{scriptPath} -> {target} (scene asset is missing)");
                else if (!productionSceneNames.Contains(target))
                    invalidTargets.Add($"{scriptPath} -> {target} (not in the production Build Settings)");
            }
        }

        if (invalidTargets.Count != 0)
            throw new InvalidOperationException(
                "Script scene-loading validation failed: " + string.Join("; ", invalidTargets));
    }

    private static void ValidateSerializedSceneTargets(
        string scenePath,
        GameObject gameObject,
        HashSet<string> availableSceneNames,
        HashSet<string> productionSceneNames,
        List<string> invalidTargets)
    {
        foreach (var behaviour in gameObject.GetComponents<MonoBehaviour>().Where(item => item != null))
        {
            var iterator = new SerializedObject(behaviour).GetIterator();
            if (!iterator.NextVisible(true))
                continue;

            do
            {
                if (iterator.propertyType != SerializedPropertyType.String ||
                    !iterator.name.EndsWith("SceneName", StringComparison.OrdinalIgnoreCase) ||
                    string.IsNullOrWhiteSpace(iterator.stringValue))
                {
                    continue;
                }

                var target = iterator.stringValue;
                var location = $"{scenePath}/{GetHierarchyPath(gameObject)} " +
                               $"({behaviour.GetType().Name}.{iterator.propertyPath})";
                if (!availableSceneNames.Contains(target))
                    invalidTargets.Add($"{location} -> {target} (scene asset is missing)");
                else if (!productionSceneNames.Contains(target))
                    invalidTargets.Add($"{location} -> {target} (not in the production Build Settings)");
            }
            while (iterator.NextVisible(false));
        }
    }

    private static string GetHierarchyPath(GameObject gameObject)
    {
        var path = gameObject.name;
        for (var parent = gameObject.transform.parent; parent != null; parent = parent.parent)
            path = $"{parent.name}/{path}";
        return path;
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
            scenes = EditorBuildSettings.scenes
                .Where(scene => scene.enabled)
                .Select(scene => scene.path)
                .ToArray(),
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
