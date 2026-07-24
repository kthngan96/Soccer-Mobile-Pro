using UnityEngine;
using System.Collections;

public class OGTController : MonoBehaviour
{
	public GameObject Golie;
	public BallScript ballScript;

	float lastTriggerTime = 0f;
	
	void Start()
	{
		ballScript = GameObject.FindGameObjectWithTag("TheSoccerBall").GetComponent<BallScript>();
	}

	void StartPlay()
	{
		GameManager.SharedObject ().IsGameReady = true;
	}

	void OnTriggerEnter(Collider other)
	{
		Invoke ("Reset",1.5f);
		AudioManager.PlayOnGoalRoar();
	}
	void Reset()
	{
		if(Time.time - lastTriggerTime > 1)
		{
			GoalCeleberationManager.PlayCeleberation(0);
			ballScript.isKicked = false;
			
			lastTriggerTime = Time.time;
			GameManager.SharedObject().playerTeamGoals += 1;
			GameManager.SharedObject().IsGameReady = false;
			PlayerPosition.PlayerTurn = false;
			//Golie.GetComponent<OpponentGolie>().enabled = false;
			//Golie.GetComponent<OpponentGolieKick>().enabled = true;
			
			ballScript.PlaceOnInitialPositon();
//			AudioManager.PlayOnGoalRoar();
			
			Invoke("StartPlay",5f);
//			AudioManager.PlayOnGoalRoar();
		}
	}
}
