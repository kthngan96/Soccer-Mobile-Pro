using UnityEngine;
using System.Collections;

public class PlayerFoulHandler : MonoBehaviour
{
	public Transform foulWall1, foulWall2;

	public Texture passButton;
	public Texture passButtonSel;
	private bool passButtonPressed;
	private Rect passButtonRect;

	public BallScript ballScript;
	private Transform[] players;
	public Transform LeftHand;

	private Player playerScript;

	Transform player1, player2, player3, player4;
	Vector3 position1, position2, position3, position4;
	private GameObject FootBall;

	private bool throwing = false;

	private float foulTime = 0f;

	void Start()
	{
		passButtonRect = new Rect (Screen.width - GetValue(150), Screen.height - GetValue(150) - GetValue(130), GetValue(110), GetValue(110));

		playerScript = gameObject.GetComponent<Player>();

		FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
		ballScript = FootBall.GetComponent<BallScript>();

		GameObject[] playersT = GameObject.FindGameObjectsWithTag("Player");
		
		players = new Transform[playersT.Length];
		
		int i = 0;
		for(i = 0; i < playersT.Length; i++)
			players[i] = playersT[i].transform;

		player1 = players[0];
		player2 = players[1];
		player3 = players[2];
		player4 = players[3];
	}
	
	void Update()
	{
		if(Player.noControls)
		{
			if(GetComponent<Animation>()["reposo"].enabled == false)
				GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
			return;
		}

		if(!GameManager.SharedObject().OpponentMadeFoul && !GameManager.SharedObject().PlayerMadeFoul && playerScript.enabled == false)
		{
			gameObject.GetComponent<PlayerFoulHandler>().enabled = false;
			playerScript.enabled = true;
			ballScript.enabled = true;

			foulWall1.gameObject.SetActive(true);
			foulWall2.gameObject.SetActive(true);
			throwing = false;
			return;
		}

		if(transform == player1)
		{
			if(GameManager.SharedObject().OpponentMadeFoul)
			{
				if(ballScript.ownerPlayer==null)
				{
					ballScript.ownerPlayer = LeftHand;
					transform.position = GameManager.SharedObject().foulPosition;
					transform.LookAt(Vector3.zero);
					foulTime = 0;
				}

				if(GetComponent<Animation>()["saque_banda"].enabled == false)
					GetComponent<Animation>().Play("saque_banda", PlayMode.StopAll);

				if(throwing == false)
				{
					GetComponent<Animation>()["saque_banda"].normalizedTime = 0F;
					FootBall.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
				}

				//////				
				if(foulTime>5)
				{
					transform.rotation = Quaternion.LookRotation(transform.forward);
					StartCoroutine(PassTheBall());
				}
				foulTime += Time.deltaTime;
				//////

				#if !UNITY_EDITOR
				float x = -1*GameObject.Find("Single Joystick").GetComponent<Joystick>().position.x;
				float y = -1*GameObject.Find("Single Joystick").GetComponent<Joystick>().position.y;
				//			
				//				if((Mathf.Abs(x) > 0.1f) || (Mathf.Abs(y) > 0.1f))
				//					transform.eulerAngles = new Vector3(0, 90 + Mathf.Atan2(-y, x) * 180 / Mathf.PI, 0);
				
				if(x > 0.2f)
				{
					float limit = 120f;
					if(transform.position.z < 0)
						limit *= -1f;
					
					if(transform.eulerAngles.y > limit)
						transform.Rotate(0, -1*Time.deltaTime*40, 0);
				}
				else if(x < -0.2f)
				{
					float limit = 230f;
					if(transform.position.z < 0)
						limit *= -1f;
					
					if(transform.eulerAngles.y < limit)
						transform.Rotate(0, 1*Time.deltaTime*40, 0);
				}

				foreach(SoccerTouch touch in SoccerInput.Touches)
				{
					Vector2 inputGuiPosition = touch.position;
					inputGuiPosition.y = Screen.height - inputGuiPosition.y;
				
					if(touch.phase != SoccerTouchPhase.Canceled)
					{
						if(passButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
							passButtonPressed = true;
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(passButtonPressed == true)
							{
								passButtonPressed = false;
								StartCoroutine(PassTheBall());
								break;
							}
						}
					}
				}
				#endif


				#if UNITY_EDITOR
				if(SoccerInput.HorizontalAxis < 0)
					transform.Rotate(0, -1*Time.deltaTime*40, 0);
				else if(SoccerInput.HorizontalAxis > 0)
					transform.Rotate(0, 1*Time.deltaTime*40, 0);

				if(SoccerInput.PrimaryActionPressed)
					passButtonPressed = true;
				else if(passButtonPressed == true)
				{
					GetComponent<Animation>().Play("saque_banda", PlayMode.StopAll);
					passButtonPressed = false;
					StartCoroutine(PassTheBall());
				}
				#endif
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
			if(GameManager.SharedObject().OpponentMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x,0,GameManager.SharedObject().foulPosition.z+10f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x,0,GameManager.SharedObject().foulPosition.z-10f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z+8f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z-8f);
			}
		}
		else if(transform == player3)
		{
			if(GameManager.SharedObject().OpponentMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+8f,0,GameManager.SharedObject().foulPosition.z+8f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+8f,0,GameManager.SharedObject().foulPosition.z-8f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+5f,0,GameManager.SharedObject().foulPosition.z+8f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x+5f,0,GameManager.SharedObject().foulPosition.z-8f);
			}
		}
		else if(transform == player4)
		{
			if(GameManager.SharedObject().OpponentMadeFoul)
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-8f,0,GameManager.SharedObject().foulPosition.z+8f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-8f,0,GameManager.SharedObject().foulPosition.z-8f);
			}
			else
			{
				if(GameManager.SharedObject().foulPosition.z < 0)
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,GameManager.SharedObject().foulPosition.z+8f);
				else
					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,GameManager.SharedObject().foulPosition.z-8f);
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
		throwing = true;
		yield return new WaitForSeconds(0.56f);

		FootBall.GetComponent<BallScript>().SetFree();
		FootBall.GetComponent<Rigidbody>().velocity = transform.forward * 15f;//(new Vector3(player2.position.x,FootBall.transform.position.y,player2.position.z) - FootBall.transform.position).normalized * 15; 

		GameManager.SharedObject().PlayerMadeFoul = false;
		GameManager.SharedObject().OpponentMadeFoul = false;
	}

	void OnGUI()
	{

		if(!PauseController.isPaused)
		{
		if(transform == player1 && GameManager.SharedObject().OpponentMadeFoul)
		{
			if(passButtonPressed)
				GUI.DrawTexture(passButtonRect, passButtonSel);
			else
				GUI.DrawTexture(passButtonRect, passButton);
		}
		}
	
	}

	float GetValue(float value)
	{
		return value * Screen.height / 640f;
	}

	static float ClampAngle(float angle, float min, float max)
	{
		//angle = Mathf.LerpAngle(angle,angle,0);
		if (angle < -360f)
			angle += 360f;
		if (angle >  360f)
			angle -= 360f;
		return Mathf.Clamp(angle, min, max);
	}
}
