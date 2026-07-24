using UnityEngine;
using System.Collections;

public class MatchesSceneController : MonoBehaviour
{
	public Texture[] Flags;

	public Transform flag1,flag2,flag3,flag4,flag5,flag6,flag7;
	private int match1TeamIndex=-1,match2TeamIndex=-1,match3TeamIndex=-1,match4TeamIndex=-1,match5TeamIndex=-1,match6TeamIndex=-1,match7TeamIndex=-1;
	private string teamGroup = "";

	private int match1score1,match2score1,match3score1,match4score1,match5score1,match6score1,match7score1;
	private int match1score2,match2score2,match3score2,match4score2,match5score2,match6score2,match7score2;
	private int matchNumber = 1;

	public static bool HasPendingCup()
	{
		return (PlayerPrefs.GetInt ("HasPendingCup")==1?true:false);
	}


	// Use this for initialization
	void Start ()
	{
		if(HasPendingCup())
		{
			match1TeamIndex = PlayerPrefs.GetInt("match1TeamIndex");
			match2TeamIndex = PlayerPrefs.GetInt("match2TeamIndex");
			match3TeamIndex = PlayerPrefs.GetInt("match3TeamIndex");
			match4TeamIndex = PlayerPrefs.GetInt("match4TeamIndex");
			match5TeamIndex = PlayerPrefs.GetInt("match5TeamIndex");
			match6TeamIndex = PlayerPrefs.GetInt("match6TeamIndex");
			match7TeamIndex = PlayerPrefs.GetInt("match7TeamIndex");
		}
		else
		{
			int SelectedGroupStartIndex = 0;
			int SelectedGroupEndIndex = 3;
			
			if(TeamSelectionController.teamIndex>=0 && TeamSelectionController.teamIndex<=3)
			{
				teamGroup = "A";
				SelectedGroupStartIndex = 0;
				SelectedGroupEndIndex = 3;
			}
			else if(TeamSelectionController.teamIndex>=4 && TeamSelectionController.teamIndex<=7)
			{
				teamGroup = "B";
				SelectedGroupStartIndex = 4;
				SelectedGroupEndIndex = 7;
			}
			else if(TeamSelectionController.teamIndex>=8 && TeamSelectionController.teamIndex<=11)
			{
				teamGroup = "C";
				SelectedGroupStartIndex = 8;
				SelectedGroupEndIndex = 11;
			}
			else if(TeamSelectionController.teamIndex>=12 && TeamSelectionController.teamIndex<=15)
			{
				teamGroup = "D";
				SelectedGroupStartIndex = 12;
				SelectedGroupEndIndex = 15;
			}
			else if(TeamSelectionController.teamIndex>=16 && TeamSelectionController.teamIndex<=19)
			{
				teamGroup = "E";
				SelectedGroupStartIndex = 16;
				SelectedGroupEndIndex = 19;
			}
			else if(TeamSelectionController.teamIndex>=20 && TeamSelectionController.teamIndex<=23)
			{
				teamGroup = "F";
				SelectedGroupStartIndex = 20;
				SelectedGroupEndIndex = 23;
			}
			else if(TeamSelectionController.teamIndex>=24 && TeamSelectionController.teamIndex<=27)
			{
				teamGroup = "G";
				SelectedGroupStartIndex = 24;
				SelectedGroupEndIndex = 27;
			}
			else if(TeamSelectionController.teamIndex>=28 && TeamSelectionController.teamIndex<=31)
			{
				teamGroup = "H";
				SelectedGroupStartIndex = 28;
				SelectedGroupEndIndex = 31;
			}
			
			// GROUP MATCHES
			int tIndex = 0;
			for(int i = SelectedGroupStartIndex; i <= SelectedGroupEndIndex; i++)
			{
				if(i != TeamSelectionController.teamIndex)
				{
					if(match1TeamIndex == -1)
						match1TeamIndex = i;
					else if(match2TeamIndex == -1)
						match2TeamIndex = i;
					else if(match3TeamIndex == -1)
						match3TeamIndex = i;
					
					tIndex += 1;
				}
			}
			
			int NextGroupStartIndex = 0;
			int NextGroupEndIndex = 3;
			
			if(teamGroup == "A")
			{
				NextGroupStartIndex = 4;
				NextGroupStartIndex = 7;
			}
			else if(teamGroup == "B")
			{
				NextGroupStartIndex = 0;
				NextGroupStartIndex = 3;
			}
			else if(teamGroup == "C")
			{
				NextGroupStartIndex = 12;
				NextGroupStartIndex = 15;
			}
			else if(teamGroup == "D")
			{
				NextGroupStartIndex = 8;
				NextGroupStartIndex = 11;
			}
			else if(teamGroup == "E")
			{
				NextGroupStartIndex = 20;
				NextGroupStartIndex = 23;
			}
			else if(teamGroup == "F")
			{
				NextGroupStartIndex = 16;
				NextGroupStartIndex = 19;
			}
			else if(teamGroup == "G")
			{
				NextGroupStartIndex = 28;
				NextGroupStartIndex = 31;
			}
			else if(teamGroup == "H")
			{
				NextGroupStartIndex = 24;
				NextGroupStartIndex = 27;
			}
			
			match4TeamIndex = Random.Range(NextGroupStartIndex,NextGroupEndIndex+1);
			
			int QuarterFinalStartIndex = 0;
			int QuarterFinalEndIndex = 0;
			
			if(teamGroup == "C" || teamGroup == "B" || teamGroup == "C" || teamGroup == "D")
			{
				QuarterFinalStartIndex = 0;
				QuarterFinalEndIndex = 15;
			}
			else
			{
				QuarterFinalStartIndex = 16;
				QuarterFinalEndIndex = 31;
			}
			//Quarter Final
			while(match5TeamIndex == -1)
			{
				int index = Random.Range(QuarterFinalStartIndex,QuarterFinalEndIndex+1);
				if(index != TeamSelectionController.teamIndex && index != match1TeamIndex && index != match2TeamIndex && index != match3TeamIndex && index != match4TeamIndex)
					match5TeamIndex = index;
			}
			//Semi Final
			while(match6TeamIndex == -1)
			{
				int index = Random.Range(0,32);
				if(index != TeamSelectionController.teamIndex && index != match1TeamIndex && index != match2TeamIndex && index != match3TeamIndex && index != match4TeamIndex && index != match5TeamIndex)
					match6TeamIndex = index;
			}
			//Final
			while(match7TeamIndex == -1)
			{
				int index = Random.Range(0,32);
				if(index != TeamSelectionController.teamIndex && index != match1TeamIndex && index != match2TeamIndex && index != match3TeamIndex && index != match4TeamIndex && index != match5TeamIndex && index != match6TeamIndex)
					match7TeamIndex = index;
			}
			
			PlayerPrefs.SetInt("match1TeamIndex",match1TeamIndex);
			PlayerPrefs.SetInt("match2TeamIndex",match2TeamIndex);
			PlayerPrefs.SetInt("match3TeamIndex",match3TeamIndex);
			PlayerPrefs.SetInt("match4TeamIndex",match4TeamIndex);
			PlayerPrefs.SetInt("match5TeamIndex",match5TeamIndex);
			PlayerPrefs.SetInt("match6TeamIndex",match6TeamIndex);
			PlayerPrefs.SetInt("match7TeamIndex",match7TeamIndex);
			PlayerPrefs.SetInt("playerTeamIndex",TeamSelectionController.teamIndex);

			PlayerPrefs.SetInt("HasPendingCup",1);

			PlayerPrefs.SetInt("match1score1",-1); PlayerPrefs.SetInt("match1score2",-1);
			PlayerPrefs.SetInt("match2score1",-1); PlayerPrefs.SetInt("match2score2",-1);
			PlayerPrefs.SetInt("match3score1",-1); PlayerPrefs.SetInt("match3score2",-1);
			PlayerPrefs.SetInt("match4score1",-1); PlayerPrefs.SetInt("match4score2",-1);
			PlayerPrefs.SetInt("match5score1",-1); PlayerPrefs.SetInt("match5score2",-1);
			PlayerPrefs.SetInt("match6score1",-1); PlayerPrefs.SetInt("match6score2",-1);
			PlayerPrefs.SetInt("match7score1",-1); PlayerPrefs.SetInt("match7score2",-1);
			PlayerPrefs.SetInt("matchNumber",1);
		}

		match1score1 = PlayerPrefs.GetInt("match1score1");
		match2score1 = PlayerPrefs.GetInt("match2score1");
		match3score1 = PlayerPrefs.GetInt("match3score1");
		match4score1 = PlayerPrefs.GetInt("match4score1");
		match5score1 = PlayerPrefs.GetInt("match5score1");
		match6score1 = PlayerPrefs.GetInt("match6score1");
		match7score1 = PlayerPrefs.GetInt("match7score1");

		match1score2 = PlayerPrefs.GetInt("match1score2");
		match2score2 = PlayerPrefs.GetInt("match2score2");
		match3score2 = PlayerPrefs.GetInt("match3score2");
		match4score2 = PlayerPrefs.GetInt("match4score2");
		match5score2 = PlayerPrefs.GetInt("match5score2");
		match6score2 = PlayerPrefs.GetInt("match6score2");
		match7score2 = PlayerPrefs.GetInt("match7score2");

		matchNumber = PlayerPrefs.GetInt("matchNumber");

		LegacyGuiUtility.GetOrAddGUITexture(flag1).texture = Flags[match1TeamIndex];
		LegacyGuiUtility.GetOrAddGUITexture(flag2).texture = Flags[match2TeamIndex];
		LegacyGuiUtility.GetOrAddGUITexture(flag3).texture = Flags[match3TeamIndex];

		if(matchNumber > 3)
			LegacyGuiUtility.GetOrAddGUITexture(flag4).texture = Flags[match4TeamIndex];

		if(matchNumber > 4)
			LegacyGuiUtility.GetOrAddGUITexture(flag5).texture = Flags[match5TeamIndex];

		if(matchNumber > 5)
			LegacyGuiUtility.GetOrAddGUITexture(flag6).texture = Flags[match6TeamIndex];
	
		if(matchNumber > 6)
			LegacyGuiUtility.GetOrAddGUITexture(flag7).texture = Flags[match7TeamIndex];

		GameManager.SharedObject ().CurrentMatch = CurrentMatchIndex();
		GameManager.SharedObject ().IsFirstHalf = true;
		TeamSelectionController2.teamIndex = GameManager.SharedObject ().CurrentMatch;
		TeamSelectionController.teamIndex = PlayerPrefs.GetInt("playerTeamIndex");
	}

	private int CurrentMatchIndex()
	{
		int cti = 1;
		switch(matchNumber)
		{
			case 1: cti = match1TeamIndex; break;
			case 2: cti = match2TeamIndex; break;
			case 3: cti = match3TeamIndex; break;
			case 4: cti = match4TeamIndex; break;
			case 5: cti = match5TeamIndex; break;
			case 6: cti = match6TeamIndex; break;
			case 7: cti = match7TeamIndex; break;
		}
		return cti;
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
