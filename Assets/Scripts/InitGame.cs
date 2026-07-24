using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;

public class InitGame : MonoBehaviour
{
	public AudioClip timeOverSound;
	public static bool halfComplete,matchcomplete;
	public Transform halfTimeDialog, matchEndDialog;
	float startTime = 0f;
	float matchTimeStep = 0f;
	public bool quickHalf=false;

	float lasTime = 0;

	void OnDestroy()
	{
		AudioManager.StopAudienceSound();
	}
	void Start()
	{
	}
	void Awake()
	{

		halfComplete=false;
		matchcomplete = false;
		Player.noControls = false;


		AudioManager.PlayAudienceSound();

		halfTimeDialog.gameObject.SetActive(false);
		matchEndDialog.gameObject.SetActive(false);

		////////////////********************\\\\\\\\\\\\\\\\\\\\\
		if(quickHalf)/////////*************** For Testing, if it's true, half finishes early...
			matchTimeStep = 1 / 400f;
		else
		matchTimeStep = 1 / 10f; 


		GameManager gm = GameManager.SharedObject();
		gm.GameTime = 0;
		gm.IsGameReady = false;
		gm.PlayerMadeFoul = false;
		gm.OpponentMadeFoul = false;

		if(gm.IsFirstHalf)
		{
			gm.playerTeamGoals = 0;
			gm.opponentTeamGoals = 0;

			transform.position = new Vector3(-45.39893f,4.642409f,14.2863f);
			transform.rotation = Quaternion.Euler(new Vector3(25.6506f,0f,0f));
		}
		else
		{
			transform.position = new Vector3(-45.39893f,4.642409f,14.2863f);
			transform.rotation = Quaternion.Euler(new Vector3(25.6506f,180f,0f));
		}

		startTime = Time.time;
		gm.GameTime = 0;
		lasTime = Time.time;

		AudioManager.StopBackgroundMusic();

	
	}
	
	void Update()
	{
		if(Time.time-lasTime >= matchTimeStep && GameManager.SharedObject().IsGameReady)
		{
			GameManager.SharedObject().GameTime += 1;
			lasTime = Time.time;
		}

		if(GameManager.SharedObject().GameTime > 45f*60f)
		{
			Player.noControls = true;

			GameManager.SharedObject().IsGameReady = false;
			AudioManager.PlayPauseWhistle();

			if(GameManager.SharedObject().IsFirstHalf)
			{
				PlayerPrefs.SetInt("lost",0);
				PlayerPrefs.Save();

				GameManager.SharedObject().ShowHalfTimeDialog = true;
				if(matchEndDialog != null)
				{
					matchEndDialog.gameObject.SetActive(false);
					Destroy(matchEndDialog.gameObject);


					matchcomplete = true;

					Destroy(GameObject.Find("GameGUI"));
					matchEndDialog = null;
				}

				if(halfTimeDialog != null && halfTimeDialog.gameObject != null)
				{
					//////////*****************\\\\\\\\\\\\\\
					if(!halfTimeDialog.gameObject.activeSelf)
					AudioSource.PlayClipAtPoint(timeOverSound, transform.position);
					halfTimeDialog.gameObject.SetActive(true);
				}
			}
			else
			{
				GameManager.SharedObject().ShowMatchEndDialog = true;

				if(matchEndDialog != null && matchEndDialog.gameObject != null)
				{
					//////////*****************\\\\\\\\\\\\\\
					if(!matchEndDialog.gameObject.activeSelf)
						AudioSource.PlayClipAtPoint(timeOverSound, transform.position);
					matchEndDialog.gameObject.SetActive(true);
					if(AudioManager.isSFXOn)
						AudioListener.volume=0;
				}

				if(halfTimeDialog != null)
				{
					halfTimeDialog.gameObject.SetActive(false);
					Destroy(halfTimeDialog.gameObject);

					halfComplete=true;


					Destroy(GameObject.Find("GameGUI"));
					halfTimeDialog = null;
				}

				if(GameManager.SharedObject().isQuickMatch == false)
				{
					int currentMatch = 1;
					if(PlayerPrefs.GetInt("match1TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 1;
					else if(PlayerPrefs.GetInt("match2TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 2;
					else if(PlayerPrefs.GetInt("match3TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 3;
					else if(PlayerPrefs.GetInt("match4TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 4;
					else if(PlayerPrefs.GetInt("match5TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 5;
					else if(PlayerPrefs.GetInt("match6TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 6;
					else if(PlayerPrefs.GetInt("match7TeamIndex") == GameManager.SharedObject().CurrentMatch)
						currentMatch = 7;

					string score1Key, score2Key;
					score1Key = "match"+currentMatch+"score1";
					score2Key = "match"+currentMatch+"score2";

					PlayerPrefs.SetInt(score1Key,GameManager.SharedObject().playerTeamGoals);
					PlayerPrefs.SetInt(score2Key,GameManager.SharedObject().opponentTeamGoals);

					if(GameManager.SharedObject().playerTeamGoals > GameManager.SharedObject().opponentTeamGoals || currentMatch <= 3)
					{
						currentMatch += 1;
						if(currentMatch > 7)
						{
							PlayerPrefs.SetString("message","Congratulation!\nYou won the International Cup.");
							PlayerPrefs.Save();
							SceneManager.LoadScene("FinalCeleberation");
						}
						PlayerPrefs.SetInt("matchNumber",currentMatch);
						PlayerPrefs.Save();
					}

					if(GameManager.SharedObject().playerTeamGoals <= GameManager.SharedObject().opponentTeamGoals && currentMatch > 3)
					{
						PlayerPrefs.SetString("message","Sorry!\nYou loose the International Cup.");
						PlayerPrefs.SetInt("lost",1);
						PlayerPrefs.Save();

						SceneManager.LoadScene("FinalCeleberation");
					}
				}
			}
		}

	}
	/*
	void FixedUpdate()
	{
		if(GameManager.SharedObject().ShowHalfTimeDialog)
		{
			//GameManager.SharedObject().IsGameReady = false;
			matchEndDialog.gameObject.SetActive(false);
			halfTimeDialog.gameObject.SetActive(true);
		}
		else if(GameManager.SharedObject().ShowMatchEndDialog)
		{
			//GameManager.SharedObject().IsGameReady = false;
			halfTimeDialog.gameObject.SetActive(false);
			matchEndDialog.gameObject.SetActive(true);
		}
	}*/
}
