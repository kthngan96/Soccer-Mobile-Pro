using UnityEngine;
using System.Collections;

public class PlayerGoalMissTrigger : MonoBehaviour
{
	private BallScript ballScript;
	public GameObject Golie;
	// Use this for initialization
	void Start()
	{
		GameObject FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript>();
	}
	
	void OnTriggerEnter(Collider other)
	{
		if(other.tag == "TheSoccerBall")
		{
			Golie.GetComponent<PlayerGolie>().enabled = false;
			Golie.GetComponent<PlayerGolieKick>().enabled = true;
			
			other.gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
			other.gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;
		}
	}
}
