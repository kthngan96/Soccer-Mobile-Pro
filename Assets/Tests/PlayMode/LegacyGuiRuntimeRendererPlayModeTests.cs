using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using NUnit.Framework;
using UnityEditor;
using UnityEditor.SceneManagement;
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

        private static readonly string[] IntegerPlayerPrefKeys =
        {
            "jtsounds",
            "isMusicOn",
            "isSFXOn",
            "HasPendingCup",
            "lost",
            "matchNumber",
            "playerTeamIndex",
            "match1TeamIndex",
            "match2TeamIndex",
            "match3TeamIndex",
            "match4TeamIndex",
            "match5TeamIndex",
            "match6TeamIndex",
            "match7TeamIndex",
            "match1score1",
            "match1score2",
            "match2score1",
            "match2score2",
            "match3score1",
            "match3score2",
            "match4score1",
            "match4score2",
            "match5score1",
            "match5score2",
            "match6score1",
            "match6score2",
            "match7score1",
            "match7score2"
        };

        private readonly List<RuntimeLog> unexpectedLogs = new List<RuntimeLog>();
        private readonly Dictionary<string, PlayerPrefSnapshot> playerPrefSnapshots =
            new Dictionary<string, PlayerPrefSnapshot>(StringComparer.Ordinal);

        [OneTimeSetUp]
        public void CapturePlayerPrefs()
        {
            foreach (string key in IntegerPlayerPrefKeys)
            {
                playerPrefSnapshots[key] = new PlayerPrefSnapshot(
                    PlayerPrefs.HasKey(key),
                    PlayerPrefs.GetInt(key));
            }

            playerPrefSnapshots["message"] = new PlayerPrefSnapshot(
                PlayerPrefs.HasKey("message"),
                PlayerPrefs.GetString("message"));
        }

        [OneTimeTearDown]
        public void RestorePlayerPrefs()
        {
            StopLogCapture();

            foreach (KeyValuePair<string, PlayerPrefSnapshot> entry in playerPrefSnapshots)
            {
                if (!entry.Value.Existed)
                {
                    PlayerPrefs.DeleteKey(entry.Key);
                    continue;
                }

                if (entry.Value.IsString)
                    PlayerPrefs.SetString(entry.Key, entry.Value.StringValue);
                else
                    PlayerPrefs.SetInt(entry.Key, entry.Value.IntValue);
            }

            PlayerPrefs.Save();
        }

        [TearDown]
        public void TearDown()
        {
            StopLogCapture();
            Time.timeScale = 1f;
            AudioListener.volume = 1f;
        }

        [Test]
        public void SceneCatalogContainsExactlyFourteenProjectScenes()
        {
            string[] discoveredPaths = AssetDatabase.FindAssets("t:Scene", new[] { "Assets" })
                .Select(AssetDatabase.GUIDToAssetPath)
                .Where(path => !string.IsNullOrEmpty(path))
                .Where(path => !System.IO.Path.GetFileName(path)
                    .StartsWith("InitTestScene", StringComparison.Ordinal))
                .OrderBy(path => path, StringComparer.Ordinal)
                .ToArray();

            CollectionAssert.AreEquivalent(ScenePaths, discoveredPaths);
        }

        [UnityTest]
        public IEnumerator TestScene_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/testScene.unity");
        }

        [UnityTest]
        public IEnumerator LifeBarScene_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/_LifeBar.unity");
        }

        [UnityTest]
        public IEnumerator LegacyTestScene_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scene/Test_Scene.unity");
        }

        [UnityTest]
        public IEnumerator FirstTeamSelection_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/1stTeamSelection.unity");
        }

        [UnityTest]
        public IEnumerator SecondTeamSelection_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/2ndTeamSelection.unity");
        }

        [UnityTest]
        public IEnumerator FinalCelebration_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/FinalCeleberation.unity");
        }

        [UnityTest]
        public IEnumerator GameSelection_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/GameSelectionScene.unity");
        }

        [UnityTest]
        public IEnumerator Groups_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/GroupsScene.unity");
        }

        [UnityTest]
        public IEnumerator Intro_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/IntroScene.unity");
        }

        [UnityTest]
        public IEnumerator KickOff_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/KickOffScene.unity");
        }

        [UnityTest]
        public IEnumerator MainMenu_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/MainMenu.unity");
        }

        [UnityTest]
        public IEnumerator Matches_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/MatchesScene.unity");
        }

        [UnityTest]
        public IEnumerator Match_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/MatchScene.unity");
        }

        [UnityTest]
        public IEnumerator Splash_RunsForThirtySecondsWithoutErrors()
        {
            yield return RunScene("Assets/Scenes/SplashScene.unity");
        }

        [UnityTest]
        public IEnumerator GameSelectionScene_ResolutionChangeDoesNotLogErrors()
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/GameSelectionScene.unity");

            int originalWidth = Screen.width;
            int originalHeight = Screen.height;
            Screen.SetResolution(
                Mathf.Max(320, originalWidth / 2),
                Mathf.Max(180, originalHeight / 2),
                false);
            yield return null;
            yield return null;

            Screen.SetResolution(originalWidth, originalHeight, false);
            yield return null;
            yield return null;

            AssertNoUnexpectedLogs("GameSelectionScene resolution change");
        }

        [UnityTest]
        public IEnumerator GameSelectionScene_BackToMainMenuDoesNotKeepBallOverlay()
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/MainMenu.unity");

            yield return InvokeButtonAndWaitForScene("Play", "GameSelectionScene");
            yield return new WaitForEndOfFrame();
            Assert.IsTrue(
                GetStaticProperty<bool>(
                    "LegacyGuiRuntimeRenderer",
                    "DrewLegacy3DGuiOverlayLastRepaint"),
                "GameSelectionScene did not render its 3D ball overlay.");

            yield return InvokeButtonAndWaitForScene("Back", "MainMenu");
            yield return new WaitForEndOfFrame();
            Assert.IsFalse(
                GetStaticProperty<bool>(
                    "LegacyGuiRuntimeRenderer",
                    "DrewLegacy3DGuiOverlayLastRepaint"),
                "MainMenu redrew the stale GameSelectionScene ball overlay.");

            AssertNoUnexpectedLogs("GameSelectionScene back to MainMenu");
        }

        [UnityTest]
        public IEnumerator MatchScene_ReentryResetsBallPlayersAndNavigationState()
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/MatchScene.unity");
            yield return null;
            yield return null;
            AssertMatchActorsInitialized("first MatchScene entry");

            Time.timeScale = 0f;
            AudioListener.volume = 0f;
            SetStaticField("PauseController", "isPaused", true);
            SetStaticField("Player", "noControls", true);
            InvokeSceneNavigation("2ndTeamSelection");
            yield return WaitForActiveScene("2ndTeamSelection");

            Assert.AreEqual(1f, Time.timeScale, "Back navigation did not restore Time.timeScale.");
            Assert.AreEqual(1f, AudioListener.volume, "Back navigation did not restore audio volume.");
            Assert.IsFalse(
                GetStaticField<bool>("PauseController", "isPaused"),
                "Back navigation left the game paused.");
            Assert.IsFalse(
                GetStaticField<bool>("Player", "noControls"),
                "Back navigation left player controls disabled.");

            InvokeSceneNavigation("MatchScene");
            yield return WaitForActiveScene("MatchScene");
            yield return null;
            yield return null;
            AssertMatchActorsInitialized("MatchScene re-entry");
            AssertNoUnexpectedLogs("MatchScene round trip");
        }

        [UnityTest]
        public IEnumerator QuickMatchSceneFlow_LoadsWithoutErrors()
        {
            yield return RunSceneFlow(
                "Quick Match",
                "MainMenu",
                "GameSelectionScene",
                "1stTeamSelection",
                "2ndTeamSelection",
                "KickOffScene",
                "MatchScene",
                "FinalCeleberation",
                "MainMenu");
        }

        [UnityTest]
        public IEnumerator TournamentSceneFlow_LoadsWithoutErrors()
        {
            yield return RunSceneFlow(
                "Tournament",
                "MainMenu",
                "GameSelectionScene",
                "1stTeamSelection",
                "GroupsScene",
                "MatchesScene",
                "KickOffScene",
                "MatchScene",
                "FinalCeleberation",
                "MainMenu");
        }

        [UnityTest]
        public IEnumerator QuickMatchFlow_TransitionsWithoutErrors()
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/MainMenu.unity");

            yield return InvokeButtonAndWaitForScene("Play", "GameSelectionScene");
            yield return InvokeButtonAndWaitForScene("Back", "MainMenu");
            yield return InvokeButtonAndWaitForScene("Play", "GameSelectionScene");
            yield return InvokeButtonAndWaitForScene("QuickMatch", "1stTeamSelection");
            yield return InvokeButtonAndWaitForScene("Next", "2ndTeamSelection");
            yield return InvokeButtonAndWaitForScene("Next", "KickOffScene");
            yield return InvokeButtonAndWaitForScene("KickOff", "MatchScene");
            yield return InvokeButtonAndWaitForScene("MainMenu", "MainMenu");

            AssertNoUnexpectedLogs("Quick Match flow");
        }

        [UnityTest]
        public IEnumerator TournamentFlow_TransitionsWithoutErrors()
        {
            yield return ResetPersistentSceneState();
            PlayerPrefs.SetInt("HasPendingCup", 0);
            PlayerPrefs.SetInt("matchNumber", 1);
            BeginLogCapture();
            yield return LoadScene("Assets/Scenes/MainMenu.unity");

            yield return InvokeButtonAndWaitForScene("Play", "GameSelectionScene");
            yield return InvokeButtonAndWaitForScene("InternationalCup", "1stTeamSelection");
            yield return InvokeButtonAndWaitForScene("Back", "GameSelectionScene");
            yield return InvokeButtonAndWaitForScene("InternationalCup", "1stTeamSelection");
            yield return InvokeButtonAndWaitForScene("Next", "GroupsScene");
            yield return InvokeButtonAndWaitForScene("Next", "MatchesScene");
            yield return InvokeButtonAndWaitForScene("Next", "KickOffScene");
            yield return InvokeButtonAndWaitForScene("KickOff", "MatchScene");

            PlayerPrefs.SetInt("matchNumber", 8);
            yield return InvokeButtonAndWaitForScene("MainMenu", "FinalCeleberation");
            yield return InvokeButtonAndWaitForScene("MainMenu", "MainMenu");

            AssertNoUnexpectedLogs("Tournament flow");
        }

        private IEnumerator RunScene(string scenePath)
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();
            yield return LoadScene(scenePath);

            float deadline = Time.realtimeSinceStartup + SceneRunDurationSeconds;
            while (Time.realtimeSinceStartup < deadline)
                yield return null;

            AssertNoUnexpectedLogs(scenePath);
            yield return ResetPersistentSceneState();
        }

        private IEnumerator RunSceneFlow(string scope, params string[] sceneNames)
        {
            yield return ResetPersistentSceneState();
            BeginLogCapture();

            foreach (string sceneName in sceneNames)
            {
                InvokeSceneNavigation(sceneName);
                yield return WaitForActiveScene(sceneName);
            }

            AssertNoUnexpectedLogs(scope + " scene flow");
            yield return ResetPersistentSceneState();
        }

        private static IEnumerator LoadScene(string scenePath)
        {
            Assert.IsTrue(
                AssetDatabase.LoadAssetAtPath<SceneAsset>(scenePath) != null,
                "Scene asset does not exist: " + scenePath);

            AsyncOperation operation = EditorSceneManager.LoadSceneAsyncInPlayMode(
                scenePath,
                new LoadSceneParameters(LoadSceneMode.Single));
            Assert.IsNotNull(operation, "Could not start loading scene " + scenePath + ".");
            yield return operation;
            yield return null;
            yield return null;

            Assert.IsTrue(
                SceneManager.GetActiveScene().IsValid(),
                "No valid active scene after loading " + scenePath + ".");

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

        private static IEnumerator WaitForActiveScene(string sceneName)
        {
            float deadline = Time.realtimeSinceStartup + 10f;
            while (SceneManager.GetActiveScene().name != sceneName &&
                   Time.realtimeSinceStartup < deadline)
            {
                yield return null;
            }

            Assert.AreEqual(
                sceneName,
                SceneManager.GetActiveScene().name,
                "Timed out waiting for scene " + sceneName + ".");
            yield return null;
            yield return null;
        }

        private static void InvokeSceneNavigation(string sceneName)
        {
            Type buttonActionType = FindType("ButtonAction");
            Assert.IsNotNull(buttonActionType, "ButtonAction type was not found.");

            MethodInfo loadMethod = buttonActionType.GetMethod(
                "LoadSceneWithReset",
                BindingFlags.NonPublic | BindingFlags.Static);
            Assert.IsNotNull(loadMethod, "ButtonAction.LoadSceneWithReset was not found.");
            loadMethod.Invoke(null, new object[] { sceneName });
        }

        private static void AssertMatchActorsInitialized(string scope)
        {
            Assert.AreEqual(1f, Time.timeScale, scope + " started with a frozen time scale.");
            Assert.IsFalse(
                GetStaticField<bool>("PauseController", "isPaused"),
                scope + " started paused.");
            Assert.IsFalse(
                GetStaticField<bool>("Player", "noControls"),
                scope + " started with player controls disabled.");

            AssertAiOffsetsInitialized("AI_Striker", scope);
            AssertAiOffsetsInitialized("AI_MidfielderScript", scope);
            AssertAiOffsetsInitialized("AI_DefenderScript", scope);

            GameObject ball = GameObject.FindGameObjectWithTag("TheSoccerBall");
            Assert.IsNotNull(ball, scope + " did not contain a soccer ball.");
            Rigidbody body = ball.GetComponent<Rigidbody>();
            Assert.IsNotNull(body, scope + " ball did not contain a Rigidbody.");
            Assert.IsFalse(body.isKinematic, scope + " ball Rigidbody was kinematic.");

            Type ballScriptType = FindType("BallScript");
            Component ballScript = ball.GetComponent(ballScriptType);
            Assert.IsNotNull(ballScript, scope + " ball did not contain BallScript.");
            Transform owner = (Transform)ballScriptType
                .GetField("ownerPlayer", BindingFlags.Public | BindingFlags.Instance)
                .GetValue(ballScript);
            if (owner != null)
            {
                Assert.IsTrue(
                    owner.gameObject.scene.IsValid() && owner.gameObject.scene.isLoaded,
                    scope + " ball retained an owner from an unloaded scene.");
            }
        }

        private static void AssertAiOffsetsInitialized(string typeName, string scope)
        {
            Type type = FindType(typeName);
            Assert.IsNotNull(type, typeName + " type was not found.");

            FieldInfo xOffset = type.GetField(
                "xOffset",
                BindingFlags.NonPublic | BindingFlags.Instance);
            FieldInfo zOffset = type.GetField(
                "zOffset",
                BindingFlags.NonPublic | BindingFlags.Instance);
            Assert.IsNotNull(xOffset, typeName + ".xOffset was not found.");
            Assert.IsNotNull(zOffset, typeName + ".zOffset was not found.");

            UnityEngine.Object[] objects = Resources.FindObjectsOfTypeAll(type);
            Component[] sceneComponents = objects
                .OfType<Component>()
                .Where(component =>
                    component.gameObject.scene.IsValid() &&
                    component.gameObject.scene.isLoaded)
                .ToArray();
            Assert.IsNotEmpty(sceneComponents, scope + " did not contain " + typeName + ".");

            foreach (Component component in sceneComponents)
            {
                int x = (int)xOffset.GetValue(component);
                int z = (int)zOffset.GetValue(component);
                Assert.IsFalse(
                    x == 0 && z == 0,
                    scope + " left " + component.name + " without an AI position offset.");
            }
        }

        private static IEnumerator InvokeButtonAndWaitForScene(
            string buttonName,
            string expectedSceneName)
        {
            Type componentType = FindType("ButtonAction");
            Assert.IsNotNull(componentType, "ButtonAction type was not found.");

            Type enumType = componentType.GetNestedType("Buttons", BindingFlags.Public);
            Assert.IsNotNull(enumType, "ButtonAction.Buttons enum was not found.");
            object expectedButtonValue = Enum.Parse(enumType, buttonName);

            FieldInfo buttonTypeField = componentType.GetField(
                "buttonType",
                BindingFlags.Public | BindingFlags.Instance);
            MethodInfo invokeMethod = componentType.GetMethod(
                "OnMouseUpAsButton",
                BindingFlags.NonPublic | BindingFlags.Instance);
            Assert.IsNotNull(buttonTypeField, "ButtonAction.buttonType field was not found.");
            Assert.IsNotNull(invokeMethod, "ButtonAction.OnMouseUpAsButton method was not found.");

            MonoBehaviour target = UnityEngine.Object
                .FindObjectsByType<MonoBehaviour>(FindObjectsInactive.Include)
                .FirstOrDefault(behaviour =>
                    behaviour != null &&
                    behaviour.GetType() == componentType &&
                    Equals(buttonTypeField.GetValue(behaviour), expectedButtonValue));
            Assert.IsNotNull(
                target,
                $"No {buttonName} ButtonAction exists in scene {SceneManager.GetActiveScene().name}.");

            invokeMethod.Invoke(target, null);

            float deadline = Time.realtimeSinceStartup + 15f;
            while (SceneManager.GetActiveScene().name != expectedSceneName &&
                   Time.realtimeSinceStartup < deadline)
            {
                yield return null;
            }

            Assert.AreEqual(
                expectedSceneName,
                SceneManager.GetActiveScene().name,
                $"The {buttonName} action did not load the expected scene.");
            yield return null;
            yield return null;
        }

        private static IEnumerator ResetPersistentSceneState()
        {
            Time.timeScale = 1f;
            AudioListener.volume = 1f;

            foreach (MonoBehaviour behaviour in
                     UnityEngine.Object.FindObjectsByType<MonoBehaviour>(FindObjectsInactive.Include))
            {
                if (behaviour != null && behaviour.GetType().Name == "AudioManager")
                    UnityEngine.Object.Destroy(behaviour.gameObject);
            }

            SetStaticField("AudioManager", "au", null);
            SetStaticField("AudioManager", "isMusicOn", false);
            SetStaticField("AudioManager", "isSFXOn", false);
            SetStaticField("GameManager", "sharedObject", null);
            SetStaticField("PauseController", "isPaused", false);
            SetStaticField("Player", "noControls", false);
            SetStaticField("Player", "ballGetableDistance", 0.5f);
            SetStaticField("PlayerPosition", "PlayerTurn", true);
            SetStaticField("InitGame", "halfComplete", false);
            SetStaticField("InitGame", "matchcomplete", false);
            SetStaticField("TeamSelectionController", "teamIndex", 0);
            SetStaticField("TeamSelectionController2", "teamIndex", 1);
            ResetPositionAvailability("AI_Striker", 4);
            ResetPositionAvailability("AI_MidfielderScript", 4);
            ResetPositionAvailability("AI_DefenderScript", 3);

            PrepareDeterministicPlayerPrefs();
            yield return null;
            yield return null;
        }

        private static void PrepareDeterministicPlayerPrefs()
        {
            PlayerPrefs.SetInt("jtsounds", 1);
            PlayerPrefs.SetInt("isMusicOn", 1);
            PlayerPrefs.SetInt("isSFXOn", 1);
            PlayerPrefs.SetInt("HasPendingCup", 0);
            PlayerPrefs.SetInt("lost", 0);
            PlayerPrefs.SetInt("matchNumber", 1);
            PlayerPrefs.SetInt("playerTeamIndex", 0);

            for (int index = 1; index <= 7; index++)
            {
                PlayerPrefs.SetInt("match" + index + "TeamIndex", index);
                PlayerPrefs.SetInt("match" + index + "score1", -1);
                PlayerPrefs.SetInt("match" + index + "score2", -1);
            }
        }

        private static void ResetPositionAvailability(string typeName, int positionCount)
        {
            for (int index = 1; index <= positionCount; index++)
                SetStaticField(typeName, "Position" + index + "Available", true);
        }

        private static void SetStaticField(string typeName, string fieldName, object value)
        {
            Type type = FindType(typeName);
            FieldInfo field = type?.GetField(
                fieldName,
                BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
            field?.SetValue(null, value);
        }

        private static T GetStaticField<T>(string typeName, string fieldName)
        {
            Type type = FindType(typeName);
            FieldInfo field = type?.GetField(
                fieldName,
                BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
            Assert.IsNotNull(field, typeName + "." + fieldName + " was not found.");
            return (T)field.GetValue(null);
        }

        private static T GetStaticProperty<T>(string typeName, string propertyName)
        {
            Type type = FindType(typeName);
            PropertyInfo property = type?.GetProperty(
                propertyName,
                BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);
            Assert.IsNotNull(property, typeName + "." + propertyName + " was not found.");
            return (T)property.GetValue(null);
        }

        private void BeginLogCapture()
        {
            unexpectedLogs.Clear();
            Application.logMessageReceived -= CaptureUnexpectedLog;
            Application.logMessageReceived += CaptureUnexpectedLog;
        }

        private void StopLogCapture()
        {
            Application.logMessageReceived -= CaptureUnexpectedLog;
        }

        private void CaptureUnexpectedLog(string condition, string stackTrace, LogType type)
        {
            if (type == LogType.Error || type == LogType.Assert || type == LogType.Exception)
                unexpectedLogs.Add(new RuntimeLog(type, condition, stackTrace));
        }

        private void AssertNoUnexpectedLogs(string scope)
        {
            StopLogCapture();
            if (unexpectedLogs.Count == 0)
                return;

            string details = string.Join(
                "\n\n",
                unexpectedLogs.Select(log => log.ToString()).ToArray());
            Assert.Fail(scope + " produced unexpected Unity console entries:\n" + details);
        }

        private static Type FindType(string typeName)
        {
            foreach (Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                Type type = assembly.GetType(typeName, false);
                if (type != null)
                    return type;
            }

            return null;
        }

        private readonly struct PlayerPrefSnapshot
        {
            public PlayerPrefSnapshot(bool existed, int value)
            {
                Existed = existed;
                IsString = false;
                IntValue = value;
                StringValue = null;
            }

            public PlayerPrefSnapshot(bool existed, string value)
            {
                Existed = existed;
                IsString = true;
                IntValue = 0;
                StringValue = value;
            }

            public bool Existed { get; }
            public bool IsString { get; }
            public int IntValue { get; }
            public string StringValue { get; }
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
