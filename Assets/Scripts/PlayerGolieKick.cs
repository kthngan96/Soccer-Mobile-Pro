using UnityEngine;
using System.Collections;

public class PlayerGolieKick : MonoBehaviour
{
	[HideInInspector]
	public bool kickTheBall = false;
	public bool ballKicked = false;

	private Vector3 ballPosition;

	private GameObject FootBall;
	public BallScript ballScript;

	// Use this for initialization
	void Start ()
	{
		ballPosition = new Vector3 (-48,0.15773f,0);

		FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript>();
	}
	
	// Update is called once per frame
	void Update ()
	{
		if(kickTheBall == false)
		{
			if(GetComponent<Animation>()["reposo"].enabled == false)
				GetComponent<Animation>().Play("reposo", PlayMode.StopAll);

			FootBall.GetComponent<Rigidbody>().velocity = Vector3.zero;
			FootBall.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
			FootBall.transform.position = ballPosition;

			transform.position = new Vector3(ballPosition.x-3, 0, ballPosition.z);
			transform.rotation = Quaternion.Euler(new Vector3(0,90,0));

			kickTheBall = true;
		}
		else
		{
			if(GetComponent<Animation>()["saque_esquina"].enabled == false)
				GetComponent<Animation>().Play("saque_esquina", PlayMode.StopAll);
			else if(GetComponent<Animation>()["saque_esquina"].enabled == true && GetComponent<Animation>()["saque_esquina"].normalizedTime < 0.5f)
				transform.Translate(Vector3.forward*Time.deltaTime*2.2f);
			else if(GetComponent<Animation>()["saque_esquina"].enabled == true && GetComponent<Animation>()["saque_esquina"].normalizedTime >= 0.7f)
			{
				kickTheBall = false;
				ballKicked = false;
				ballScript.ownerPlayer = null;
				gameObject.GetComponent<PlayerGolie>().enabled = true;
				gameObject.GetComponent<PlayerGolieKick>().enabled = false;
			}
			else if(GetComponent<Animation>()["saque_esquina"].enabled == true && GetComponent<Animation>()["saque_esquina"].normalizedTime >= 0.5f)
			{
				if(ballKicked == false)
				{
					AudioManager.PlayKickSound ();

					ballKicked = true;
					Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 30,transform.rotation.eulerAngles.y,transform.rotation.eulerAngles.z));
					FootBall.transform.rotation = shotAngle;
					FootBall.GetComponent<Rigidbody>().AddForce(FootBall.transform.forward*2000, ForceMode.Impulse);
					GameManager.SharedObject().IsGameReady = true;
				}
			}


		}
	}
}
