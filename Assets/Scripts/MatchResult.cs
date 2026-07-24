using UnityEngine;
using System.Collections;

public class MatchResult : MonoBehaviour
{
	public bool ourTeamScore=true; //if true then player team score will be displayed, otherwise opponent team score.
	public bool opponentTeamScore=true;  
	void FixedUpdate ()
	{
		string str = ""+GameManager.SharedObject().playerTeamGoals;
		if(ourTeamScore)
		{
		if(GameManager.SharedObject().playerTeamGoals < GameManager.SharedObject().opponentTeamGoals)
			LegacyGuiUtility.GetOrAddGUIText(gameObject).color = Color.red;

		
//		if(GameManager.SharedObject().playerTeamGoals < 10)	str = "0"+GameManager.SharedObject().playerTeamGoals;
		}
		else
		{

//			if(GameManager.SharedObject().playerTeamGoals < 10)
//			{
//				str = "0"+GameManager.SharedObject().opponentTeamGoals;
//			}
//			else
				str =""+ GameManager.SharedObject().opponentTeamGoals;

		}
		if(opponentTeamScore&&ourTeamScore)// if both checks are true then both team scores will be displayed in this single guiText
		{
		str += " - ";
//		str += "       ";

		if(GameManager.SharedObject().opponentTeamGoals < 10)	str += "0"+GameManager.SharedObject().opponentTeamGoals;
		else	str += GameManager.SharedObject().opponentTeamGoals;
		}
		LegacyGuiUtility.GetOrAddGUIText(gameObject).text = str;
		
	}
}