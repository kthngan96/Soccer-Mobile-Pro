using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;

public class TeamNameController : MonoBehaviour
{
	public string[] TeamNames;
	private static readonly string[] DefaultTeamNames =
	{
		"Algeria", "Angola", "Argentina", "Austria", "Bahrain", "Brazil", "Cameroon", "China",
		"Colombia", "Denmark", "Finland", "Gabon", "Germany", "Greece", "Honduras", "Iran",
		"Israel", "Italy", "Ivory Coast", "Korea", "Latvia", "Mexico", "Morocco", "Peru",
		"Potugal", "Scotland", "Slovakia", "Slovenia", "Togo", "Ukraine", "Uruguay", "USA"
	};

	// Use this for initialization
	void Start () {
		ConfigureText(LegacyGuiUtility.GetOrAddGUIText(gameObject));
	}
	
	// Update is called once per frame
	void Update () 
	{
		int index = 0;
		string[] names = GetTeamNames();

		if(SceneManager.GetActiveScene().name == "1stTeamSelection")
		{
			index = NormalizeIndex(TeamSelectionController.teamIndex, names.Length);

			GameManager.SharedObject().playerTeamName = names[index];
			GameManager.SharedObject().playerTeamShortName = GameManager.SharedObject().playerTeamName.Substring(0,3).ToUpper();
		}
		else if(SceneManager.GetActiveScene().name == "2ndTeamSelection" || SceneManager.GetActiveScene().name == "MatchesScene")
		{
			index = NormalizeIndex(TeamSelectionController2.teamIndex, names.Length);
			
			GameManager.SharedObject().opponentTeamName = names[index];
			GameManager.SharedObject().opponentTeamShortName = GameManager.SharedObject().opponentTeamName.Substring(0,3).ToUpper();

			//
			int index2 = NormalizeIndex(TeamSelectionController.teamIndex, names.Length);
			
			GameManager.SharedObject().playerTeamName = names[index2];
			GameManager.SharedObject().playerTeamShortName = GameManager.SharedObject().playerTeamName.Substring(0,3).ToUpper();
		}

		GUIText guiText = LegacyGuiUtility.GetOrAddGUIText(gameObject);
		if(guiText)
		{
			ConfigureText(guiText);
			guiText.text = names[index];
		}
	}

	private string[] GetTeamNames()
	{
		if(TeamNames != null && TeamNames.Length > 0)
			return TeamNames;

		return DefaultTeamNames;
	}

	private static int NormalizeIndex(int index, int count)
	{
		if(count <= 0)
			return 0;

		index %= count;
		if(index < 0)
			index += count;

		return index;
	}

	private static void ConfigureText(GUIText guiText)
	{
		if(!guiText)
			return;

		guiText.anchor = 4;
		guiText.alignment = 1;
		guiText.fontSize = Mathf.Max(28, Screen.height / 18);
		guiText.fontStyle = FontStyle.Bold;
		guiText.color = new Color(0.02f, 0.02f, 0.02f, 1f);
		guiText.richText = false;
	}
}
