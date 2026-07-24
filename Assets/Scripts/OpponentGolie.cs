using UnityEngine;
using System.Collections;

public class OpponentGolie : MonoBehaviour
{
	public Transform LeftHand;

	private GameObject FootBall;
	public BallScript ballScript;

	private bool playStandAnimation = false;
	private Vector3 InitialPosition;
	private Quaternion InitialRotation;

	float timeSinceCaught = 0f;

	// Use this for initialization
	void Start ()
	{
		timeSinceCaught = Time.time;

		InitialPosition = transform.position;
		InitialRotation = transform.rotation;

		FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript>();
	}
	
	// Update is called once per frame
	void Update ()
	{
		if(GameManager.SharedObject().PlayerGotCornerKick || GameManager.SharedObject().PlayerMadeFoul || GameManager.SharedObject().OpponentMadeFoul)
			return;

		if(ballScript.ownerPlayer == LeftHand && Time.time - timeSinceCaught > 2)
		{
			ballScript.isKicked = false;
			ballScript.ownerPlayer = null;
			gameObject.GetComponent<OpponentGolie>().enabled = false;
			gameObject.GetComponent<OpponentGolieKick>().enabled = true;
			return;
		}

		if(ballScript.isKicked && Vector3.Distance(transform.position,FootBall.transform.position) <= 5f  && ballScript.ownerPlayer != LeftHand)
		{
			//portero_despeje_lateral_izquierdo_raso down left
			//portero_despeje_lateral_izquierdo_alto up left
			//portero_despeje_lateral_derecho_raso down right
			//portero_despeje_lateral_derecho_alto up right
			//
			//
			if(transform.position.z - FootBall.transform.position.z < -0.2f && FootBall.transform.position.y < 0.5f)
			{
				if(GetComponent<Animation>()["portero_despeje_lateral_izquierdo_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_izquierdo_alto"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_alto"].enabled == false)
					GetComponent<Animation>().Play("portero_despeje_lateral_izquierdo_raso", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*-2f);
			}
			else if(transform.position.z - FootBall.transform.position.z < -0.2f && FootBall.transform.position.y >= 0.5f)
			{
				if(GetComponent<Animation>()["portero_despeje_lateral_izquierdo_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_izquierdo_alto"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_alto"].enabled == false)
					GetComponent<Animation>().Play("portero_despeje_lateral_izquierdo_alto", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*-2f);
			}
			else if(transform.position.z - FootBall.transform.position.z > 0.2f && FootBall.transform.position.y < 0.5f)
			{
				if(GetComponent<Animation>()["portero_despeje_lateral_izquierdo_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_izquierdo_alto"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_alto"].enabled == false)
					GetComponent<Animation>().Play("portero_despeje_lateral_derecho_raso", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*2f);
			}
			else if(transform.position.z - FootBall.transform.position.z > 0.2f && FootBall.transform.position.y >= 0.5f)
			{
				if(GetComponent<Animation>()["portero_despeje_lateral_izquierdo_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_izquierdo_alto"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_raso"].enabled == false && GetComponent<Animation>()["portero_despeje_lateral_derecho_alto"].enabled == false)
					GetComponent<Animation>().Play("portero_despeje_lateral_derecho_alto", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*2f);
			}

			if(Vector3.Distance(FootBall.transform.position, transform.position) <= 2.5f)
			{
				GameManager.SharedObject().IsGameReady = false;
				ballScript.SetOwner(LeftHand);
				timeSinceCaught = Time.time;
				playStandAnimation = true;
				ballScript.isKicked = false;
			}
		}
		else if((FootBall.transform.position.x > 43 || FootBall.transform.position.x > transform.position.x) && ballScript.ownerPlayer != LeftHand) // run towards ball
		{
			if(Vector3.Distance(transform.position,FootBall.transform.position) > 1f && GetComponent<Animation>()["portero_parada_frontal_rasa"].enabled == false)
			{
				if(GetComponent<Animation>()["corriendo"].enabled == false)
					GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);

				Quaternion rot = Quaternion.LookRotation((FootBall.transform.position - transform.position).normalized);
				rot.x = 0;
				transform.rotation = rot;
				transform.Translate(Vector3.forward*Time.deltaTime*5);
			}
			else
			{
				if(GetComponent<Animation>()["portero_parada_frontal_rasa"].enabled == false)
				{
					GetComponent<Animation>().Play("portero_parada_frontal_rasa", PlayMode.StopAll);
					if(Vector3.Distance(transform.position,FootBall.transform.position) < 4.5f)
					{
						timeSinceCaught = Time.time;
						GameManager.SharedObject().IsGameReady = false;
						playStandAnimation = true;
						ballScript.isKicked = false;
						ballScript.SetOwner(LeftHand);
					}
				}
				else if(GetComponent<Animation>()["portero_parada_frontal_rasa"].enabled == true/* && animation["portero_parada_frontal_rasa"].normalizedTime>=0.1f*/)
				{
					if(Vector3.Distance(transform.position,FootBall.transform.position) < 2.5f)
					{
						timeSinceCaught = Time.time;
						GameManager.SharedObject().IsGameReady = false;
						ballScript.SetOwner(LeftHand);
						playStandAnimation = true;
						ballScript.isKicked = false;
					}
				}
			}
		}
		else if(FootBall.transform.position.x > 43f && ballScript.ownerPlayer == LeftHand && playStandAnimation)
		{
			playStandAnimation = false;
			if(GetComponent<Animation>()["portero_levanta_balon"].enabled == false)
				GetComponent<Animation>().Play("portero_levanta_balon", PlayMode.StopAll);

			ballScript.isKicked = false;
		}
		else if(FootBall.transform.position.x >34f)
		{
			if(transform.position.z - FootBall.transform.position.z < -1 && FootBall.transform.position.z > -3.4f && FootBall.transform.position.z < 3.4f) // ball to left side
			{
				if(GetComponent<Animation>()["portero_guardia_izquierda"].enabled == false)
					GetComponent<Animation>().Play("portero_guardia_izquierda", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*-2f);
			}
			else if(transform.position.z - FootBall.transform.position.z > 1f && FootBall.transform.position.z > -3.4f && FootBall.transform.position.z < 3.4f)// ball to right side
			{
				if(GetComponent<Animation>()["portero_guardia_derecha"].enabled == false)
					GetComponent<Animation>().Play("portero_guardia_derecha", PlayMode.StopAll);

				transform.Translate(Vector3.right*Time.deltaTime*2f);
			}
			else if(GetComponent<Animation>()["portero_levanta_balon"].enabled == false)// ball in front
			{
				if(GetComponent<Animation>()["portero_guardia_reposo"].enabled == false)
					GetComponent<Animation>().Play("portero_guardia_reposo", PlayMode.StopAll);
			}
		}
		else
		{
			if(GetComponent<Animation>()["reposo"].enabled == false)
				GetComponent<Animation>().Play("reposo", PlayMode.StopAll);

			transform.rotation = InitialRotation;
			transform.position = InitialPosition;
		}

		Vector3 pos = transform.position;
		pos.y = 0f;
		transform.position = pos;
	}
}
