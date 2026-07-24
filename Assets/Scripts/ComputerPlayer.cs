using UnityEngine;
using System.Collections;

public class ComputerPlayer : MonoBehaviour
{
	/*
	public Transform goal;
	private bool trick = false;

	int moveSpeed = 5;
	


	public Texture barTexture;
	private float progress = 0;
	private Rect touchRect;
	private bool touchEnded = false;
	private GameObject theBall;

	private Transform[] players;
	private Transform[] opponents;

	private Vector3 initialPosition, targetPosition;

	public enum PlayerType {AttackerLeft, AttackerRight, DefenderLeft, DefenderRight, MidFielderLeft, MidFielderRight};

	public PlayerType playerType = PlayerType.AttackerLeft;

	[HideInInspector]
	public bool isMoving = false;
	private float lastTime = 0;

	private float timeToPass = 0;
	private float waitForPass = 0;

	// Use this for initialization
	void Start()
	{
		lastTime = Time.time;
		timeToPass = Time.time;

		initialPosition = transform.position;
		targetPosition = initialPosition;

		touchRect = new Rect (Screen.width / 3 * 2, 0, Screen.width / 3, Screen.height);
		theBall = GameObject.FindGameObjectWithTag("TheSoccerBall");

		GameObject[] playersT = GameObject.FindGameObjectsWithTag("ComputerPlayer");
		GameObject[] opponentsT = GameObject.FindGameObjectsWithTag("Player");

		players = new Transform[playersT.Length];
		opponents = new Transform[opponentsT.Length];

		int i = 0;
		for(i = 0; i < playersT.Length; i++)
			players[i] = playersT[i].transform;

		for(i = 0; i < opponentsT.Length; i++)
			opponents[i] = opponentsT[i].transform;
	}

	Transform ControllablePlayer()
	{
		if(HasTheBall()) return transform;

		Transform idealPlayer = transform;

		foreach(Transform player in players)
		{
			if(Vector3.Distance(theBall.transform.position,player.position) - Vector3.Distance(theBall.transform.position,idealPlayer.position) < 1f)
				idealPlayer = player;
		}
		return idealPlayer;
	}

	private bool OpponentNearBy()
	{
		Transform opponentNB = null;
		
		foreach(Transform opponent in opponents)
		{
			if(Vector3.Distance(transform.position,opponent.position) < 2f)
				opponentNB = opponent;
		}
		return opponentNB != null;
	}

	bool HasTheBall()
	{
		return (theBall.GetComponent<BallScript> ().ownerPlayer == transform);
	}

	private void MoveForward()
	{
		float minZ = 0f;
		float maxZ = 0f;
		float xPos = 0f;

		targetPosition = theBall.transform.position;

		if(playerType == PlayerType.AttackerLeft || playerType == PlayerType.AttackerRight)
		{
			minZ = transform.position.z - 3f;
			maxZ = transform.position.z + 3f;
			xPos = theBall.transform.position.x+10;

			if(xPos > 49) xPos = 49;
			if(minZ < -30) minZ = -30;
			if(maxZ > 30) maxZ = 30;

			if((theBall.transform.position.x > transform.position.x || Vector3.Distance(theBall.transform.position, targetPosition)>15f) && Vector3.Distance(transform.position, targetPosition)<1f)
				targetPosition = new Vector3(xPos,transform.position.y,Random.Range(minZ,maxZ));
		}
		else if(playerType == PlayerType.MidFielderLeft || playerType == PlayerType.MidFielderRight)
		{
			minZ = transform.position.z - 3f;
			maxZ = transform.position.z + 3f;
			xPos = theBall.transform.position.x-10;

			if(xPos < -49) xPos = -49;
			if(minZ < -30) minZ = -30;
			if(maxZ > 30) maxZ = 30;

			if((theBall.transform.position.x < transform.position.x || Vector3.Distance(theBall.transform.position, targetPosition)>15f)&& Vector3.Distance(transform.position, targetPosition)<1f)
				targetPosition = new Vector3(xPos,transform.position.y,Random.Range(minZ,maxZ));
		}		
		else if(playerType == PlayerType.DefenderLeft || playerType == PlayerType.DefenderRight)
		{
			if(theBall.transform.position.x > 10)
			{
				if(playerType == PlayerType.DefenderLeft)
					targetPosition = new Vector3(theBall.transform.position.x+5,0,theBall.transform.position.z+10);
				else
					targetPosition = new Vector3(theBall.transform.position.x+5,0,theBall.transform.position.z-10);
			}
			else
				targetPosition = initialPosition;
		}
		
		targetPosition.y = transform.position.y;
		float RotationSpeed = 50;
		
		Quaternion _lookRotation;
		Vector3 _direction;
		
		//find the vector pointing from our position to the target
		_direction = (targetPosition - transform.position).normalized;
		
		//create the rotation we need to be in to look at the target
		_lookRotation = Quaternion.LookRotation(_direction);
		
		//rotate us over time according to speed until we are in the required rotation
		

		
		Vector3 currentPosition = this.transform.position;
		
		if(Vector3.Distance(currentPosition, targetPosition) > 2f)
		{ 
			isMoving = true;

			if(Time.time - lastTime > 0.3f)
			{
				transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
				lastTime = Time.time;
			}

			transform.Translate(Vector3.forward*Time.deltaTime*moveSpeed * 0.65f);
		}
		else
		{
			isMoving = false;
			
			_direction = (theBall.transform.position - transform.position).normalized;
			_lookRotation = Quaternion.LookRotation(_direction);
			transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
		}
		
		if(isMoving == false)
			animation.Play("reposo", PlayMode.StopAll);
		else// if(animation["corriendo"].enabled == false)
			animation.Play("corriendo", PlayMode.StopAll);
	}

	private void MoveTowardsTheBall()
	{
		if(animation["tiro"].enabled == true || animation["pase"].enabled == true) return;
		isMoving = true;

		targetPosition = theBall.transform.position;

		if((playerType == PlayerType.DefenderLeft || playerType == PlayerType.DefenderRight) && theBall.GetComponent<BallScript>().ownerPlayer != null)
		{
			if(theBall.transform.position.x > 10)
				targetPosition = new Vector3(theBall.transform.position.x+5,0,theBall.transform.position.z);
			else
				targetPosition = initialPosition;
		}
		targetPosition.y = transform.position.y;
		float RotationSpeed = 50;
		
		Quaternion _lookRotation;
		Vector3 _direction;

		//find the vector pointing from our position to the target
		_direction = (targetPosition - transform.position).normalized;
		
		//create the rotation we need to be in to look at the target
		_lookRotation = Quaternion.LookRotation(_direction);
		
		//rotate us over time according to speed until we are in the required rotation

		if(Time.time - lastTime > 0.3f)
		{
			transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
			lastTime = Time.time;
		}

		Vector3 currentPosition = this.transform.position;

		if(Vector3.Distance(currentPosition, targetPosition) > 0.4f)
		{ 
			transform.Translate(Vector3.forward*Time.deltaTime*moveSpeed * 0.65f);
		}
		else
		{
			isMoving = false;
			
			_direction = (theBall.transform.position - transform.position).normalized;
			_lookRotation = Quaternion.LookRotation(_direction);
			transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
		}
		
		if(isMoving == false)
			animation.Play("reposo", PlayMode.StopAll);
		else// if(animation["corriendo"].enabled == false)
			animation.Play("corriendo", PlayMode.StopAll);
	}

	//
	private void MoveForGoal()
	{
		if(animation["tiro"].enabled == true || animation["pase"].enabled == true) return;
		isMoving = true;
		
		if(animation["corriendo"].enabled == false)
			animation.Play("corriendo", PlayMode.StopAll);
		
		Vector3 targetPosition = goal.transform.position;
		targetPosition.y = transform.position.y;
		float RotationSpeed = 50;
		
		Quaternion _lookRotation;
		Vector3 _direction;
		
		//find the vector pointing from our position to the target
		_direction = (targetPosition - transform.position).normalized;
		
		//create the rotation we need to be in to look at the target
		_lookRotation = Quaternion.LookRotation(_direction);

		transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);

		if(Vector3.Distance(transform.position, targetPosition) > .5f)
			transform.Translate(Vector3.forward*Time.deltaTime*moveSpeed * 0.65f);
	}

	// Update is called once per frame
	void Update()
	{
		if(Vector3.Distance(theBall.transform.position,transform.position) < 0.5f)
			theBall.GetComponent<BallScript>().SetOwnerIfPossible(transform);

		isMoving = false;

#if !UNITY_EDITOR
		float x = GameObject.Find("Single Joystick").GetComponent<Joystick>().position.x;
		float y = GameObject.Find("Single Joystick").GetComponent<Joystick>().position.y;

		if((Mathf.Abs(x) > 0.1f) || (Mathf.Abs(y) > 0.1f))
		{	
			if(animation["tiro"].enabled == false)
				transform.Translate(Vector3.forward*Time.deltaTime*moveSpeed);

			isMoving = true;

			if (animation["corriendo"].enabled == false && animation["tiro"].enabled == false)
				animation.Play("corriendo", PlayMode.StopAll);

			transform.eulerAngles = new Vector3(0, 90 + Mathf.Atan2(-y, x) * 180 / Mathf.PI, 0);
		}

		if(isMoving == false && animation["reposo"].enabled == false && animation["tiro"].enabled == false)
			animation.Play("reposo", PlayMode.StopAll);

		foreach(SoccerTouch touch in SoccerInput.Touches)
		{
			if (touch.phase != SoccerTouchPhase.Ended && touch.phase != SoccerTouchPhase.Canceled)
			{
				if(touchRect.Contains(touch.position))
				{
					progress += Time.deltaTime;
					progress = (progress>1)?1:progress;
				}

			}
			else if(progress > 0)
			{
				if (animation["tiro"].enabled == false)
					animation.Play("tiro", PlayMode.StopAll);

				GameObject theBall = GameObject.FindGameObjectWithTag ("TheSoccerBall");
				theBall.GetComponent<BallScript> ().ownerPlayer = null;
				theBall.GetComponent<Rigidbody>().AddForce(transform.forward, 10, ForceMode.Impulse);
				progress = 0;
			}
		}
#endif

#if UNITY_EDITOR
		if((waitForPass <= 0 || Vector3.Distance(transform.position, theBall.transform.position)<5f) && isMoving == false && transform == ControllablePlayer() && !HasTheBall()  && (theBall.GetComponent<BallScript> ().ownerPlayer == null || Vector3.Distance(transform.position, theBall.transform.position)<10f))
			MoveTowardsTheBall();
		
		if(waitForPass <= 0 && transform != ControllablePlayer() && !HasTheBall())
			MoveForward();

		if(transform == ControllablePlayer() && HasTheBall())
			MoveForGoal();

		if(isMoving == false && animation["reposo"].enabled == false && animation["tiro"].enabled == false && animation["pase"].enabled == false && animation["entrada"].enabled == false)
			animation.Play("reposo", PlayMode.StopAll);

		if(Vector3.Distance(transform.position, theBall.transform.position)<0.5f && transform == ControllablePlayer() && !HasTheBall())
		{
			if(theBall.GetComponent<BallScript>().ownerPlayer)
				theBall.GetComponent<BallScript>().ownerPlayer.gameObject.animation.Play("entrada", PlayMode.StopAll);

			theBall.GetComponent<BallScript>().SetOwner(transform);
			timeToPass = 5f;
		}
#endif

		timeToPass -= Time.deltaTime;
		waitForPass -= Time.deltaTime;

		if(transform == ControllablePlayer() && HasTheBall())
			Debug.Log ("time: "+timeToPass);

		if(transform == ControllablePlayer() && HasTheBall() && OpponentNearBy() && timeToPass <= 0)
		{
			StartCoroutine(MakeAPass());
		}
	}

	void WaitForPass()
	{
		waitForPass = 1;
	}

	IEnumerator MakeAPass()
	{
		if(animation["pase"].enabled == false)
			animation.Play("pase", PlayMode.StopAll);

		yield return new WaitForSeconds(0.3f);

		Quaternion _lookRotation;
		Vector3 _direction;

		Transform idealPlayer = null;

		float dot = -1f;
		foreach(Transform player in players)
		{
			Vector3 heading = player.position - transform.position;
			if(Vector3.Dot(heading, transform.forward) > dot)
			{
				dot = Vector3.Dot(heading, transform.forward);
				idealPlayer = player;
			}
		}
		idealPlayer.GetComponent<ComputerPlayer> ().WaitForPass ();
		progress = Vector3.Distance(transform.position, idealPlayer.position)/500;
		//find the vector pointing from our position to the target
		_direction = (idealPlayer.position - transform.position).normalized;
		
		//create the rotation we need to be in to look at the target
		_lookRotation = Quaternion.LookRotation(_direction);
		
		transform.rotation = _lookRotation;//Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * 9999999999);
		
		Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 25 * progress,transform.rotation.eulerAngles.y,transform.rotation.eulerAngles.z));
		theBall.transform.rotation = shotAngle;
		theBall.GetComponent<BallScript> ().ownerPlayer = null;
		theBall.GetComponent<Rigidbody>().AddForce(theBall.transform.forward*3000*progress, ForceMode.Impulse);
		
		progress = 0;
	}

	IEnumerator KickTheBall()
	{
		if(animation["tiro"].enabled == false)
			animation.Play("tiro", PlayMode.StopAll);

		progress = progress < 0.3f ? 0.3f : progress;

		yield return new WaitForSeconds(0.3f);
		theBall.GetComponent<BallScript> ().ownerPlayer = null;
		
		Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 25 * progress,transform.rotation.eulerAngles.y,transform.rotation.eulerAngles.z));
		theBall.transform.rotation = shotAngle;
		theBall.GetComponent<Rigidbody>().AddForce(theBall.transform.forward*3000*progress, ForceMode.Impulse);

		progress = 0;
	}

	void OnGUI()
	{
		if(HasTheBall())
		{
			GUI.BeginGroup(new Rect(Screen.width-GetValue(53), GetValue(236)+GetValue(10)-GetValue(236*progress), GetValue(43), GetValue(236*progress)));
			GUI.DrawTexture(new Rect(GetValue(0), GetValue(0), GetValue(43), GetValue(236)), barTexture);
			GUI.EndGroup();
		}
	}

	float GetValue(float v)
	{
		return v * Screen.height / 640f;
	}
	*/
}
