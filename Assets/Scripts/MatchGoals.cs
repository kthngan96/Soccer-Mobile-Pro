using UnityEngine;
using System.Collections;

public class MatchGoals : MonoBehaviour
{
	public int matchnumber = 1;

	// Use this for initialization
	void Start ()
	{

	}
	
	// Update is called once per frame
	void Update ()
	{
		string score1Key, score2Key;
		score1Key = "match"+matchnumber+"score1";
		score2Key = "match"+matchnumber+"score2";
		
		int goal1 = PlayerPrefs.GetInt (score1Key);
		int goal2 = PlayerPrefs.GetInt (score2Key);
		
		if(goal1 < 0)
		{
			Destroy(gameObject);
			return;
		}
		
		string score = "";
		if(goal1 < 10) score = "0"+goal1;
		else score = "" + goal1;
		
		score += " - ";
		
		if(goal2 < 10) score += ("0"+goal2);
		else score += goal2;
		
		LegacyGuiUtility.GetOrAddGUIText(gameObject).text = score;
	}
}
