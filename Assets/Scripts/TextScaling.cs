using UnityEngine;
using System.Collections;

public class TextScaling : MonoBehaviour
{
	public int fontSize = 10;

	void Awake ()
	{
		ApplyFontSize();
	}

	void OnEnable ()
	{
		ApplyFontSize();
	}

	void Update ()
	{
		ApplyFontSize();
	}

	void FixedUpdate ()
	{
		ApplyFontSize();
	}

	private void ApplyFontSize()
	{
		GUIText guiText = LegacyGuiUtility.GetOrAddGUIText(gameObject);
		if(guiText)
		{
			guiText.fontSize = Mathf.Max(1, fontSize * Screen.height / 800);
		}
	}
}
