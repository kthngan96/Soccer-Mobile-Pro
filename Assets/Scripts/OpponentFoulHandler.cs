using UnityEngine;
using System.Collections;

public class OpponentFoulHandler : MonoBehaviour
{
	public Transform foulWall1, foulWall2;
	
	public BallScript ballScript;
	private Transform[] players;
	public Transform LeftHand;
	
	private AI_MidfielderScript playerScript;
	
	Transform player1, player2, player3, player4;
	Vector3 position1, position2, position3, position4;

	private GameObject FootBall;
	
	private bool throwing = false;

	// Use this for initialization
	void Start()
	{
		playerScript = gameObject.GetComponent<AI_MidfielderScript> ();
		
		FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript> ();
		
		GameObject[] playersT = GameObject.FindGameObjectsWithTag ("AIMidfiielder");
		
		players = new Transform[playersT.Length];
		
		int i = 0;
		for(i = 0; i < playersT.Length; i++)
			players[i] = playersT[i].transform;
		
		player1 = players[0];
		player2 = players[1];
		player3 = players[2];
		player4 = players[3];
	}
	
	// Update is called once per frame
	void Update ()
	{
		if(!GameManager.SharedObject().OpponentMadeFoul && !GameManager.SharedObject().PlayerMadeFoul && playerScript.enabled == false)
		{
			gameObject.GetComponent<OpponentFoulHandler>().enabled = false;
			playerScript.enabled = true;
			ballScript.enabled = true;
			
			foulWall1.gameObject.SetActive(true);
			foulWall2.gameObject.SetActive(true);
			throwing = false;
			return;
		}
	
		//
		if(transform == player1)
		{
			if(GameManager.SharedObject().PlayerMadeFoul)
			{
				if(ballScript.ownerPlayer==null)
					ballScript.ownerPlayer = LeftHand;
				
				if(GetComponent<Animation>()["saque_banda"].enabled == false)
					GetComponent<Animation>().Play("saque_banda", PlayMode.StopAll);
				
				if(throwing == false)
				{
					GetComponent<Animation>()["saque_banda"].normalizedTime = 0F;
					FootBall.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
				}

				if(Vector3.Distance(transform.position, GameManager.SharedObject().foulPosition) > 0.5f)
					transform.position = GameManager.SharedObject().foulPosition;

				StartCoroutine(PassTheBall());
			}
			else
			{
				transform.rotation = Quaternion.LookRotation((GameManager.SharedObject().foulPosition - transform.position).normalized);
				if(GetComponent<Animation>()["reposo"].enabled == false)
					GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
			}
		}
		else if(transform == player2)
		{
			if(GameManager.SharedObject().PlayerMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x,0,GameManager.SharedObject().foulPosition.z+6f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x,0,GameManager.SharedObject().foulPosition.z-6f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z+4f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z-4f);
			}
		}
		else if(transform == player3)
		{
			if(GameManager.SharedObject().PlayerMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+6f,0,GameManager.SharedObject().foulPosition.z+4f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+6f,0,GameManager.SharedObject().foulPosition.z-4f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+5f,0,GameManager.SharedObject().foulPosition.z+4f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+5f,0,GameManager.SharedObject().foulPosition.z-4f);
			}
		}
		else if(transform == player4)
		{
			if(GameManager.SharedObject().PlayerMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-6f,0,GameManager.SharedObject().foulPosition.z+4f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-6f,0,GameManager.SharedObject().foulPosition.z-4f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,GameManager.SharedObject().foulPosition.z+4f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,GameManager.SharedObject().foulPosition.z-4f);
			}
		}
		
		if(transform != player1)
		{
			transform.rotation = Quaternion.LookRotation((GameManager.SharedObject().foulPosition - transform.position).normalized);
			if(GetComponent<Animation>()["reposo"].enabled == false)
				GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
		}
	}

	IEnumerator PassTheBall()
	{
		if(!throwing)
		{
		Transform t = player2;

		switch(Random.Range(0,3))
		{
			case 0:
				t = player2;
				break;

			case 1:
				t = player3;
				break;

			case 2:
				t = player4;
				break;

			default:
				t = player2;
				break;
		}

		transform.rotation = Quaternion.LookRotation((t.position - transform.position));
		
		throwing = true;
		yield return new WaitForSeconds(0.56f);

		FootBall.GetComponent<BallScript> ().SetFree();
		FootBall.GetComponent<Rigidbody>().velocity = transform.forward * 15f;//(new Vector3(player2.position.x,FootBall.transform.position.y,player2.position.z) - FootBall.transform.position).normalized * 15; 
		
		GameManager.SharedObject ().PlayerMadeFoul = false;
		GameManager.SharedObject ().OpponentMadeFoul = false;
		}
	}
}
