using UnityEngine;
using System.Collections;

public class TeamSelectionController2:MonoBehaviour
{
	public static int teamIndex = 0;
	public Texture[] teams;
	public Texture[] textures;
	public Texture[] HDTextures;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void FixedUpdate()
	{
		if(teamIndex > 31) teamIndex = 0;
		if(teamIndex < 0) teamIndex = 31;

		if(LegacyGuiUtility.GetOrAddGUITexture(gameObject))
			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture = teams[teamIndex];

		GameManager.SharedObject ().opponentTeamFlag = teams[teamIndex];
		GameManager.SharedObject ().opponentTeamTexture = textures[teamIndex];

		GameManager.SharedObject ().opponentTeamHDTexture = HDTextures[teamIndex];
	}
}