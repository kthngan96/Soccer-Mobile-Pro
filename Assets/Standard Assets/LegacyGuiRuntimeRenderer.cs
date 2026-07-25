using System;
using UnityEngine;
using UnityEngine.Rendering;
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
    private Camera legacy3DGuiCamera;

    internal static bool DrewLegacy3DGuiOverlayLastRepaint { get; private set; }

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

        if (legacy3DGuiCamera != null)
        {
            DestroyUnityObject(legacy3DGuiCamera.gameObject);
            legacy3DGuiCamera = null;
        }
    }

    private void HandleSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        DrewLegacy3DGuiOverlayLastRepaint = false;
        EnsureInstance();
    }

    private void OnGUI()
    {
        if (Event.current.type != EventType.Repaint)
        {
            return;
        }

        DrewLegacy3DGuiOverlayLastRepaint = false;

        GUITexture[] textures = UnityEngine.Object.FindObjectsByType<GUITexture>();
        Array.Sort(textures, CompareLegacyGuiComponents);

        string sceneName = SceneManager.GetActiveScene().name;
        bool kickOffScene = string.Equals(sceneName, "KickOffScene", StringComparison.Ordinal);
        if (kickOffScene)
        {
            DrawLegacyTextures(textures, true);
            DrawLegacy3DGuiOverlay();
            DrawLegacyTextures(textures, false);
        }
        else
        {
            DrawLegacyTextures(textures, null);
        }

        GUIText[] texts = UnityEngine.Object.FindObjectsByType<GUIText>();
        Array.Sort(texts, CompareLegacyGuiComponents);
        DrawLegacyTexts(texts);

        if (!kickOffScene && RequiresLegacy3DGuiOverlay(sceneName))
        {
            DrawLegacy3DGuiOverlay();
        }
    }

    private void LateUpdate()
    {
        string sceneName = SceneManager.GetActiveScene().name;
        if (!RequiresLegacy3DGuiOverlay(sceneName))
        {
            SetLegacy3DGuiCameraEnabled(false);
            return;
        }

        ConfigureLegacy3DGuiOverlayCamera();
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

    private Camera FindLegacy3DGuiSourceCamera()
    {
        Camera[] cameras = Camera.allCameras;
        for (int i = 0; i < cameras.Length; i++)
        {
            Camera camera = cameras[i];
            if (camera != null &&
                camera != legacy3DGuiCamera &&
                camera.isActiveAndEnabled &&
                camera.targetTexture == null &&
                (camera.cullingMask & Legacy3DGuiLayerMask) != 0)
            {
                return camera;
            }
        }

        return null;
    }

    private static bool RequiresLegacy3DGuiOverlay(string sceneName)
    {
        return string.Equals(sceneName, "GameSelectionScene", StringComparison.Ordinal) ||
               string.Equals(sceneName, "KickOffScene", StringComparison.Ordinal);
    }

    private void ConfigureLegacy3DGuiOverlayCamera()
    {
        if (SystemInfo.graphicsDeviceType == GraphicsDeviceType.Null)
        {
            SetLegacy3DGuiCameraEnabled(false);
            return;
        }

        Camera sourceCamera = FindLegacy3DGuiSourceCamera();
        if (sourceCamera == null)
        {
            SetLegacy3DGuiCameraEnabled(false);
            return;
        }

        EnsureLegacy3DGuiResources();
        if (legacy3DGuiTexture == null || legacy3DGuiMaterial == null || !legacy3DGuiTexture.IsCreated())
        {
            SetLegacy3DGuiCameraEnabled(false);
            return;
        }

        if (legacy3DGuiCamera == null)
        {
            GameObject cameraObject = new GameObject("Legacy3DGuiOverlayCamera");
            cameraObject.hideFlags = HideFlags.HideAndDontSave;
            cameraObject.transform.SetParent(transform, false);
            legacy3DGuiCamera = cameraObject.AddComponent<Camera>();
        }

        legacy3DGuiCamera.CopyFrom(sourceCamera);
        legacy3DGuiCamera.cullingMask = Legacy3DGuiLayerMask;
        legacy3DGuiCamera.clearFlags = CameraClearFlags.SolidColor;
        legacy3DGuiCamera.targetTexture = legacy3DGuiTexture;
        legacy3DGuiCamera.depth = sourceCamera.depth + 0.01f;
        legacy3DGuiCamera.enabled = true;
        legacy3DGuiMaterial.SetColor("_KeyColor", sourceCamera.backgroundColor);
    }

    private void SetLegacy3DGuiCameraEnabled(bool enabled)
    {
        if (legacy3DGuiCamera != null)
        {
            legacy3DGuiCamera.enabled = enabled;
        }

    }

    private void DrawLegacy3DGuiOverlay()
    {
        DrewLegacy3DGuiOverlayLastRepaint = true;

        if (legacy3DGuiTexture == null || legacy3DGuiMaterial == null)
        {
            return;
        }

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
                name = "Legacy3DGuiOverlay",
                hideFlags = HideFlags.HideAndDontSave
            };
            legacy3DGuiTexture.Create();

            if (legacy3DGuiCamera != null)
            {
                legacy3DGuiCamera.targetTexture = legacy3DGuiTexture;
            }
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
