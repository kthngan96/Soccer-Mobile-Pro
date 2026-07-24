using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;

public class ButtonAction : MonoBehaviour
{
	OtherDialoguesActive oda;
	private GUITexture legacyTexture;
	private ButtonController buttonController;
	private bool pressedInside;

	void Start ()
	{
		GameObject mainCamera = GameObject.Find ("Main Camera");
		oda = mainCamera == null ? null : mainCamera.GetComponent<OtherDialoguesActive> ();
		legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		buttonController = gameObject.GetComponent<ButtonController>();
  
    }

//	public GameObject matchCompleted, halfCompleted;
	public enum Buttons{
		Play,
		MoreGames,
		RateUs,
		QuickMatch,
		InternationalCup,
		Back,
		Next,
		PrevTeam,
		NextTeam,
		KickOff,
		MainMenu,
		PlaySecondHalf,
		Replay,
		YES_QUIT,
		NO_QUIT,
		Resume,
		Pause,
		None
	};

	public Buttons buttonType = Buttons.None;
	void Update()
	{
		if(SoccerInput.PausePressedThisFrame)
		{
			backPressed();
		}

		UpdateLegacyPointerInput();
	}
	void OnMouseDown()
	{
		SetPressedTexture();
	}

	void OnMouseUpAsButton()
	{
		SetNormalTexture();

		switch(buttonType)
		{
		case Buttons.Pause:
			if(oda == null || !oda.isOtherDialogueActive)
			{
				PauseController.isPaused=true;
			}
               

                break;
		case Buttons.Play:
			SceneManager.LoadScene("GameSelectionScene");
			break;

		case Buttons.MoreGames:
			Application.OpenURL("https://play.google.com/store/apps/developer?id=XYZ");
			break;

		case Buttons.RateUs:
			Application.OpenURL("https://play.google.com/store/apps/details?id=com.yourgame.url");
			break;

		case Buttons.QuickMatch:
			GameManager.SharedObject().isQuickMatch = true;
			GameManager.SharedObject().IsFirstHalf = true;
			PlayerPosition.PlayerTurn = true;
			SceneManager.LoadScene("1stTeamSelection");
			break;

		case Buttons.InternationalCup:
			GameManager.SharedObject().isQuickMatch = false;
			GameManager.SharedObject().IsFirstHalf = true;
			PlayerPosition.PlayerTurn = true;

			if(MatchesSceneController.HasPendingCup())
				SceneManager.LoadScene("MatchesScene");
			else
				SceneManager.LoadScene("1stTeamSelection");
			break;

		case Buttons.Back:
			backPressed();
			break;

		case Buttons.Next:
			if(SceneManager.GetActiveScene().name == "1stTeamSelection" && GameManager.SharedObject().isQuickMatch)
				SceneManager.LoadScene("2ndTeamSelection");
			else if(SceneManager.GetActiveScene().name == "1stTeamSelection" && !GameManager.SharedObject().isQuickMatch)
				SceneManager.LoadScene("GroupsScene");
			else if(SceneManager.GetActiveScene().name == "GroupsScene" && !GameManager.SharedObject().isQuickMatch)
				SceneManager.LoadScene("MatchesScene");
			else if(SceneManager.GetActiveScene().name == "MatchesScene" && !GameManager.SharedObject().isQuickMatch)
				SceneManager.LoadScene("KickOffScene");
			else if(SceneManager.GetActiveScene().name == "2ndTeamSelection")
				SceneManager.LoadScene("KickOffScene");
			break;

		case Buttons.PrevTeam:
			if(SceneManager.GetActiveScene().name == "1stTeamSelection")
				TeamSelectionController.teamIndex -= 1;
			else if(SceneManager.GetActiveScene().name == "2ndTeamSelection")
				TeamSelectionController2.teamIndex -= 1;
			break;

		case Buttons.NextTeam:
			if(SceneManager.GetActiveScene().name == "1stTeamSelection")
				TeamSelectionController.teamIndex += 1;
			else if(SceneManager.GetActiveScene().name == "2ndTeamSelection")
				TeamSelectionController2.teamIndex += 1;
			break;

		case Buttons.KickOff:
			PlayerPrefs.Save();
			SceneManager.LoadScene("MatchScene");
			break;

		case Buttons.MainMenu:
			InitGame.matchcomplete=false;
			InitGame.halfComplete=false;
			if(AudioManager.isSFXOn)
				AudioListener.volume=1;

			PauseController.isPaused = false;
			Time.timeScale = 1f;
			PlayerPosition.PlayerTurn = !PlayerPosition.PlayerTurn;
			if(SceneManager.GetActiveScene().name == "FinalCeleberation")
			{
				SceneManager.LoadScene("MainMenu");
			}
			else if(GameManager.SharedObject().isQuickMatch == false && PlayerPrefs.GetInt("matchNumber")>7)
			{
				PlayerPrefs.SetInt("HasPendingCup",0);
				SceneManager.LoadScene("FinalCeleberation");
			}
			else if(GameManager.SharedObject().isQuickMatch == false && PlayerPrefs.GetInt("matchNumber")<7)
			{
				SceneManager.LoadScene("MatchesScene");
			}
			else
			{
				SceneManager.LoadScene("MainMenu");
			}

			//if(GameManager.SharedObject().isQuickMatch)	SceneManager.LoadScene("MainMenu");
			//else	SceneManager.LoadScene("MatchesScene");

			break;

		case Buttons.PlaySecondHalf:
			InitGame.halfComplete=false;
			PlayerPosition.PlayerTurn = false;
			GameManager.SharedObject().GameTime = 0;
			GameManager.SharedObject().IsFirstHalf = false;
			if(GameManager.SharedObject().isQuickMatch)
				SceneManager.LoadScene("MatchScene");
			else
				SceneManager.LoadScene("KickOffScene");
			break;

		case Buttons.Replay:
			GameManager.SharedObject().IsFirstHalf = true;
			GameManager.SharedObject().IsGameReady = true;
			GameManager.SharedObject().ShowHalfTimeDialog = false;
			GameManager.SharedObject().ShowMatchEndDialog = false;
			GameManager.SharedObject().playerTeamGoals = 0;
			GameManager.SharedObject().opponentTeamGoals = 0;
			SceneManager.LoadScene("MatchScene");
			if(AudioManager.isSFXOn)
				AudioListener.volume=1;
			break;

		case Buttons.YES_QUIT:
			Application.Quit();
			break;

		case Buttons.NO_QUIT:
			if(AudioManager.isMusicOn)
				AudioListener.volume=1;
			GameObject.Find("QuitDialog").SetActive(false);
			break;

		case Buttons.Resume:
			PauseController.isPaused = false;
			break;
		}
	}

