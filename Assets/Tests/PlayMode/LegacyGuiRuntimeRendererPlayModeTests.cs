using System.Collections;
using System;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

namespace SoccerMobile.Tests.PlayMode
{
    public sealed class LegacyGuiRuntimeRendererPlayModeTests
    {
        [UnityTest]
        public IEnumerator GameSelectionScene_RendersFramesWithoutUnexpectedLogs()
        {
            yield return LoadAndRenderFrames("GameSelectionScene");
        }

        [UnityTest]
        public IEnumerator KickOffScene_RendersFramesWithoutUnexpectedLogs()
        {
            yield return LoadAndRenderFrames("KickOffScene");
        }

        [UnityTest]
        public IEnumerator ResolutionChange_RecreatesResourcesWithoutUnexpectedLogs()
        {
            yield return SceneManager.LoadSceneAsync("GameSelectionScene", LoadSceneMode.Single);
            yield return null;

            int originalWidth = Screen.width;
            int originalHeight = Screen.height;
            Screen.SetResolution(Mathf.Max(320, originalWidth / 2), Mathf.Max(180, originalHeight / 2), false);

            yield return null;
            yield return null;

            Screen.SetResolution(originalWidth, originalHeight, false);
            yield return null;
        }

        private static IEnumerator LoadAndRenderFrames(string sceneName)
        {
            yield return SceneManager.LoadSceneAsync(sceneName, LoadSceneMode.Single);
            yield return null;
            yield return null;
            yield return null;

            Type rendererType = FindType("LegacyGuiRuntimeRenderer");
            Assert.IsNotNull(rendererType, "LegacyGuiRuntimeRenderer type was not found.");
            Assert.IsNotEmpty(
                Resources.FindObjectsOfTypeAll(rendererType),
                "Legacy GUI runtime renderer was not active after loading " + sceneName + ".");
        }

        private static Type FindType(string typeName)
        {
            foreach (System.Reflection.Assembly assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                Type type = assembly.GetType(typeName, false);
                if (type != null)
                {
                    return type;
                }
            }

            return null;
        }
    }
}
