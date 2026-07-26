using System;
using System.Collections;
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
    public sealed class FinalCelebrationPlayModeTests
    {
        private const string ScenePath = "Assets/Scenes/FinalCeleberation.unity";
        private bool hadLost;
        private int previousLost;
        private bool hadMessage;
        private string previousMessage;
        private bool hadPendingCup;
        private int previousPendingCup;

        [SetUp]
        public void CapturePlayerPrefs()
        {
            hadLost = PlayerPrefs.HasKey("lost");
            previousLost = PlayerPrefs.GetInt("lost");
            hadMessage = PlayerPrefs.HasKey("message");
            previousMessage = PlayerPrefs.GetString("message");
            hadPendingCup = PlayerPrefs.HasKey("HasPendingCup");
            previousPendingCup = PlayerPrefs.GetInt("HasPendingCup");
        }

        [TearDown]
        public void RestorePlayerPrefs()
        {
            RestoreInt("lost", hadLost, previousLost);
            RestoreString("message", hadMessage, previousMessage);
            RestoreInt("HasPendingCup", hadPendingCup, previousPendingCup);
            PlayerPrefs.Save();
            Time.timeScale = 1f;
            AudioListener.volume = 1f;
        }

        [UnityTest]
        public IEnumerator WinnerState_ShowsCelebrationAndCorrectCopy()
        {
            PlayerPrefs.SetInt("lost", 0);
            PlayerPrefs.SetString(
                "message",
                "Congratulations!\nYou won the International Cup.");
            PlayerPrefs.SetInt("HasPendingCup", 1);
            yield return LoadScene();

            Component controller = FindController();
            Assert.IsFalse(GetProperty<bool>(controller, "IsLoss"));
            Assert.AreEqual(0, PlayerPrefs.GetInt("HasPendingCup"));

            Component title = FindComponent("Title", "TMPro.TMP_Text");
            Component message = FindComponent("Result Message", "TMPro.TMP_Text");
            Assert.AreEqual("CHAMPIONS!", GetProperty<string>(title, "text"));
            StringAssert.Contains(
                "won the International Cup",
                GetProperty<string>(message, "text"));

            GameObject[] characters = FindCelebrationCharacters();
            Assert.AreEqual(3, characters.Length);
            Assert.IsTrue(characters.All(character => character.activeInHierarchy));
            Assert.IsTrue(
                characters.Select(character => character.GetComponent<Animator>())
                    .All(animator =>
                        animator != null &&
                        animator.enabled &&
                        !animator.applyRootMotion));
        }

        [UnityTest]
        public IEnumerator LossState_HidesCelebrationAndUsesLossLayout()
        {
            PlayerPrefs.SetInt("lost", 1);
            PlayerPrefs.SetString(
                "message",
                "Sorry!\nYou lost the International Cup.");
            yield return LoadScene();

            Component controller = FindController();
            Assert.IsTrue(GetProperty<bool>(controller, "IsLoss"));

            Component title = FindComponent("Title", "TMPro.TMP_Text");
            Component message = FindComponent("Result Message", "TMPro.TMP_Text");
            Assert.AreEqual("FINAL RESULT", GetProperty<string>(title, "text"));
            StringAssert.Contains(
                "lost the International Cup",
                GetProperty<string>(message, "text"));
            Assert.IsTrue(
                FindCelebrationCharacters().All(character => !character.activeInHierarchy));
            Assert.IsFalse(GameObject.Find("Winner Badge") != null);
            Assert.IsNotNull(GameObject.Find("Loss Badge"));
        }

        [UnityTest]
        public IEnumerator LandscapeResolutions_KeepPrimaryUiInsideSafeArea()
        {
            PlayerPrefs.SetInt("lost", 0);
            yield return LoadScene();

            int originalWidth = Screen.width;
            int originalHeight = Screen.height;
            Vector2Int[] resolutions =
            {
                new Vector2Int(1024, 768),
                new Vector2Int(1920, 1080),
                new Vector2Int(2340, 1080)
            };

            foreach (Vector2Int resolution in resolutions)
            {
                Screen.SetResolution(resolution.x, resolution.y, false);
                yield return null;
                yield return null;

                AssertRectInsideSafeArea(
                    GameObject.Find("Title").GetComponent<RectTransform>(),
                    resolution + " title");
                AssertRectInsideSafeArea(
                    GameObject.Find("Main Menu Border").GetComponent<RectTransform>(),
                    resolution + " main menu button");
            }

            Screen.SetResolution(originalWidth, originalHeight, false);
            yield return null;
            yield return null;
        }

        [UnityTest]
        public IEnumerator SceneAsset_ContainsModernCanvasWithoutLegacyResultObjects()
        {
            yield return LoadScene();

            Assert.IsNotNull(GameObject.Find("Final Celebration Canvas"));
            Assert.IsNotNull(GameObject.Find("FinalCelebration"));
            Assert.IsNotNull(GameObject.Find("EventSystem"));
            Assert.IsNull(GameObject.Find("Button"));
            Assert.IsNull(GameObject.Find("Message"));

            MonoBehaviour[] behaviours = UnityEngine.Object
                .FindObjectsByType<MonoBehaviour>(FindObjectsInactive.Include);
            Assert.IsFalse(behaviours.Any(behaviour =>
                behaviour != null &&
                (behaviour.GetType().Name == "FinalMessageScript" ||
                 behaviour.GetType().Name == "ButtonController")));
        }

        private static IEnumerator LoadScene()
        {
            AsyncOperation operation = EditorSceneManager.LoadSceneAsyncInPlayMode(
                ScenePath,
                new LoadSceneParameters(LoadSceneMode.Single));
            Assert.IsNotNull(operation);
            yield return operation;
            yield return null;
            yield return null;
        }

        private static Component FindController()
        {
            Type controllerType = FindType("FinalCelebrationController");
            Assert.IsNotNull(controllerType);
            Component controller = UnityEngine.Object
                .FindObjectsByType<Component>(FindObjectsInactive.Include)
                .FirstOrDefault(component =>
                    component != null &&
                    component.GetType() == controllerType);
            Assert.IsNotNull(controller);
            return controller;
        }

        private static void AssertRectInsideSafeArea(RectTransform rect, string scope)
        {
            Assert.IsNotNull(rect, scope + " RectTransform was not found.");
            Vector3[] corners = new Vector3[4];
            rect.GetWorldCorners(corners);
            Rect safeArea = Screen.safeArea;
            const float tolerance = 2f;
            foreach (Vector3 corner in corners)
            {
                Assert.That(
                    corner.x,
                    Is.InRange(safeArea.xMin - tolerance, safeArea.xMax + tolerance),
                    scope + " exceeded the horizontal safe area.");
                Assert.That(
                    corner.y,
                    Is.InRange(safeArea.yMin - tolerance, safeArea.yMax + tolerance),
                    scope + " exceeded the vertical safe area.");
            }
        }

        private static Component FindComponent(string objectName, string typeName)
        {
            GameObject target = GameObject.Find(objectName);
            Assert.IsNotNull(target, objectName + " was not found.");
            Type componentType = FindType(typeName);
            Assert.IsNotNull(componentType, typeName + " was not found.");
            Component component = target.GetComponent(componentType);
            Assert.IsNotNull(component, objectName + " does not contain " + typeName + ".");
            return component;
        }

        private static GameObject[] FindCelebrationCharacters()
        {
            return UnityEngine.Object
                .FindObjectsByType<Animator>(FindObjectsInactive.Include)
                .Where(animator =>
                    animator.gameObject.name == "Milton@t-pose_1" ||
                    animator.gameObject.name == "Second_Life_Female@t-pose_2")
                .Select(animator => animator.gameObject)
                .ToArray();
        }

        private static T GetProperty<T>(Component component, string propertyName)
        {
            PropertyInfo property = component.GetType().GetProperty(
                propertyName,
                BindingFlags.Public | BindingFlags.Instance);
            Assert.IsNotNull(property);
            return (T)property.GetValue(component);
        }

        private static Type FindType(string typeName)
        {
            return AppDomain.CurrentDomain.GetAssemblies()
                .Select(assembly => assembly.GetType(typeName, false))
                .FirstOrDefault(type => type != null);
        }

        private static void RestoreInt(string key, bool existed, int value)
        {
            if (existed)
                PlayerPrefs.SetInt(key, value);
            else
                PlayerPrefs.DeleteKey(key);
        }

        private static void RestoreString(string key, bool existed, string value)
        {
            if (existed)
                PlayerPrefs.SetString(key, value);
            else
                PlayerPrefs.DeleteKey(key);
        }
    }
}
