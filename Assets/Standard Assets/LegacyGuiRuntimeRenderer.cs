using System;
using UnityEngine;
using UnityEngine.SceneManagement;

[ExecuteAlways]
[DefaultExecutionOrder(-10000)]
public class LegacyGuiRuntimeRenderer : MonoBehaviour
{
    private const int Legacy3DGuiLayerMask = 1 << 8;
    private const string Legacy3DGuiOverlayShaderName = "Hidden/SoccerGame3D/Legacy3DGuiChromaKey";

    private static LegacyGuiRuntimeRenderer instance;
    private RenderTexture legacy3DGuiTexture;
    private Material legacy3DGuiMaterial;

    public static bool IsRenderingThroughRuntime
    {
        get { return instance != null && instance.isActiveAndEnabled; }
    }

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    private static void EnsureInstanceBeforeSceneLoad()
    {
        EnsureInstance();
    }

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.AfterSceneLoad)]
    private static void EnsureInstanceAfterSceneLoad()
    {
        EnsureInstance();
    }

#if UNITY_EDITOR
    [UnityEditor.InitializeOnLoadMethod]
    private static void EnsureInstanceInEditMode()
    {
        UnityEditor.EditorApplication.delayCall += delegate
        {
            if (!Application.isPlaying)
            {
                EnsureInstance();
            }
        };
    }
