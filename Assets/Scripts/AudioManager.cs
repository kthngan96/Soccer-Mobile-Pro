using UnityEngine;
using UnityEngine.SceneManagement;
using System.Collections;

public class AudioManager : MonoBehaviour
{
	public AudioSource backgroundMusic;
	public AudioSource footballKicked;
	public AudioSource pauseWhistle;
	public AudioSource resumeWhistle;
	public AudioSource restartWhistle;
	public AudioSource onGoal;
	public AudioSource onMiss;

	public AudioSource onMatchStart;
	public bool loadMainMenuOnStart = false;

	static AudioManager au;

	public static bool isMusicOn = false;
	public static bool isSFXOn = false;

	void Start()
	{
		if(PlayerPrefs.GetInt("jtsounds")==1)
		{
			if(PlayerPrefs.GetInt("isMusicOn")==1)
				isMusicOn = true;
			else
				isMusicOn = false;

			if(PlayerPrefs.GetInt("isSFXOn")==1)
				isSFXOn = true;
			else
				isSFXOn = false;
		}
		else
		{
			PlayerPrefs.SetInt("jtsounds",1);
			PlayerPrefs.SetInt("isMusicOn",1);
			PlayerPrefs.SetInt("isSFXOn",1);
			PlayerPrefs.Save();

			isMusicOn = true;
			isSFXOn = true;
		}

		if(au == null)
		{
			DontDestroyOnLoad(this.gameObject);
			au = gameObject.GetComponent<AudioManager>();
		}
		else
			Destroy(gameObject);

		if (loadMainMenuOnStart)
			Invoke ("LoadMainMenu",0.5f);
	}

	void LoadMainMenu()
	{
		SceneManager.LoadScene("MainMenu");
	}

	public static void Save()
	{
		if(isMusicOn)
			PlayerPrefs.SetInt("isMusicOn",1);
		else
			PlayerPrefs.SetInt("isMusicOn",0);

		if(isSFXOn)
			PlayerPrefs.SetInt("isSFXOn",1);
		else
			PlayerPrefs.SetInt("isSFXOn",0);

		PlayerPrefs.Save();
	}

	public static void PlayKickSound()
	{
		if (au != null && isSFXOn)
			au.footballKicked.Play();
	}

	public static void PlayResumeWhistle()
	{
		if (au != null && isSFXOn)
			au.resumeWhistle.Play();
	}

	public static void PlayRestartWhistle()
	{
		if (au != null && isSFXOn)
			au.restartWhistle.Play();
	}

	public static void PlayPauseWhistle()
	{
		if (au != null && isSFXOn)
			au.pauseWhistle.Play();
	}

	public static void PlayOnGoalRoar()
	{
		if (au != null && isSFXOn)
			au.onGoal.Play();
	}

	public static void PlayAudienceSound()
	{
		if (au != null && isSFXOn)
			au.onMatchStart.Play();
	}

	public static void StopAudienceSound()
	{
		if (au != null)
			au.onMatchStart.Stop();
	}

	public static void PlayOnMissRoar()
	{
		if (au != null && isSFXOn)
			au.onMiss.Play();
	}

	public static void PlayBackgroundMusic()
	{
		if (au != null && isMusicOn)
		if(!au.backgroundMusic.isPlaying)
			au.backgroundMusic.Play();
	}

	public static void StopBackgroundMusic()
	{
		if (au != null)
			au.backgroundMusic.Stop();
	}
}
