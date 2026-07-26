using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

[DisallowMultipleComponent]
public sealed class FinalCelebrationController : MonoBehaviour
{
    private const string LostKey = "lost";
    private const string MessageKey = "message";
    private const string PendingCupKey = "HasPendingCup";
    private const string WinnerAnimationState = "samba_dancing_2";
    private const string WinnerTitle = "CHAMPIONS!";
    private const string LossTitle = "FINAL RESULT";
    private const string WinnerMessage = "Congratulations!\nYou won the International Cup.";
    private const string LossMessage = "Sorry!\nYou lost the International Cup.";

    [Header("Responsive UI")]
    [SerializeField] private RectTransform safeAreaRoot;
    [SerializeField] private Image screenTint;
    [SerializeField] private Image accentBar;
    [SerializeField] private TMP_Text eyebrowText;
    [SerializeField] private TMP_Text titleText;
    [SerializeField] private TMP_Text messageText;
    [SerializeField] private GameObject[] winnerOnlyObjects;
    [SerializeField] private GameObject[] lossOnlyObjects;
    [SerializeField] private Button mainMenuButton;

    [Header("Celebration")]
    [SerializeField] private GameObject[] celebrationCharacters;
    [SerializeField] private Animator[] celebrationAnimators;
    [SerializeField] private Light keyLight;
    [SerializeField] private Light fillLight;
    [SerializeField] private Camera sceneCamera;

    private Rect lastSafeArea;
    private Vector2Int lastScreenSize;

    public bool IsLoss { get; private set; }

    private void Awake()
    {
        PlayerPrefs.SetInt(PendingCupKey, 0);
        PlayerPrefs.Save();

        if (mainMenuButton != null)
        {
            mainMenuButton.onClick.RemoveListener(ReturnToMainMenu);
            mainMenuButton.onClick.AddListener(ReturnToMainMenu);
        }

        ApplyResult(PlayerPrefs.GetInt(LostKey, 0) == 1);
        ApplySafeArea(true);
    }

    private void Update()
    {
        ApplySafeArea(false);
    }

    public void ReturnToMainMenu()
    {
        InitGame.matchcomplete = false;
        InitGame.halfComplete = false;
        PauseController.isPaused = false;
        Player.noControls = false;
        PlayerPosition.PlayerTurn = true;
        Time.timeScale = 1f;
        AudioListener.volume = 1f;
        SceneManager.LoadScene("MainMenu");
    }

    private void ApplyResult(bool isLoss)
    {
        IsLoss = isLoss;

        SetActive(winnerOnlyObjects, !isLoss);
        SetActive(lossOnlyObjects, isLoss);
        SetActive(celebrationCharacters, !isLoss);

        if (eyebrowText != null)
            eyebrowText.text = isLoss ? "INTERNATIONAL CUP  •  FULL TIME" : "INTERNATIONAL CUP  •  TROPHY CEREMONY";

        if (titleText != null)
        {
            titleText.text = isLoss ? LossTitle : WinnerTitle;
            titleText.color = isLoss
                ? new Color32(235, 239, 246, 255)
                : new Color32(255, 207, 75, 255);
        }

        if (messageText != null)
        {
            messageText.text = GetDisplayMessage(isLoss);
            messageText.color = isLoss
                ? new Color32(194, 203, 218, 255)
                : new Color32(246, 248, 252, 255);
        }

        if (screenTint != null)
        {
            screenTint.color = isLoss
                ? new Color32(5, 12, 29, 224)
                : new Color32(6, 18, 43, 160);
        }

        if (accentBar != null)
        {
            accentBar.color = isLoss
                ? new Color32(154, 168, 190, 230)
                : new Color32(255, 190, 35, 255);
        }

        ConfigureLighting(isLoss);
        ConfigureAnimators(isLoss);
    }

    private string GetDisplayMessage(bool isLoss)
    {
        string storedMessage = PlayerPrefs.GetString(MessageKey, string.Empty).Trim();
        if (string.IsNullOrEmpty(storedMessage) ||
            storedMessage == "dfsdfsdf" ||
            storedMessage.Contains("Congratulation!") ||
            storedMessage.Contains("You loose"))
        {
            return isLoss ? LossMessage : WinnerMessage;
        }

        bool looksLikeLoss = storedMessage.StartsWith("Sorry", System.StringComparison.OrdinalIgnoreCase);
        return looksLikeLoss == isLoss ? storedMessage : (isLoss ? LossMessage : WinnerMessage);
    }

    private void ConfigureAnimators(bool isLoss)
    {
        if (celebrationAnimators == null)
            return;

        foreach (Animator animator in celebrationAnimators)
        {
            if (animator == null)
                continue;

            animator.applyRootMotion = false;
            animator.cullingMode = AnimatorCullingMode.AlwaysAnimate;
            animator.enabled = !isLoss;
            if (!isLoss && animator.runtimeAnimatorController != null)
            {
                animator.Rebind();
                animator.Update(0f);
                animator.Play(WinnerAnimationState, 0, 0f);
            }
        }
    }

    private void ConfigureLighting(bool isLoss)
    {
        if (keyLight != null)
        {
            keyLight.color = isLoss
                ? new Color32(164, 178, 204, 255)
                : new Color32(255, 222, 164, 255);
            keyLight.intensity = isLoss ? 0.55f : 1.15f;
        }

        if (fillLight != null)
        {
            fillLight.color = isLoss
                ? new Color32(79, 104, 145, 255)
                : new Color32(255, 184, 68, 255);
            fillLight.intensity = isLoss ? 0.35f : 1.1f;
        }

        if (sceneCamera != null)
        {
            sceneCamera.backgroundColor = isLoss
                ? new Color32(5, 10, 22, 255)
                : new Color32(7, 24, 55, 255);
        }
    }

    private void ApplySafeArea(bool force)
    {
        if (safeAreaRoot == null || Screen.width <= 0 || Screen.height <= 0)
            return;

        Rect safeArea = Screen.safeArea;
        Vector2Int screenSize = new Vector2Int(Screen.width, Screen.height);
        if (!force && safeArea == lastSafeArea && screenSize == lastScreenSize)
            return;

        lastSafeArea = safeArea;
        lastScreenSize = screenSize;

        safeAreaRoot.anchorMin = new Vector2(
            safeArea.xMin / Screen.width,
            safeArea.yMin / Screen.height);
        safeAreaRoot.anchorMax = new Vector2(
            safeArea.xMax / Screen.width,
            safeArea.yMax / Screen.height);
        safeAreaRoot.offsetMin = Vector2.zero;
        safeAreaRoot.offsetMax = Vector2.zero;
    }

    private static void SetActive(GameObject[] objects, bool active)
    {
        if (objects == null)
            return;

        foreach (GameObject target in objects)
        {
            if (target != null)
                target.SetActive(active);
        }
    }
}
