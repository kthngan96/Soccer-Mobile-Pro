using UnityEngine;

[ExecuteAlways]
public class GUITexture : MonoBehaviour
{
    public Texture texture;
    public Rect pixelInset;
    public Color color = Color.white;

    public Rect GetScreenRect()
    {
        Vector3 position = transform.position;
        Vector3 scale = transform.lossyScale;
        float width = pixelInset.width;
        float height = pixelInset.height;

        if (Mathf.Approximately(width, 0f))
        {
            width = Mathf.Abs(scale.x) * Screen.width;
        }

        if (Mathf.Approximately(height, 0f))
        {
            height = Mathf.Abs(scale.y) * Screen.height;
        }

        float left = (position.x * Screen.width) + pixelInset.x - (width * 0.5f);
        float bottom = (position.y * Screen.height) + pixelInset.y - (height * 0.5f);
        return PixelAlign(new Rect(left, bottom, width, height));
    }

    public Rect GetGuiRect()
    {
        Rect screenRect = GetScreenRect();
        return PixelAlign(new Rect(screenRect.x, Screen.height - screenRect.y - screenRect.height, screenRect.width, screenRect.height));
    }

    public bool HitTest(Vector3 screenPosition)
    {
        return gameObject.activeInHierarchy && enabled && GetScreenRect().Contains(screenPosition);
    }

    private void OnGUI()
    {
        if (!LegacyGuiRuntimeRenderer.IsRenderingThroughRuntime)
        {
            DrawOnGUI();
        }
    }

    public void DrawOnGUI()
    {
        if (Event.current.type != EventType.Repaint || !gameObject.activeInHierarchy)
        {
            return;
        }

        Texture drawTexture = ResolveTexture();
        if (drawTexture == null)
        {
            return;
        }

        Color previousColor = GUI.color;
        GUI.color = LegacyGuiUtility.ResolveLegacyRenderColor(color, true);
        GUI.DrawTexture(GetGuiRect(), drawTexture, ScaleMode.StretchToFill, true);
        GUI.color = previousColor;
    }

    private Texture ResolveTexture()
    {
        if (texture != null)
        {
            return texture;
        }

        Texture buttonTexture = ResolveTextureFromButtonController();
        if (buttonTexture != null)
        {
            return buttonTexture;
        }

        Renderer objectRenderer = GetComponent<Renderer>();
        if (objectRenderer != null && objectRenderer.sharedMaterial != null)
        {
            return objectRenderer.sharedMaterial.mainTexture;
        }

        return null;
    }

    private Texture ResolveTextureFromButtonController()
    {
        MonoBehaviour[] behaviours = GetComponents<MonoBehaviour>();
        foreach (MonoBehaviour behaviour in behaviours)
        {
            if (behaviour == null || behaviour.GetType().Name != "ButtonController")
            {
                continue;
            }

            Texture normalTexture = GetTextureField(behaviour, "normalTexture");
            if (normalTexture != null)
            {
                return normalTexture;
            }

            Texture hoverTexture = GetTextureField(behaviour, "hoverTexture");
            if (hoverTexture != null)
            {
                return hoverTexture;
            }
        }

        return null;
    }

    private static Texture GetTextureField(MonoBehaviour behaviour, string fieldName)
    {
        System.Reflection.FieldInfo field = behaviour.GetType().GetField(fieldName);
        if (field == null)
        {
            return null;
        }

        return field.GetValue(behaviour) as Texture;
    }

    private static Rect PixelAlign(Rect rect)
    {
        return new Rect(
            Mathf.Round(rect.x),
            Mathf.Round(rect.y),
            Mathf.Round(rect.width),
            Mathf.Round(rect.height));
    }
}

public static class LegacyGuiUtility
{
    public static Color ResolveLegacyRenderColor(Color color, bool textureTint = false)
    {
        if (Mathf.Abs(color.r - 0.5f) <= 0.02f
            && Mathf.Abs(color.g - 0.5f) <= 0.02f
            && Mathf.Abs(color.b - 0.5f) <= 0.02f
            && Mathf.Abs(color.a - 0.5f) <= 0.02f)
        {
            return Color.white;
        }

        if (textureTint && IsNeutralLegacyTextureTint(color))
        {
            return Color.white;
        }

        return color;
    }

    private static bool IsNeutralLegacyTextureTint(Color color)
    {
        float maxChannel = Mathf.Max(color.r, Mathf.Max(color.g, color.b));
        float minChannel = Mathf.Min(color.r, Mathf.Min(color.g, color.b));
        bool neutralGray = maxChannel - minChannel <= 0.06f && color.r >= 0.45f && color.r <= 0.58f;
        bool oldGuiAlpha = Mathf.Abs(color.a - 0.5f) <= 0.08f || color.a >= 0.9f;
        return neutralGray && oldGuiAlpha;
    }

    public static GUITexture GetOrAddGUITexture(Component component)
    {
        return component == null ? null : GetOrAddGUITexture(component.gameObject);
    }

    public static GUITexture GetOrAddGUITexture(GameObject gameObject)
    {
        if (gameObject == null)
        {
            return null;
        }

        GUITexture guiTexture = gameObject.GetComponent<GUITexture>();
        return guiTexture == null ? gameObject.AddComponent<GUITexture>() : guiTexture;
    }

    public static GUIText GetOrAddGUIText(Component component)
    {
        return component == null ? null : GetOrAddGUIText(component.gameObject);
    }

    public static GUIText GetOrAddGUIText(GameObject gameObject)
    {
        if (gameObject == null)
        {
            return null;
        }

        GUIText guiText = gameObject.GetComponent<GUIText>();
        return guiText == null ? gameObject.AddComponent<GUIText>() : guiText;
    }
}
