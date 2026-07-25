using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

namespace SoccerMobile.Tests.PlayMode
{
    public sealed class LegacyGuiRuntimeRendererPlayModeTests
    {
        private const float SceneRunDurationSeconds = 30f;

        private static readonly string[] ScenePaths =
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

        private EditorBuildSettingsScene[] originalBuildScenes;
        private readonly List<RuntimeLog> unexpectedLogs = new List<RuntimeLog>();

        [OneTimeSetUp]
        public void AddSmokeTestScenesToBuildSettings()
        {
            originalBuildScenes = EditorBuildSettings.scenes;
            var configuredPaths = new HashSet<string>(
                originalBuildScenes.Select(scene => scene.path),
                StringComparer.Ordinal);
            var smokeOnlyScenes = ScenePaths
                .Where(path => !configuredPaths.Contains(path))
                .Select(path => new EditorBuildSettingsScene(path, true));

            EditorBuildSettings.scenes = originalBuildScenes.Concat(smokeOnlyScenes).ToArray();
        }

        [OneTimeTearDown]
        public void RestoreBuildSettings()
        {
            if (originalBuildScenes != null)
                EditorBuildSettings.scenes = originalBuildScenes;
        }

        [UnityTest]
        public IEnumerator Scene_RunsForThirtySecondsWithoutWarningsOrErrors(
            [ValueSource(nameof(ScenePaths))] string scenePath)
        {
            yield return RunScene(scenePath);
        }

        [UnityTest]
        public IEnumerator GameSelectionScene_ResolutionChangeDoesNotLogWarningsOrErrors()
        {
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/GameSelectionScene.unity");

            int originalWidth = Screen.width;
            int originalHeight = Screen.height;
            Screen.SetResolution(Mathf.Max(320, originalWidth / 2), Mathf.Max(180, originalHeight / 2), false);
            yield return null;
            yield return null;

            Screen.SetResolution(originalWidth, originalHeight, false);
            yield return null;
            yield return null;

            AssertNoUnexpectedLogs("GameSelectionScene resolution change");
        }

        private IEnumerator RunScene(string scenePath)
        {
            BeginLogCapture();
            yield return LoadScene(scenePath);

            float deadline = Time.realtimeSinceStartup + SceneRunDurationSeconds;
            while (Time.realtimeSinceStartup < deadline)
                yield return null;

            yield return ResetPersistentSceneState();
            AssertNoUnexpectedLogs(scenePath);
        }

        private static IEnumerator LoadScene(string scenePath)
        {
            AsyncOperation operation = SceneManager.LoadSceneAsync(scenePath, LoadSceneMode.Single);
            Assert.IsNotNull(operation, "Could not start loading scene " + scenePath + ".");
            yield return operation;
            yield return null;
            yield return null;

            if (scenePath == "Assets/Scenes/GameSelectionScene.unity" ||
                scenePath == "Assets/Scenes/KickOffScene.unity")
            {
                Type rendererType = FindType("LegacyGuiRuntimeRenderer");
                Assert.IsNotNull(rendererType, "Legacy GUI runtime renderer type was not found.");
                Assert.IsNotEmpty(
                    Resources.FindObjectsOfTypeAll(rendererType),
                    "Legacy GUI runtime renderer was not active after loading " + scenePath + ".");
            }
        }

        private static IEnumerator ResetPersistentSceneState()
        {
            foreach (MonoBehaviour behaviour in UnityEngine.Object.FindObjectsByType<MonoBehaviour>(FindObjectsInactive.Include))
            {
                if (behaviour.GetType().Name == "AudioManager")
                    UnityEngine.Object.Destroy(behaviour.gameObject);
            }

            yield return null;
            yield return null;
        }

        private void BeginLogCapture()
        {
            unexpectedLogs.Clear();
            Application.logMessageReceived -= CaptureUnexpectedLog;
            Application.logMessageReceived += CaptureUnexpectedLog;
        }

        private void CaptureUnexpectedLog(string condition, string stackTrace, LogType type)
        {
            if (type == LogType.Warning || type == LogType.Error || type == LogType.Assert || type == LogType.Exception)
                unexpectedLogs.Add(new RuntimeLog(type, condition, stackTrace));
        }

        private void AssertNoUnexpectedLogs(string scope)
        {
            Application.logMessageReceived -= CaptureUnexpectedLog;
            if (unexpectedLogs.Count == 0)
                return;

            string details = string.Join(
                "\n\n",
                unexpectedLogs.Select(log => log.ToString()).ToArray());
            Assert.Fail(scope + " produced unexpected Unity console entries:\n" + details);
        }

        private static Type FindType(string typeName)
        {
            foreach (System.Reflection.Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                Type type = assembly.GetType(typeName, false);
                if (type != null)
                    return type;
            }

            return null;
        }

        private readonly struct RuntimeLog
        {
            private readonly LogType type;
            private readonly string condition;
            private readonly string stackTrace;

            public RuntimeLog(LogType type, string condition, string stackTrace)
            {
                this.type = type;
                this.condition = condition;
                this.stackTrace = stackTrace;
            }

            public override string ToString()
            {
                return "[" + type + "] " + condition + "\n" + stackTrace;
            }
        }
    }
}
