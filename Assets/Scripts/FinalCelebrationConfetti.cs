using UnityEngine;
using UnityEngine.UI;

[DisallowMultipleComponent]
public sealed class FinalCelebrationConfetti : MonoBehaviour
{
    [SerializeField, Range(12, 80)] private int pieceCount = 42;
    [SerializeField] private float fallSpeed = 155f;
    [SerializeField] private float horizontalDrift = 32f;
    [SerializeField] private Vector2 pieceSize = new Vector2(8f, 22f);

    private static readonly Color32[] Palette =
    {
        new Color32(255, 193, 35, 235),
        new Color32(255, 226, 115, 235),
        new Color32(240, 245, 255, 225),
        new Color32(54, 146, 255, 225)
    };

    private RectTransform[] pieces;
    private float[] speeds;
    private float[] phases;

    private void Awake()
    {
        BuildPieces();
    }

    private void OnEnable()
    {
        if (pieces == null || pieces.Length == 0)
            BuildPieces();
    }

    private void Update()
    {
        if (pieces == null)
            return;

        float delta = Time.unscaledDeltaTime;
        for (int index = 0; index < pieces.Length; index++)
        {
            RectTransform piece = pieces[index];
            if (piece == null)
                continue;

            Vector2 position = piece.anchoredPosition;
            position.y -= speeds[index] * delta;
            position.x += Mathf.Sin(Time.unscaledTime * 1.7f + phases[index]) *
                          horizontalDrift * delta;
            if (position.y < -590f)
            {
                position.y = 590f + Repeat01(index * 0.371f) * 180f;
                position.x = Mathf.Lerp(-900f, 900f, Repeat01(index * 0.613f));
            }

            piece.anchoredPosition = position;
            piece.Rotate(0f, 0f, (55f + index % 5 * 17f) * delta);
        }
    }

    private void BuildPieces()
    {
        for (int childIndex = transform.childCount - 1; childIndex >= 0; childIndex--)
            Destroy(transform.GetChild(childIndex).gameObject);

        pieces = new RectTransform[pieceCount];
        speeds = new float[pieceCount];
        phases = new float[pieceCount];

        for (int index = 0; index < pieceCount; index++)
        {
            GameObject pieceObject = new GameObject(
                "Confetti " + (index + 1),
                typeof(RectTransform),
                typeof(CanvasRenderer),
                typeof(Image));
            RectTransform piece = pieceObject.GetComponent<RectTransform>();
            piece.SetParent(transform, false);
            piece.anchorMin = new Vector2(0.5f, 0.5f);
            piece.anchorMax = new Vector2(0.5f, 0.5f);
            piece.sizeDelta = index % 3 == 0
                ? new Vector2(pieceSize.y, pieceSize.x)
                : pieceSize;
            piece.anchoredPosition = new Vector2(
                Mathf.Lerp(-930f, 930f, Repeat01(index * 0.613f)),
                Mathf.Lerp(-560f, 720f, Repeat01(index * 0.371f)));
            piece.localRotation = Quaternion.Euler(0f, 0f, index * 37f);

            Image image = pieceObject.GetComponent<Image>();
            image.color = Palette[index % Palette.Length];
            image.raycastTarget = false;

            pieces[index] = piece;
            speeds[index] = fallSpeed * Mathf.Lerp(0.72f, 1.35f, Repeat01(index * 0.217f));
            phases[index] = index * 0.73f;
        }
    }

    private static float Repeat01(float value)
    {
        return value - Mathf.Floor(value);
    }
}
