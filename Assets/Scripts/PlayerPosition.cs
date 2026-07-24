using UnityEngine;
using System.Collections;

public class PlayerPosition : MonoBehaviour
{
	public GUIStyle passButtonStyle;

	public static bool PlayerTurn = true;

	public Transform InitialPositonTransform, SecondaryPositonTransform;
	private Vector3 InitialPosition, SecondaryPosition;

	private Player playerScript;

	public Transform passingPlayer;
	GameObject ball;

	void Start ()
	{
		ball = GameObject.FindGameObjectWithTag("TheSoccerBall");
		playerScript = InitialPositonTransform.GetComponent<Player> ();
		InitialPosition = InitialPositonTransform.position;
		SecondaryPosition = SecondaryPositonTransform.position;
	}
	
	void Update ()
	{
		if(PlayerTurn)
			playerScript.initialPosition = InitialPosition;
		else 
			playerScript.initialPosition = SecondaryPosition;
	}

	void OnGUI()
	{
		if(!PauseController.isPaused)
		{


		if(PlayerTurn && GameManager.SharedObject().IsGameReady == false && Vector3.Distance(transform.position,ball.transform.position)<1.5f)
		{
			if(GUI.Button(new Rect (Screen.width - GetValue(150), Screen.height - GetValue(150) - GetValue(130), GetValue(110), GetValue(110)),"",passButtonStyle))
			{
				Vector3 direction = (passingPlayer.position-ball.transform.position).normalized;

				ball.GetComponent<Rigidbody>().AddForce(direction*1200, ForceMode.Impulse);

				AudioManager.PlayResumeWhistle();
				GameManager.SharedObject().IsGameReady = true;
			}
		}
		}
	}

	float GetValue(float value)
	{
		return value * Screen.height/640f;
	}
}