	void backPressed()
	{
		if(SceneManager.GetActiveScene().name == "GameSelectionScene")
		{
			SceneManager.LoadScene("MainMenu");
		}
		else if(SceneManager.GetActiveScene().name == "1stTeamSelection")
			SceneManager.LoadScene("GameSelectionScene");
		else if(SceneManager.GetActiveScene().name == "2ndTeamSelection")
			SceneManager.LoadScene("1stTeamSelection");
		else if(SceneManager.GetActiveScene().name == "MatchesScene" && MatchesSceneController.HasPendingCup())
			SceneManager.LoadScene("GameSelectionScene");
		
		else if(SceneManager.GetActiveScene().name == "KickOffScene" && !GameManager.SharedObject().isQuickMatch && MatchesSceneController.HasPendingCup())
			SceneManager.LoadScene("MatchesScene");
		else if(SceneManager.GetActiveScene().name == "KickOffScene" && !GameManager.SharedObject().isQuickMatch && !MatchesSceneController.HasPendingCup())
			SceneManager.LoadScene("1stTeamSelection");
		
		else if(SceneManager.GetActiveScene().name == "KickOffScene" && GameManager.SharedObject().isQuickMatch)
			SceneManager.LoadScene("2ndTeamSelection");
		else if(SceneManager.GetActiveScene().name == "GroupsScene" && !GameManager.SharedObject().isQuickMatch)
			SceneManager.LoadScene("1stTeamSelection");
	}

	private void UpdateLegacyPointerInput()
	{
		if (legacyTexture == null)
		{
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		}

		if (legacyTexture == null)
		{
			return;
		}

		if (SoccerInput.PointerPressedThisFrame)
		{
			pressedInside = legacyTexture.HitTest(SoccerInput.PointerPosition);
			if (pressedInside)
			{
				SetPressedTexture();
			}
		}

		if (SoccerInput.PointerReleasedThisFrame)
		{
			bool releaseInside = legacyTexture.HitTest(SoccerInput.PointerPosition);
			if (pressedInside && releaseInside)
			{
				OnMouseUpAsButton();
			}
			else if (pressedInside)
			{
				SetNormalTexture();
			}

			pressedInside = false;
		}

		for (int i = 0; i < SoccerInput.TouchCount; i++)
		{
			SoccerTouch touch = SoccerInput.GetTouch(i);
			Vector3 touchPosition = new Vector3(touch.position.x, touch.position.y, 0f);
			if (touch.phase == SoccerTouchPhase.Began && legacyTexture.HitTest(touchPosition))
			{
				pressedInside = true;
				SetPressedTexture();
			}
			else if ((touch.phase == SoccerTouchPhase.Ended || touch.phase == SoccerTouchPhase.Canceled) && pressedInside)
			{
				if (touch.phase == SoccerTouchPhase.Ended && legacyTexture.HitTest(touchPosition))
				{
					OnMouseUpAsButton();
				}
				else
				{
					SetNormalTexture();
				}

				pressedInside = false;
			}
		}
	}

	private void SetPressedTexture()
	{
		if (buttonController == null)
		{
			buttonController = gameObject.GetComponent<ButtonController>();
		}

		if (legacyTexture == null)
		{
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		}

		if (buttonController != null && buttonController.hoverTexture != null && legacyTexture != null)
		{
			legacyTexture.texture = buttonController.hoverTexture;
		}
	}

	private void SetNormalTexture()
	{
		if (buttonController == null)
		{
			buttonController = gameObject.GetComponent<ButtonController>();
		}

		if (legacyTexture == null)
		{
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		}

		if (buttonController != null && legacyTexture != null)
		{
			legacyTexture.texture = buttonController.normalTexture;
		}
	}
}
