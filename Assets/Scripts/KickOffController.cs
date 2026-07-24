using UnityEngine;
using System.Collections;

public class KickOffController : MonoBehaviour
{
	private const string DefaultPlayerTeamName = "Algeria";
	private const string DefaultPlayerTeamShortName = "ALG";
	private const string DefaultOpponentTeamName = "Angola";
	private const string DefaultOpponentTeamShortName = "ANG";

	public GUITexture Team1Flag, Team2Flag;
	public GUIText Team1Name, Team2Name;
	public Material Team1Material, Team2Material,Team1HDMaterial, Team2HDMaterial;

	// Use this for initialization
	void Start()
	{
		Team1Flag = ResolveTextureField(Team1Flag, "Team1Flag");
		Team2Flag = ResolveTextureField(Team2Flag, "Team2Flag");
		Team1Name = ResolveTextField(Team1Name, "Team1Name");
		Team2Name = ResolveTextField(Team2Name, "Team2Name");

		GameManager manager = GameManager.SharedObject();

		Texture defaultTeam1Flag = Team1Flag != null ? Team1Flag.texture : null;
		Texture defaultTeam2Flag = Team2Flag != null ? Team2Flag.texture : null;
		Texture defaultTeam1Texture = Team1Material != null ? Team1Material.mainTexture : null;
		Texture defaultTeam2Texture = Team2Material != null ? Team2Material.mainTexture : null;
		Texture defaultTeam1HDTexture = Team1HDMaterial != null ? Team1HDMaterial.mainTexture : null;
		Texture defaultTeam2HDTexture = Team2HDMaterial != null ? Team2HDMaterial.mainTexture : null;

		if(manager.playerTeamFlag == null)
			manager.playerTeamFlag = defaultTeam1Flag;
		if(manager.opponentTeamFlag == null)
			manager.opponentTeamFlag = defaultTeam2Flag;
		if(manager.playerTeamTexture == null)
			manager.playerTeamTexture = defaultTeam1Texture;
		if(manager.opponentTeamTexture == null)
			manager.opponentTeamTexture = defaultTeam2Texture;
		if(manager.playerTeamHDTexture == null)
			manager.playerTeamHDTexture = defaultTeam1HDTexture;
		if(manager.opponentTeamHDTexture == null)
			manager.opponentTeamHDTexture = defaultTeam2HDTexture;

		manager.playerTeamName = ResolveTeamName(manager.playerTeamName, "PlayerTeam", DefaultPlayerTeamName);
		manager.playerTeamShortName = ResolveTeamName(manager.playerTeamShortName, "PTM", DefaultPlayerTeamShortName);
		manager.opponentTeamName = ResolveTeamName(manager.opponentTeamName, "OpponentTeam", DefaultOpponentTeamName);
		manager.opponentTeamShortName = ResolveTeamName(manager.opponentTeamShortName, "OTM", DefaultOpponentTeamShortName);

		if(Team1Flag != null && manager.playerTeamFlag != null)
			Team1Flag.texture = manager.playerTeamFlag;
		if(Team2Flag != null && manager.opponentTeamFlag != null)
			Team2Flag.texture = manager.opponentTeamFlag;

		if(Team1Name != null)
			Team1Name.text = manager.playerTeamName;
		if(Team2Name != null)
			Team2Name.text = manager.opponentTeamName;

		if(Team1Material != null && manager.playerTeamTexture != null)
			Team1Material.mainTexture =  manager.playerTeamTexture;
		if(Team2Material != null && manager.opponentTeamTexture != null)
			Team2Material.mainTexture =  manager.opponentTeamTexture;

		if(Team1HDMaterial != null && manager.playerTeamHDTexture != null)
			Team1HDMaterial.mainTexture =  manager.playerTeamHDTexture;
		if(Team2HDMaterial != null && manager.opponentTeamHDTexture != null)
			Team2HDMaterial.mainTexture =  manager.opponentTeamHDTexture;
//		Team1HDMaterial.mainTexture =  GameManager.SharedObject ().opponentTeamHDTexture;


	}

	private static string ResolveTeamName(string candidate, string defaultValue, string fallback)
	{
		return string.IsNullOrEmpty(candidate) || candidate == defaultValue ? fallback : candidate;
	}

	private GUITexture ResolveTextureField(GUITexture current, string objectName)
	{
		if(current != null)
			return current;

		GameObject target = GameObject.Find(objectName);
		if(target == null)
			return null;

		return LegacyGuiUtility.GetOrAddGUITexture(target);
	}

	private GUIText ResolveTextField(GUIText current, string objectName)
	{
		if(current != null)
			return current;

		GameObject target = GameObject.Find(objectName);
		if(target == null)
			return null;

		return LegacyGuiUtility.GetOrAddGUIText(target);
	}
}
