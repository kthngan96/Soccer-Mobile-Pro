using UnityEngine;
using System.Collections;

public class PauseController : MonoBehaviour
{

	public static bool isPaused = false;
	OtherDialoguesActive oda;
	// Use this for initialization
	void Start ()
	{
		Time.timeScale = 1f;
		isPaused = false;
		oda = GameObject.Find ("Main Camera").GetComponent<OtherDialoguesActive> ();
	}
	// Update is called once per frame
	void Update ()
	{
		if(!oda.isOtherDialogueActive)
		{
		if(SoccerInput.PausePressedThisFrame)
			{
			isPaused = !isPaused;

			if(isPaused)
			{
			}
			}
		}
		if (isPaused)
		{
			AudioListener.volume=0;
//			Time.timeScale = 0f;
			Invoke("timeScaleZero",0.8f);
			transform.position = new Vector3(0.5f,0.48f,30);
		}
		else
		{
			AudioListener.volume=1;
			Time.timeScale = 1f;
			transform.position = new Vector3(3,3,3);
		}
	
	}

	public static void pause()
	{
		isPaused = !isPaused;
	}
	void timeScaleZero()
	{
		Time.timeScale = 0f;
	}
//	void OnApplicationFocus(bool focusStatus) {
//		isPaused = focusStatus;
//	}
}
