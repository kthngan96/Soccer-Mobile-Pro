using UnityEngine;
using System.Collections;

public class ScreenSizeManager : MonoBehaviour
{

    // Use this for initialization
    void Start()
    {
        Rect insets = LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset;
        insets.width *= Screen.width / 480f;
        insets.height *= Screen.width / 480f;

        LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset = insets;
    }

    // Update is called once per frame
    void Update()
    {

    }
}
