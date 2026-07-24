using UnityEngine;
using System.Collections;

public class OpponentGoalMissTrigger : MonoBehaviour
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
			Golie.GetComponent<OpponentGolie>().enabled = false;
			Golie.GetComponent<OpponentGolieKick>().enabled = true;
				
			other.gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
			other.gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;
		}
	}
}
