using UnityEngine;
using System.Collections;

public class GameManager
{
	public bool ShowHalfTimeDialog = false;
	public bool ShowMatchEndDialog = false;

	public int GameTime = 0;

	private static GameManager sharedObject = null;

	public string playerTeamName = "PlayerTeam";
	public string playerTeamShortName = "PTM";

	public string opponentTeamName = "OpponentTeam";
	public string opponentTeamShortName = "OTM";

	public Texture playerTeamFlag;
	public Texture opponentTeamFlag;

	public Texture playerTeamTexture;
	public Texture opponentTeamTexture;

	public Texture playerTeamHDTexture;
	public Texture opponentTeamHDTexture;

	public int playerTeamGoals = 0;
	public int opponentTeamGoals = 0;

	public bool IsGameReady = true;
	public bool IsFirstHalf = true;

	public bool OpponentMadeFoul = false;
	public bool PlayerMadeFoul = false;

	public bool OpponentGotCornerKick = false;
	public bool PlayerGotCornerKick = false;

	public bool PlayerMissedGoal = false;
	public bool OpponentMissedGoal = false;

	public Vector3 foulPosition = Vector3.zero;

	public bool isQuickMatch = false;

	public int CurrentMatch = 0;

	public static GameManager SharedObject()
	{
		if(sharedObject == null)
			sharedObject = new GameManager();

		return sharedObject;
	}

	public GameManager()
	{
		playerTeamGoals = 0;
		opponentTeamGoals = 0;
		IsGameReady = true;
		IsFirstHalf = true;

		OpponentMadeFoul = false;
		PlayerMadeFoul = false;

		foulPosition = Vector3.zero;
	}
}