#endif

    private static void EnsureInstance()
    {
        if (instance != null)
        {
            return;
        }

        GameObject rendererObject = new GameObject("LegacyGuiRuntimeRenderer");
        if (Application.isPlaying)
        {
            UnityEngine.Object.DontDestroyOnLoad(rendererObject);
        }
        else
        {
            rendererObject.hideFlags = HideFlags.HideAndDontSave;
        }

        instance = rendererObject.AddComponent<LegacyGuiRuntimeRenderer>();
    }

    private void Awake()
    {
        if (instance != null && instance != this)
        {
            DestroyUnityObject(gameObject);
            return;
        }

        instance = this;
        if (Application.isPlaying)
        {
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            gameObject.hideFlags = HideFlags.HideAndDontSave;
        }

        SceneManager.sceneLoaded -= HandleSceneLoaded;
        SceneManager.sceneLoaded += HandleSceneLoaded;
    }

    private void OnDestroy()
    {
        if (instance == this)
        {
            SceneManager.sceneLoaded -= HandleSceneLoaded;
            instance = null;
        }

        if (legacy3DGuiTexture != null)
        {
            legacy3DGuiTexture.Release();
            DestroyUnityObject(legacy3DGuiTexture);
            legacy3DGuiTexture = null;
        }

        if (legacy3DGuiMaterial != null)
        {
            DestroyUnityObject(legacy3DGuiMaterial);
            legacy3DGuiMaterial = null;
        }
    }

    private void HandleSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        EnsureInstance();
    }

    private void OnGUI()
    {
        if (Event.current.type != EventType.Repaint)
        {
            return;
        }

        GUITexture[] textures = UnityEngine.Object.FindObjectsOfType<GUITexture>();
        Array.Sort(textures, CompareLegacyGuiComponents);

        string sceneName = SceneManager.GetActiveScene().name;
        bool kickOffScene = string.Equals(sceneName, "KickOffScene", StringComparison.Ordinal);
        if (kickOffScene)
        {
            DrawLegacyTextures(textures, true);
            DrawLegacy3DGuiOverlay(sceneName);
            DrawLegacyTextures(textures, false);
        }
        else
        {
            DrawLegacyTextures(textures, null);
        }

        GUIText[] texts = UnityEngine.Object.FindObjectsOfType<GUIText>();
        Array.Sort(texts, CompareLegacyGuiComponents);
        DrawLegacyTexts(texts);

        if (!kickOffScene)
        {
            DrawLegacy3DGuiOverlay(sceneName);
        }
    }

    private static void DrawLegacyTextures(GUITexture[] textures, bool? drawKickOffBackgroundOnly)
    {
        foreach (GUITexture legacyTexture in textures)
        {
            if (legacyTexture == null || !legacyTexture.enabled)
            {
                continue;
            }

            if (drawKickOffBackgroundOnly.HasValue && IsKickOffBackgroundTexture(legacyTexture) != drawKickOffBackgroundOnly.Value)
            {
                continue;
            }

            legacyTexture.DrawOnGUI();
        }
    }

    private static void DrawLegacyTexts(GUIText[] texts)
    {
        foreach (GUIText legacyText in texts)
        {
            if (legacyText != null && legacyText.enabled)
            {
                legacyText.DrawOnGUI();
            }
        }
    }

    private static bool IsKickOffBackgroundTexture(GUITexture legacyTexture)
    {
        return legacyTexture != null && legacyTexture.gameObject != null && string.Equals(legacyTexture.gameObject.name, "Background", StringComparison.Ordinal);
    }

    private static Camera FindLegacy3DGuiCamera()
    {
        Camera[] cameras = Camera.allCameras;
        for (int i = 0; i < cameras.Length; i++)
        {
            Camera camera = cameras[i];
            if (camera != null && camera.isActiveAndEnabled && (camera.cullingMask & Legacy3DGuiLayerMask) != 0)
            {
                return camera;
            }
        }

        return null;
    }

    private void DrawLegacy3DGuiOverlay(string sceneName)
    {
        if (!string.Equals(sceneName, "GameSelectionScene", StringComparison.Ordinal) &&
            !string.Equals(sceneName, "KickOffScene", StringComparison.Ordinal))
        {
            return;
        }

        Camera camera = FindLegacy3DGuiCamera();
        if (camera == null)
        {
            return;
        }

        EnsureLegacy3DGuiResources();
        if (legacy3DGuiTexture == null || legacy3DGuiMaterial == null)
        {
            return;
        }

        RenderTexture previousTarget = camera.targetTexture;
        CameraClearFlags previousClearFlags = camera.clearFlags;
        Color previousBackgroundColor = camera.backgroundColor;

        camera.targetTexture = legacy3DGuiTexture;
        camera.clearFlags = CameraClearFlags.SolidColor;
        camera.backgroundColor = previousBackgroundColor;
        camera.Render();
        camera.targetTexture = previousTarget;
        camera.clearFlags = previousClearFlags;
        camera.backgroundColor = previousBackgroundColor;

        legacy3DGuiMaterial.SetColor("_KeyColor", previousBackgroundColor);
        Graphics.DrawTexture(new Rect(0f, 0f, Screen.width, Screen.height), legacy3DGuiTexture, legacy3DGuiMaterial);
    }

    private void EnsureLegacy3DGuiResources()
    {
        int width = Mathf.Max(1, Screen.width);
        int height = Mathf.Max(1, Screen.height);
        if (legacy3DGuiTexture == null || legacy3DGuiTexture.width != width || legacy3DGuiTexture.height != height)
        {
            if (legacy3DGuiTexture != null)
            {
                legacy3DGuiTexture.Release();
                DestroyUnityObject(legacy3DGuiTexture);
            }

            legacy3DGuiTexture = new RenderTexture(width, height, 24, RenderTextureFormat.ARGB32)
            {
                name = "Legacy3DGuiOverlay"
            };
            legacy3DGuiTexture.Create();
        }

        if (legacy3DGuiMaterial == null)
        {
            Shader shader = Shader.Find(Legacy3DGuiOverlayShaderName);
            if (shader != null)
            {
                legacy3DGuiMaterial = new Material(shader)
                {
                    hideFlags = HideFlags.HideAndDontSave
                };
                legacy3DGuiMaterial.SetFloat("_Tolerance", 0.08f);
                legacy3DGuiMaterial.SetFloat("_Softness", 0.06f);
            }
        }
    }

    private static int CompareLegacyGuiComponents(Component left, Component right)
    {
        if (left == right)
        {
            return 0;
        }

        if (left == null)
        {
            return -1;
        }

        if (right == null)
        {
            return 1;
        }

        int zCompare = left.transform.position.z.CompareTo(right.transform.position.z);
        if (zCompare != 0)
        {
            return zCompare;
        }

        return string.CompareOrdinal(GetHierarchySortKey(left.transform), GetHierarchySortKey(right.transform));
    }

    private static string GetHierarchySortKey(Transform transform)
    {
        if (transform == null)
        {
            return string.Empty;
        }

        string key = transform.GetSiblingIndex().ToString("D6");
        Transform parent = transform.parent;
        while (parent != null)
        {
            key = parent.GetSiblingIndex().ToString("D6") + "/" + key;
            parent = parent.parent;
        }

        return key;
    }

    private static void DestroyUnityObject(UnityEngine.Object target)
    {
        if (target == null)
        {
            return;
        }

        if (Application.isPlaying)
        {
            Destroy(target);
        }
        else
        {
            DestroyImmediate(target);
        }
    }
}
