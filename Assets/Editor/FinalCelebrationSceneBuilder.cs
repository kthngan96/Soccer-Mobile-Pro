using System;
using System.Linq;
using TMPro;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public static class FinalCelebrationSceneBuilder
{
    private const string ScenePath = "Assets/Scenes/FinalCeleberation.unity";
    private static readonly Color Navy = new Color32(6, 18, 43, 255);
    private static readonly Color DeepNavy = new Color32(3, 10, 25, 255);
    private static readonly Color Gold = new Color32(255, 190, 35, 255);
    private static readonly Color PaleGold = new Color32(255, 220, 126, 255);
    private static readonly Color White = new Color32(246, 248, 252, 255);
    private static TMP_FontAsset fontAsset;

    [MenuItem("Tools/Soccer Mobile/Rebuild Final Celebration Scene")]
    public static void Build()
    {
        Scene scene = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Single);
        fontAsset = AssetDatabase.LoadAssetAtPath<TMP_FontAsset>(
            "Assets/TextMesh Pro/Resources/Fonts & Materials/LiberationSans SDF.asset");

        RemoveOldPresentation(scene);

        GameObject[] roots = scene.GetRootGameObjects();
        GameObject milton = roots.FirstOrDefault(root => root.name == "Milton@t-pose_1");
        GameObject[] dancers = roots
            .Where(root => root.name == "Second_Life_Female@t-pose_2")
            .OrderBy(root => root.transform.position.x)
            .ToArray();
        if (milton == null || dancers.Length != 2)
            throw new InvalidOperationException(
                "FinalCeleberation must contain Milton and exactly two female dancers.");

        GameObject[] characters = { dancers[0], milton, dancers[1] };
        Animator[] animators = characters.Select(ConfigureAnimator).ToArray();
        ConfigureCharacters(dancers[0], milton, dancers[1]);

        Camera sceneCamera = ConfigureCamera();
        Light keyLight = ConfigureKeyLight();
        Light fillLight = CreateFillLight();

        GameObject canvasObject = CreateObject(
            "Final Celebration Canvas",
            null,
            typeof(RectTransform),
            typeof(Canvas),
            typeof(CanvasScaler),
            typeof(GraphicRaycaster));
        Canvas canvas = canvasObject.GetComponent<Canvas>();
        canvas.renderMode = RenderMode.ScreenSpaceOverlay;
        canvas.sortingOrder = 50;

        CanvasScaler scaler = canvasObject.GetComponent<CanvasScaler>();
        scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
        scaler.referenceResolution = new Vector2(1920f, 1080f);
        scaler.screenMatchMode = CanvasScaler.ScreenMatchMode.MatchWidthOrHeight;
        scaler.matchWidthOrHeight = 0.5f;

        RectTransform canvasRect = canvasObject.GetComponent<RectTransform>();
        Stretch(canvasRect);

        Image screenTint = CreateImage(
            "Broadcast Tint",
            canvasRect,
            new Color32(6, 18, 43, 160));
        Stretch(screenTint.rectTransform);

        Image headerBackdrop = CreateImage(
            "Header Backdrop",
            canvasRect,
            new Color32(3, 10, 25, 112));
        SetRect(headerBackdrop.rectTransform, new Vector2(0f, 1f), new Vector2(1f, 1f),
            Vector2.zero, new Vector2(0f, 356f), new Vector2(0.5f, 1f));

        Image topBand = CreateImage(
            "Top Broadcast Band",
            canvasRect,
            new Color32(3, 10, 25, 222));
        SetRect(topBand.rectTransform, new Vector2(0f, 1f), new Vector2(1f, 1f),
            Vector2.zero, new Vector2(0f, 82f), new Vector2(0.5f, 1f));

        Image bottomBand = CreateImage(
            "Bottom Broadcast Band",
            canvasRect,
            new Color32(3, 10, 25, 235));
        SetRect(bottomBand.rectTransform, new Vector2(0f, 0f), new Vector2(1f, 0f),
            Vector2.zero, new Vector2(0f, 154f), new Vector2(0.5f, 0f));

        Image topGoldLine = CreateImage("Top Gold Line", canvasRect, Gold);
        SetRect(topGoldLine.rectTransform, new Vector2(0f, 1f), new Vector2(1f, 1f),
            new Vector2(0f, -80f), new Vector2(0f, 4f), new Vector2(0.5f, 1f));

        RectTransform safeArea = CreateRect("Safe Area", canvasRect);
        Stretch(safeArea);

        TMP_Text eyebrow = CreateText(
            "Eyebrow",
            safeArea,
            "INTERNATIONAL CUP  •  TROPHY CEREMONY",
            24f,
            PaleGold,
            FontStyles.Bold,
            TextAlignmentOptions.Center);
        SetRect(eyebrow.rectTransform, new Vector2(0.5f, 1f), new Vector2(0.5f, 1f),
            new Vector2(0f, -112f), new Vector2(1040f, 42f), new Vector2(0.5f, 1f));
        eyebrow.characterSpacing = 5f;

        TMP_Text title = CreateText(
            "Title",
            safeArea,
            "CHAMPIONS!",
            94f,
            new Color32(255, 207, 75, 255),
            FontStyles.Bold,
            TextAlignmentOptions.Center);
        SetRect(title.rectTransform, new Vector2(0.5f, 1f), new Vector2(0.5f, 1f),
            new Vector2(0f, -195f), new Vector2(1300f, 132f), new Vector2(0.5f, 1f));
        title.enableAutoSizing = true;
        title.fontSizeMin = 52f;
        title.fontSizeMax = 104f;
        title.characterSpacing = 2f;
        Shadow titleShadow = title.gameObject.AddComponent<Shadow>();
        titleShadow.effectColor = new Color(0f, 0f, 0f, 0.72f);
        titleShadow.effectDistance = new Vector2(4f, -5f);

        Image accentBar = CreateImage("Title Accent", safeArea, Gold);
        SetRect(accentBar.rectTransform, new Vector2(0.5f, 1f), new Vector2(0.5f, 1f),
            new Vector2(0f, -270f), new Vector2(250f, 6f), new Vector2(0.5f, 1f));

        TMP_Text message = CreateText(
            "Result Message",
            safeArea,
            "Congratulations!\nYou won the International Cup.",
            30f,
            White,
            FontStyles.Normal,
            TextAlignmentOptions.Center);
        SetRect(message.rectTransform, new Vector2(0.5f, 1f), new Vector2(0.5f, 1f),
            new Vector2(0f, -324f), new Vector2(940f, 88f), new Vector2(0.5f, 1f));
        message.enableAutoSizing = true;
        message.fontSizeMin = 21f;
        message.fontSizeMax = 32f;
        message.lineSpacing = 8f;

        RectTransform confettiLayer = CreateRect("Confetti Layer", canvasRect);
        Stretch(confettiLayer);
        FinalCelebrationConfetti confetti =
            confettiLayer.gameObject.AddComponent<FinalCelebrationConfetti>();
        confettiLayer.SetAsLastSibling();

        TMP_Text winnerBadge = CreateBadge(
            "Winner Badge",
            safeArea,
            "•   WORLD CHAMPIONS   •",
            Gold,
            new Vector2(0f, 182f));

        TMP_Text lossBadge = CreateBadge(
            "Loss Badge",
            safeArea,
            "THANK YOU FOR PLAYING",
            new Color32(184, 196, 216, 255),
            new Vector2(0f, 182f));
        lossBadge.gameObject.SetActive(false);

        Button mainMenuButton = CreateMainMenuButton(safeArea);

        EnsureEventSystem();

        GameObject controllerObject = new GameObject("FinalCelebration");
        SceneManager.MoveGameObjectToScene(controllerObject, scene);
        FinalCelebrationController controller =
            controllerObject.AddComponent<FinalCelebrationController>();

        SerializedObject serializedController = new SerializedObject(controller);
        Assign(serializedController, "safeAreaRoot", safeArea);
        Assign(serializedController, "screenTint", screenTint);
        Assign(serializedController, "accentBar", accentBar);
        Assign(serializedController, "eyebrowText", eyebrow);
        Assign(serializedController, "titleText", title);
        Assign(serializedController, "messageText", message);
        AssignArray(serializedController, "winnerOnlyObjects",
            new UnityEngine.Object[] { winnerBadge.gameObject, confetti.gameObject });
        AssignArray(serializedController, "lossOnlyObjects",
            new UnityEngine.Object[] { lossBadge.gameObject });
        Assign(serializedController, "mainMenuButton", mainMenuButton);
        AssignArray(serializedController, "celebrationCharacters", characters);
        AssignArray(serializedController, "celebrationAnimators", animators);
        Assign(serializedController, "keyLight", keyLight);
        Assign(serializedController, "fillLight", fillLight);
        Assign(serializedController, "sceneCamera", sceneCamera);
        serializedController.ApplyModifiedPropertiesWithoutUndo();

        EditorUtility.SetDirty(controller);
        EditorUtility.SetDirty(canvasObject);
        EditorSceneManager.MarkSceneDirty(scene);
        if (!EditorSceneManager.SaveScene(scene, ScenePath))
            throw new InvalidOperationException("Failed to save " + ScenePath);

        Selection.activeGameObject = controllerObject;
        Debug.Log("FinalCeleberation rebuilt with responsive cup-broadcast presentation.");
    }

    private static void RemoveOldPresentation(Scene scene)
    {
        string[] names =
        {
            "Button",
            "Message",
            "Final Celebration Canvas",
            "FinalCelebration",
            "Final Fill Light",
            "EventSystem"
        };

        foreach (GameObject root in scene.GetRootGameObjects())
        {
            if (names.Contains(root.name))
                UnityEngine.Object.DestroyImmediate(root);
        }
    }

    private static void ConfigureCharacters(
        GameObject left,
        GameObject center,
        GameObject right)
    {
        left.transform.SetPositionAndRotation(
            new Vector3(-1.3f, -0.18f, 0.58f),
            Quaternion.Euler(0f, 180f, 0f));
        left.transform.localScale = Vector3.one * 1.85f;

        center.transform.SetPositionAndRotation(
            new Vector3(0f, -0.18f, 0.82f),
            Quaternion.Euler(0f, 180f, 0f));
        center.transform.localScale = Vector3.one;

        right.transform.SetPositionAndRotation(
            new Vector3(1.3f, -0.18f, 0.58f),
            Quaternion.Euler(0f, 180f, 0f));
        right.transform.localScale = Vector3.one * 1.85f;
    }

    private static Animator ConfigureAnimator(GameObject character)
    {
        Animator animator = character.GetComponent<Animator>();
        if (animator == null)
            throw new InvalidOperationException(character.name + " has no Animator.");

        animator.applyRootMotion = false;
        animator.cullingMode = AnimatorCullingMode.AlwaysAnimate;
        animator.updateMode = AnimatorUpdateMode.Normal;
        EditorUtility.SetDirty(animator);
        return animator;
    }

    private static Camera ConfigureCamera()
    {
        Camera camera = GameObject.Find("Main Camera")?.GetComponent<Camera>();
        if (camera == null)
            throw new InvalidOperationException("FinalCeleberation has no Main Camera.");

        camera.transform.position = new Vector3(0f, 1.48f, -4.65f);
        camera.transform.rotation = Quaternion.LookRotation(
            new Vector3(0f, 1.18f, 0.55f) - camera.transform.position);
        camera.fieldOfView = 40f;
        camera.clearFlags = CameraClearFlags.SolidColor;
        camera.backgroundColor = new Color32(7, 24, 55, 255);
        EditorUtility.SetDirty(camera);
        return camera;
    }

    private static Light ConfigureKeyLight()
    {
        GameObject lightObject = GameObject.Find("Directional light");
        if (lightObject == null)
            throw new InvalidOperationException("FinalCeleberation has no Directional light.");

        Light light = lightObject.GetComponent<Light>();
        light.type = LightType.Directional;
        light.color = new Color32(255, 222, 164, 255);
        light.intensity = 1.15f;
        light.shadows = LightShadows.Soft;
        light.shadowStrength = 0.7f;
        lightObject.transform.rotation = Quaternion.Euler(48f, -32f, 0f);
        EditorUtility.SetDirty(light);
        return light;
    }

    private static Light CreateFillLight()
    {
        GameObject lightObject = new GameObject("Final Fill Light");
        lightObject.transform.position = new Vector3(0f, 4.2f, -2.4f);
        lightObject.transform.rotation = Quaternion.LookRotation(
            new Vector3(0f, 1f, 0.7f) - lightObject.transform.position);
        Light light = lightObject.AddComponent<Light>();
        light.type = LightType.Spot;
        light.color = new Color32(255, 184, 68, 255);
        light.intensity = 1.1f;
        light.range = 11f;
        light.spotAngle = 72f;
        light.innerSpotAngle = 38f;
        light.shadows = LightShadows.Soft;
        return light;
    }

    private static Button CreateMainMenuButton(RectTransform parent)
    {
        Image border = CreateImage("Main Menu Border", parent, Gold);
        SetRect(border.rectTransform, new Vector2(0.5f, 0f), new Vector2(0.5f, 0f),
            new Vector2(0f, 34f), new Vector2(326f, 82f), new Vector2(0.5f, 0f));

        GameObject buttonObject = CreateObject(
            "Main Menu Button",
            border.rectTransform,
            typeof(RectTransform),
            typeof(CanvasRenderer),
            typeof(Image),
            typeof(Button));
        RectTransform buttonRect = buttonObject.GetComponent<RectTransform>();
        Stretch(buttonRect);
        buttonRect.offsetMin = new Vector2(3f, 3f);
        buttonRect.offsetMax = new Vector2(-3f, -3f);

        Image buttonImage = buttonObject.GetComponent<Image>();
        buttonImage.color = Navy;
        Button button = buttonObject.GetComponent<Button>();
        button.targetGraphic = buttonImage;
        button.navigation = new Navigation { mode = Navigation.Mode.None };
        ColorBlock colors = button.colors;
        colors.normalColor = Navy;
        colors.highlightedColor = new Color32(20, 51, 94, 255);
        colors.pressedColor = new Color32(255, 190, 35, 255);
        colors.selectedColor = colors.highlightedColor;
        colors.disabledColor = new Color32(40, 48, 62, 180);
        colors.colorMultiplier = 1f;
        colors.fadeDuration = 0.08f;
        button.colors = colors;

        TMP_Text label = CreateText(
            "Label",
            buttonRect,
            "MAIN MENU",
            27f,
            White,
            FontStyles.Bold,
            TextAlignmentOptions.Center);
        Stretch(label.rectTransform);
        label.characterSpacing = 7f;
        label.raycastTarget = false;
        return button;
    }

    private static TMP_Text CreateBadge(
        string name,
        RectTransform parent,
        string value,
        Color color,
        Vector2 position)
    {
        TMP_Text badge = CreateText(
            name,
            parent,
            value,
            23f,
            color,
            FontStyles.Bold,
            TextAlignmentOptions.Center);
        SetRect(badge.rectTransform, new Vector2(0.5f, 0f), new Vector2(0.5f, 0f),
            position, new Vector2(680f, 48f), new Vector2(0.5f, 0f));
        badge.characterSpacing = 5f;
        return badge;
    }

    private static void EnsureEventSystem()
    {
        GameObject eventSystemObject = new GameObject(
            "EventSystem",
            typeof(EventSystem),
            typeof(StandaloneInputModule));
        eventSystemObject.GetComponent<EventSystem>().sendNavigationEvents = true;
    }

    private static RectTransform CreateRect(string name, RectTransform parent)
    {
        return CreateObject(name, parent, typeof(RectTransform))
            .GetComponent<RectTransform>();
    }

    private static Image CreateImage(string name, RectTransform parent, Color color)
    {
        GameObject target = CreateObject(
            name,
            parent,
            typeof(RectTransform),
            typeof(CanvasRenderer),
            typeof(Image));
        Image image = target.GetComponent<Image>();
        image.color = color;
        image.raycastTarget = false;
        return image;
    }

    private static TMP_Text CreateText(
        string name,
        RectTransform parent,
        string value,
        float size,
        Color color,
        FontStyles style,
        TextAlignmentOptions alignment)
    {
        GameObject target = CreateObject(
            name,
            parent,
            typeof(RectTransform),
            typeof(CanvasRenderer),
            typeof(TextMeshProUGUI));
        TextMeshProUGUI text = target.GetComponent<TextMeshProUGUI>();
        text.text = value;
        text.font = fontAsset;
        text.fontSize = size;
        text.color = color;
        text.fontStyle = style;
        text.alignment = alignment;
        text.enableWordWrapping = true;
        text.overflowMode = TextOverflowModes.Ellipsis;
        text.raycastTarget = false;
        return text;
    }

    private static GameObject CreateObject(
        string name,
        RectTransform parent,
        params Type[] components)
    {
        GameObject target = new GameObject(name, components);
        if (parent != null)
            target.transform.SetParent(parent, false);
        return target;
    }

    private static void Stretch(RectTransform rect)
    {
        rect.anchorMin = Vector2.zero;
        rect.anchorMax = Vector2.one;
        rect.pivot = new Vector2(0.5f, 0.5f);
        rect.offsetMin = Vector2.zero;
        rect.offsetMax = Vector2.zero;
    }

    private static void SetRect(
        RectTransform rect,
        Vector2 anchorMin,
        Vector2 anchorMax,
        Vector2 anchoredPosition,
        Vector2 size,
        Vector2 pivot)
    {
        rect.anchorMin = anchorMin;
        rect.anchorMax = anchorMax;
        rect.pivot = pivot;
        rect.anchoredPosition = anchoredPosition;
        rect.sizeDelta = size;
    }

    private static void Assign(
        SerializedObject serializedObject,
        string propertyName,
        UnityEngine.Object value)
    {
        SerializedProperty property = serializedObject.FindProperty(propertyName);
        if (property == null)
            throw new MissingFieldException(
                serializedObject.targetObject.GetType().Name,
                propertyName);
        property.objectReferenceValue = value;
    }

    private static void AssignArray(
        SerializedObject serializedObject,
        string propertyName,
        UnityEngine.Object[] values)
    {
        SerializedProperty property = serializedObject.FindProperty(propertyName);
        if (property == null)
            throw new MissingFieldException(
                serializedObject.targetObject.GetType().Name,
                propertyName);

        property.arraySize = values.Length;
        for (int index = 0; index < values.Length; index++)
            property.GetArrayElementAtIndex(index).objectReferenceValue = values[index];
    }
}
