using UnityEngine;
using System.Collections;

public class PGTController : MonoBehaviour
{
	float lastTriggerTime = 0f;
	public GameObject Golie;
	public BallScript ballScript;

	public GameObject starterPlayer;

	void Start()
	{
		ballScript = GameObject.FindGameObjectWithTag("TheSoccerBall").GetComponent<BallScript>();
	}

//	void StartPlay()
//	{
//		GameManager.SharedObject().IsGameReady = true;
//	}

	void OnTriggerEnter(Collider other)
	{
		if(Time.time - lastTriggerTime > 5)
		{

			GoalCeleberationManager.PlayCeleberation(1);


			ballScript.isKicked = false;

			lastTriggerTime = Time.time;
			GameManager.SharedObject().opponentTeamGoals += 1;
			GameManager.SharedObject().IsGameReady = false;
			PlayerPosition.PlayerTurn = true;
//			Golie.GetComponent<PlayerGolie>().enabled = false;
//			Golie.GetComponent<PlayerGolieKick>().enabled = true;

			ballScript.PlaceOnInitialPositon();
			starterPlayer.GetComponent<PlayerPosition>().enabled = true;
			AudioManager.PlayOnGoalRoar();

//			Invoke("StartPlay",5f);
		}
	}
}