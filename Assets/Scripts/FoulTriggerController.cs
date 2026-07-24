using UnityEngine;
using System.Collections;

public class FoulTriggerController : MonoBehaviour
{
	private BallScript ballScript;

	// Use this for initialization
	void Start ()
	{
		GameObject FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript> ();
	}
	
	void OnTriggerEnter(Collider other)
	{
		if(other.tag == "TheSoccerBall" && GameManager.SharedObject().OpponentMadeFoul == false && GameManager.SharedObject().PlayerMadeFoul == false)
		{
			if(ballScript.lastOwnerTag == "Player")
			{
				GameManager.SharedObject().OpponentMadeFoul = false;
				GameManager.SharedObject().PlayerMadeFoul = true;
			}
			else
			{
				GameManager.SharedObject().OpponentMadeFoul = true;
				GameManager.SharedObject().PlayerMadeFoul = false;
			}
			ballScript.ownerPlayer = null;
			float z = 0f;
			if(other.gameObject.transform.position.z < 0)
				z = -37.5f;
			else
				z = 37.5f;

			GameManager.SharedObject().foulPosition = new Vector3(other.gameObject.transform.position.x,0,z);
		}
	}
}
