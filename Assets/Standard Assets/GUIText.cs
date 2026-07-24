using UnityEngine;

[ExecuteAlways]
public class GUIText : MonoBehaviour
{
    public string text;
    public int anchor = 4;
    public int alignment = 1;
    public Vector2 pixelOffset;
    public Color color = Color.white;
    public int fontSize = 16;
    public FontStyle fontStyle = FontStyle.Normal;
    public bool richText = true;

    private GUIStyle style;

    private void OnGUI()
    {
        if (!LegacyGuiRuntimeRenderer.IsRenderingThroughRuntime)
        {
            DrawOnGUI();
        }
    }

    public void DrawOnGUI()
    {
        if (Event.current.type != EventType.Repaint || !gameObject.activeInHierarchy || string.IsNullOrEmpty(text))
        {
            return;
        }

        if (style == null)
        {
            style = new GUIStyle(GUI.skin.label);
        }

        style.normal.textColor = LegacyGuiUtility.ResolveLegacyRenderColor(color);
        style.fontSize = Mathf.Max(1, fontSize);
        style.fontStyle = fontStyle;
        style.richText = richText;
        style.alignment = ResolveTextAnchor();

        Vector2 size = style.CalcSize(new GUIContent(text));
        Rect rect = GetGuiRect(size);
        GUI.Label(rect, text, style);
    }

    private Rect GetGuiRect(Vector2 size)
    {
        Vector3 position = transform.position;
        float x = (position.x * Screen.width) + pixelOffset.x;
        float y = Screen.height - ((position.y * Screen.height) + pixelOffset.y);

        TextAnchor resolvedAnchor = ResolveTextAnchor();
        if (resolvedAnchor == TextAnchor.UpperCenter || resolvedAnchor == TextAnchor.MiddleCenter || resolvedAnchor == TextAnchor.LowerCenter)
        {
            x -= size.x * 0.5f;
        }
        else if (resolvedAnchor == TextAnchor.UpperRight || resolvedAnchor == TextAnchor.MiddleRight || resolvedAnchor == TextAnchor.LowerRight)
        {
            x -= size.x;
        }

        if (resolvedAnchor == TextAnchor.MiddleLeft || resolvedAnchor == TextAnchor.MiddleCenter || resolvedAnchor == TextAnchor.MiddleRight)
        {
            y -= size.y * 0.5f;
        }
        else if (resolvedAnchor == TextAnchor.LowerLeft || resolvedAnchor == TextAnchor.LowerCenter || resolvedAnchor == TextAnchor.LowerRight)
        {
            y -= size.y;
        }

        return PixelAlign(new Rect(x, y, Mathf.Max(size.x + 8f, 32f), Mathf.Max(size.y + 4f, fontSize + 4f)));
    }

    private TextAnchor ResolveTextAnchor()
    {
        if (anchor >= 0 && anchor <= 8)
        {
            return (TextAnchor)anchor;
        }

        if (alignment == 0)
        {
            return TextAnchor.MiddleLeft;
        }

        if (alignment == 2)
        {
            return TextAnchor.MiddleRight;
        }

        return TextAnchor.MiddleCenter;
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
