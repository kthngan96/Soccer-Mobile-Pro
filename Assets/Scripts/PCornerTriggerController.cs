using UnityEngine;
using System.Collections;

public class PCornerTriggerController : MonoBehaviour
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
			if(ballScript.lastOwnerTag == "Player")
				GameManager.SharedObject().OpponentGotCornerKick = true;
			else
			{
				Golie.GetComponent<PlayerGolie>().enabled = false;
				Golie.GetComponent<PlayerGolieKick>().enabled = true;

				GameManager.SharedObject().OpponentMissedGoal = true;
			}
			ballScript.ownerPlayer = null;

			if(other.gameObject.transform.position.z < 0)
				GameManager.SharedObject().foulPosition = new Vector3(-55f, 0f, -37.3f);
			else
				GameManager.SharedObject().foulPosition = new Vector3(-55f, 0f, 37.3f);

			other.gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
			other.gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;

			other.gameObject.transform.position = GameManager.SharedObject().foulPosition;
		}
	}
}
