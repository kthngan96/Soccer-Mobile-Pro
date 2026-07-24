using UnityEngine;

[ExecuteAlways]
public class FinalMessageScript : MonoBehaviour {
	private const string PreviewMessage = "dfsdfsdf";

	void Awake ()
	{
		EnsurePreviewText();
	}

	void OnEnable ()
	{
		EnsurePreviewText();
	}

#if UNITY_EDITOR
	void OnValidate ()
	{
		EnsurePreviewText();
	}
#endif

	void Start ()
	{
		if(!Application.isPlaying)
		{
			EnsurePreviewText();
			return;
		}

		PlayerPrefs.SetInt ("HasPendingCup",0);
		PlayerPrefs.Save ();

		GUIText guiText = LegacyGuiUtility.GetOrAddGUIText(gameObject);
		string message = PlayerPrefs.GetString ("message");
		if(string.IsNullOrEmpty(message))
		{
			message = string.IsNullOrEmpty(guiText.text) ? PreviewMessage : guiText.text;
		}

		guiText.text = message;
		if(guiText.text.StartsWith("Sorry"))
			guiText.color = Color.red;
		else
			guiText.color = Color.green;
	}

	private void EnsurePreviewText()
	{
		GUIText guiText = LegacyGuiUtility.GetOrAddGUIText(gameObject);
		if(guiText == null)
		{
			return;
		}

		if(string.IsNullOrEmpty(guiText.text))
		{
			guiText.text = PreviewMessage;
		}

		guiText.anchor = 4;
		guiText.alignment = 1;
		guiText.fontSize = 50;
		guiText.richText = true;
		if(!Application.isPlaying)
		{
			guiText.color = Color.red;
		}
	}
}
