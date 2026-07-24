using UnityEngine;
using System.Collections;

#pragma warning disable CS0649, CS0414

public class Player : MonoBehaviour
{
    static bool waitForPass; /// <summary>
                             /// if true the expected player can't be moved for a specific time period...(it will wait for the ball)
                             /// </summary>
    public static float ballGetableDistance = 0.5f;
    public OtherDialoguesActive ode;
    public GameObject matchComplete, halfComplete;
    public Texture barTexture;
    //
    public Texture sprintButton;
    public Texture passButton;
    public Texture shootButton;
    public Texture tackleButton;

    public Texture sprintButtonSel;
    public Texture passButtonSel;
    public Texture shootButtonSel;
    public Texture tackleButtonSel;

    private bool sprintButtonPressed;
    private bool passButtonPressed;
    private bool shootButtonPressed;
    private bool tackleButtonPressed;
    //

    private Rect sprintButtonRect;
    private Rect passButtonRect;
    private Rect shootButtonRect;
    private Rect tackleButtonRect;

    int moveSpeed = 5;

    [HideInInspector]
    public bool isMoving = false;

    private float progress = 0;
    private Rect touchRect;
    private GameObject theBall;
    private BallScript theBallScript;

    private Transform[] players;
    private Transform highlight;

    public Vector3 initialPosition, targetPosition;

    public enum PlayerType { AttackerLeft, AttackerRight, DefenderLeft, DefenderRight, MidFielderLeft, MidFielderRight };

    public PlayerType playerType = PlayerType.AttackerLeft;

    private float sprintStamina = 10f;
    float lastTackleTime = 0f;

    public static bool noControls = false;
    void waitForPassMethod()
    {
        //		if (!waitForPass)
        //		{
        //			waitForPass = true;
        //			Invoke("waitForPassMethod",5);
        //		}
        //		else
        waitForPass = false;



    }
    void Start()
    {
        waitForPass = false;

        ode = GameObject.Find("Main Camera").GetComponent<OtherDialoguesActive>();
        //		noControls = false;

        lastTackleTime = Time.time;

        sprintButtonRect = new Rect(Screen.width - GetValue(150), Screen.height - GetValue(150), GetValue(130), GetValue(130));
        shootButtonRect = new Rect(Screen.width - GetValue(150) - GetValue(130), Screen.height - GetValue(150), GetValue(110), GetValue(110));
        tackleButtonRect = new Rect(Screen.width - GetValue(150) - GetValue(130), Screen.height - GetValue(150), GetValue(110), GetValue(110));
        passButtonRect = new Rect(Screen.width - GetValue(150), Screen.height - GetValue(150) - GetValue(130), GetValue(110), GetValue(110));

        initialPosition = transform.position;
        targetPosition = initialPosition;

        highlight = GameObject.Find("Highlight").transform;
        touchRect = new Rect(Screen.width / 3 * 2, 0, Screen.width / 3, Screen.height);
        theBall = GameObject.FindGameObjectWithTag("TheSoccerBall");

        GameObject[] playersT = GameObject.FindGameObjectsWithTag("Player");

        players = new Transform[playersT.Length];

        for (int i = 0; i < playersT.Length; i++)
            players[i] = playersT[i].transform;

        theBallScript = theBall.GetComponent<BallScript>();
    }

    Transform ControllablePlayer()
    {
        if (HasTheBall()) return transform;

        Transform idealPlayer = transform;

        foreach (Transform player in players)
        {
            if (Vector3.Distance(theBall.transform.position, player.position) < Vector3.Distance(theBall.transform.position, idealPlayer.position))
            {
                idealPlayer = player;
                //				Camera.main.GetComponent<SmoothFollow>().target=player;
            }

        }

        return idealPlayer;
    }

    bool HasTheBall()
    {
        return (theBall.GetComponent<BallScript>().ownerPlayer == transform);
    }

    private void MoveForward()
    {
        isMoving = false;

        float minZ = 0f;
        float maxZ = 0f;

        if (playerType == PlayerType.AttackerLeft || playerType == PlayerType.AttackerRight)
        {
            if (playerType == PlayerType.AttackerLeft)
            {
                minZ = 5f;
                maxZ = 36f;
            }
            else
            {
                minZ = -36f;
                maxZ = -5f;
            }

            if ((theBall.transform.position.x > transform.position.x || Vector3.Distance(theBall.transform.position, targetPosition) > 15f) && Vector3.Distance(transform.position, targetPosition) < 1f)
                targetPosition = new Vector3(theBall.transform.position.x + 10, transform.position.y, Random.Range(minZ, maxZ));
        }
        else if (playerType == PlayerType.MidFielderLeft || playerType == PlayerType.MidFielderRight)
        {
            if (playerType == PlayerType.MidFielderLeft)
            {
                minZ = 5f;
                maxZ = 36f;
            }
            else
            {
                minZ = -36f;
                maxZ = -5f;
            }

            if ((theBall.transform.position.x < transform.position.x || Vector3.Distance(theBall.transform.position, targetPosition) > 15f) && Vector3.Distance(transform.position, targetPosition) < 1f)
                targetPosition = new Vector3(theBall.transform.position.x - 10, transform.position.y, Random.Range(minZ, maxZ));
        }
        else if (playerType == PlayerType.DefenderLeft || playerType == PlayerType.DefenderRight)
        {
            if (playerType == PlayerType.DefenderLeft)
            {
                minZ = 5f;
                maxZ = 36f;
            }
            else
            {
                minZ = -36f;
                maxZ = -5f;
            }

            if ((theBall.transform.position.x < transform.position.x || Vector3.Distance(theBall.transform.position, targetPosition) > 25f) && Vector3.Distance(transform.position, targetPosition) < 1f && Vector3.Distance(initialPosition, targetPosition) < 40f)
                targetPosition = new Vector3(theBall.transform.position.x - 20, transform.position.y, Random.Range(minZ, maxZ));
            else if (Vector3.Distance(initialPosition, targetPosition) > 42f)
                targetPosition = initialPosition;
        }

        if (!GameManager.SharedObject().IsGameReady)
            targetPosition = initialPosition;

        float RotationSpeed = 100f;
        if (Vector3.Distance(transform.position, targetPosition) > 1f)
        {
            isMoving = true;

            //values for internal use
            Quaternion _lookRotation;
            Vector3 _direction;

            //find the vector pointing from our position to the target
            _direction = (targetPosition - transform.position).normalized;

            //create the rotation we need to be in to look at the target
            _lookRotation = Quaternion.LookRotation(_direction);
            _lookRotation.x = _lookRotation.z = 0f;
            //rotate us over time according to speed until we are in the required rotation
            transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);

            Vector3 currentPosition = this.transform.position;
            //first, check to see if we're close enough to the target
            if (Vector3.Distance(currentPosition, targetPosition) > .5f)
            {
                Vector3 directionOfTravel = targetPosition - currentPosition;
                directionOfTravel.Normalize();

                this.transform.Translate(
                    (directionOfTravel.x * moveSpeed * Time.deltaTime * 0.65f),
                    (directionOfTravel.y * moveSpeed * Time.deltaTime * 0.65f),
                    (directionOfTravel.z * moveSpeed * Time.deltaTime * 0.65f),
                    Space.World);
            }
        }
        else
        {
            isMoving = false;
            Quaternion _lookRotation;
            Vector3 _direction;

            _direction = (theBall.transform.position - transform.position).normalized;
            _lookRotation = Quaternion.LookRotation(_direction);
            _lookRotation.x = _lookRotation.z = 0f;
            transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
        }

        if (isMoving == false && GetComponent<Animation>()["entrada"].enabled == false)
            GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
        else if (GetComponent<Animation>()["corriendo"].enabled == false)
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
    }

    private void MoveTowardsTheBall()
    {
        if (GetComponent<Animation>()["tiro"].enabled == true || GetComponent<Animation>()["pase"].enabled == true || GetComponent<Animation>()["entrada"].enabled == true) return;

        isMoving = false;

        //move towards the center of the world (or where ever you like)
        Vector3 targetPosition = theBall.transform.position;
        targetPosition.y = transform.position.y;
        float RotationSpeed = 100f;

        if (!GameManager.SharedObject().IsGameReady)
            targetPosition = initialPosition;

        //values for internal use
        Quaternion _lookRotation;
        Vector3 _direction;

        //find the vector pointing from our position to the target
        _direction = (targetPosition - transform.position).normalized;

        //create the rotation we need to be in to look at the target
        _lookRotation = _direction != Vector3.zero ? Quaternion.LookRotation(_direction) : Quaternion.identity;
        _lookRotation.x = _lookRotation.z = 0f;
        //rotate us over time according to speed until we are in the required rotation
        transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);

        Vector3 currentPosition = this.transform.position;
        //first, check to see if we're close enough to the target
        if (Vector3.Distance(currentPosition, targetPosition) > .5f)
        {
            isMoving = true;
            Vector3 directionOfTravel = targetPosition - currentPosition;
            //now normalize the direction, since we only want the direction information
            directionOfTravel.Normalize();
            //scale the movement on each axis by the directionOfTravel vector components

            this.transform.Translate(
                (directionOfTravel.x * moveSpeed * Time.deltaTime * 0.65f),
                (directionOfTravel.y * moveSpeed * Time.deltaTime * 0.65f),
                (directionOfTravel.z * moveSpeed * Time.deltaTime * 0.65f),
                Space.World);
        }

        if (isMoving == true && GetComponent<Animation>()["corriendo"].enabled == false)
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
    }

    // Update is called once per frame
    void Update()
    {
        if (noControls)
        {
            if (GetComponent<Animation>()["reposo"].enabled == false)
                GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
            return;
        }

        if (GameManager.SharedObject().OpponentMadeFoul || GameManager.SharedObject().PlayerMadeFoul)
        {
            gameObject.GetComponent<Player>().enabled = false;
            gameObject.GetComponent<PlayerFoulHandler>().enabled = true;
            return;
        }

        if (GameManager.SharedObject().PlayerGotCornerKick || GameManager.SharedObject().OpponentGotCornerKick)
        {
            gameObject.GetComponent<Player>().enabled = false;
            gameObject.GetComponent<PCornerKickHandler>().enabled = true;
            return;
        }
        /////****0.5/////ballGetableDistance
        if (Vector3.Distance(theBall.transform.position, transform.position) < ballGetableDistance && GameManager.SharedObject().IsGameReady)
            theBall.GetComponent<BallScript>().SetOwnerIfPossible(transform);

        isMoving = false;
        if (transform == ControllablePlayer())
        {
            Vector3 position = transform.position;
            position.y = 0.01f;
            highlight.position = position;
#if !UNITY_EDITOR
            float multiplier = 1f;

            if (GameManager.SharedObject().IsFirstHalf)
                multiplier = 1f;
            else
                multiplier = -1f;

            if (GameManager.SharedObject().IsGameReady == false)
                multiplier = 0f;

			float x = multiplier*GameObject.Find("Single Joystick").GetComponent<Joystick>().position.x;
			float y = multiplier*GameObject.Find("Single Joystick").GetComponent<Joystick>().position.y;
			/////////////////////////**************************\\\\\\\\\\\\\\\\\\\\\\
			/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\\\\\\\ &&HasTheBall()
			if(((Mathf.Abs(x) > 0.1f) || (Mathf.Abs(y) > 0.1f))&&(!waitForPass||HasTheBall()))
			{	
				if(GetComponent<Animation>()["tiro"].enabled == false)
					transform.Translate(Vector3.forward*Time.deltaTime*moveSpeed * 0.65f);
				
				isMoving = true;
				
				if (GetComponent<Animation>()["corriendo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false)
					GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
				
				transform.eulerAngles = new Vector3(0, 90 + Mathf.Atan2(-y, x) * 180 / Mathf.PI, 0);
			}

			////////////************************************\\\\\\\\\\\\\\\\\\\\\\\\\
			/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\\\\\\\
			if(transform == ControllablePlayer())
				foreach(SoccerTouch touch in SoccerInput.Touches)
				{
					Vector2 inputGuiPosition = touch.position;
					inputGuiPosition.y = Screen.height - inputGuiPosition.y;
				
					if (touch.phase != SoccerTouchPhase.Canceled && HasTheBall())
					{
						if(sprintButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
						{
							sprintButtonPressed = true;
							break;
						}
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(sprintButtonPressed == true)
							{
								sprintButtonPressed = false;
								break;
							}
						}
					
						if(passButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
							passButtonPressed = true;
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(passButtonPressed == true)
							{
								passButtonPressed = false;
								//PASS CODE HERE..
								StartCoroutine(PassTheBall());
							Player.ballGetableDistance=2.5f;
							waitForPass=true;
							Invoke("waitForPassMethod",3.5f);
								break;
							}
						}
					
						if(shootButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
							shootButtonPressed = true;
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(shootButtonPressed == true)
							{
								shootButtonPressed = false;
								//SHOOT CODE HERE..
								progress = 1;
								StartCoroutine(KickTheBall());
								break;
							}
						}
					}
					if (touch.phase != SoccerTouchPhase.Canceled && !HasTheBall())
					{
						if(sprintButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
						{
							sprintButtonPressed = true;
							break;
						}
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(sprintButtonPressed == true)
							{
								sprintButtonPressed = false;
								break;
							}
						}
					
						if(tackleButtonRect.Contains(inputGuiPosition) && touch.phase != SoccerTouchPhase.Ended)
							tackleButtonPressed = true;
						else if(touch.phase == SoccerTouchPhase.Ended)
						{
							if(tackleButtonPressed == true)
							{
								tackleButtonPressed = false;
								//TACKLE CODE HERE..
								if(Time.time - lastTackleTime > 3 && theBallScript.ownerPlayer != null && Vector3.Distance(theBallScript.ownerPlayer.position, transform.position)<1 && GameManager.SharedObject().IsGameReady)
								{
									theBallScript.ownerPlayer.gameObject.GetComponent<Animation>().Play("entrada", PlayMode.StopAll);
									theBallScript.SetOwner(transform);
									lastTackleTime = Time.time;
								}
								break;
							}
						}
					}
					else if(touch.phase == SoccerTouchPhase.Canceled)
					{
						passButtonPressed = false;
						sprintButtonPressed = false;
						shootButtonPressed = false;
					}
					else if(progress > 0)
					{
						StartCoroutine(KickTheBall());
					}
				}
			
				if(sprintButtonPressed && sprintStamina > 0)
				{
					sprintStamina -= Time.deltaTime;
					moveSpeed = 8;
					GetComponent<Animation>()["corriendo"].speed= 1.5f;
				}
				else
				{
					GetComponent<Animation>()["corriendo"].speed= 1.0f;
					moveSpeed = 5;
				}
#endif

#if UNITY_EDITOR
            if (SoccerInput.VerticalAxis > 0.1f)
            {
                if (GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    transform.Translate(Vector3.forward * Time.deltaTime * moveSpeed * 0.65f);

                isMoving = true;

                if (GetComponent<Animation>()["corriendo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
            }

            if (SoccerInput.HorizontalAxis < 0)
            {
                if (GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    transform.Rotate(0, -20 * Time.deltaTime * moveSpeed, 0);

                if (GetComponent<Animation>()["corriendo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
            }

            if (SoccerInput.HorizontalAxis > 0)
            {
                if (GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    transform.Rotate(0, 20 * Time.deltaTime * moveSpeed, 0);

                if (GetComponent<Animation>()["corriendo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
                    GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
            }

            if (SoccerInput.PrimaryActionPressed && HasTheBall())
            {
                progress += Time.deltaTime;
                progress = (progress > 1) ? 1 : progress;
            }
            else if (progress > 0 && HasTheBall())
            {
                if (progress > 0.5f)
                    StartCoroutine(KickTheBall());
                else
                {
                    StartCoroutine(PassTheBall());
                    Player.ballGetableDistance = 2.5f;
                    waitForPass = true;
                    Invoke("waitForPassMethod", 6f);
                }
            }
#endif
        }
        if (isMoving == false && transform == ControllablePlayer() && !HasTheBall() && GetComponent<Animation>()["entrada"].enabled == false/* && Vector3.Distance(theBall.transform.position, transform.position)>5f*/)
            MoveTowardsTheBall();

        if (transform != ControllablePlayer() && !HasTheBall() && GetComponent<Animation>()["entrada"].enabled == false)
            MoveForward();

        if (isMoving == false && GetComponent<Animation>()["reposo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
            GetComponent<Animation>().Play("reposo", PlayMode.StopAll);

        if (GetComponent<Animation>()["corriendo"].enabled == false && isMoving == true && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);

        transform.position = new Vector3(transform.position.x, 0, transform.position.z);
    }

    IEnumerator KickTheBall()
    {
        if (GetComponent<Animation>()["tiro"].enabled == false)
            GetComponent<Animation>().Play("tiro", PlayMode.StopAll);

        progress = progress < 0.3f ? 0.3f : progress;
        AudioManager.PlayKickSound();
        yield return new WaitForSeconds(0.3f);

        theBall.GetComponent<BallScript>().SetFree();
        theBallScript.isKicked = true;

        Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 15 * progress, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
        theBall.transform.rotation = shotAngle;
        theBall.GetComponent<Rigidbody>().AddForce(theBall.transform.forward * 2500 * progress, ForceMode.Impulse);

        progress = 0;
    }

    IEnumerator PassTheBall()
    {
        float x = -100;

        Transform idealPlayer = null;
        float distance = 0;
        foreach (Transform player in players)
        {                                                                           ////////////********************60****\\\\\20				
            if (player != transform && player.position.x > x && Vector3.Distance(player.transform.position, transform.position) < 60 && Vector3.Distance(player.transform.position, transform.position) > 2)
            {
                x = player.position.x;
                idealPlayer = player;
                distance = Vector3.Distance(player.transform.position, transform.position);
            }
        }

        if (idealPlayer != null)
        {
            //			waitForPassMethod(); ///***** if some player is present in front then it can't be controlled untill it gets the ball...

            if (GetComponent<Animation>()["pase"].enabled == false)
                GetComponent<Animation>().Play("pase", PlayMode.StopAll);

            progress = progress < 0.3f ? 0.3f : progress;

            progress = 1;

            transform.rotation = Quaternion.LookRotation((idealPlayer.position - transform.position).normalized);
            Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x * progress, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
            theBall.transform.rotation = shotAngle;

            yield return new WaitForSeconds(0.3f);

            AudioManager.PlayKickSound();

            transform.rotation = Quaternion.LookRotation((idealPlayer.position - transform.position).normalized);
            shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x * progress, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
            theBall.transform.rotation = shotAngle;

            theBall.GetComponent<BallScript>().SetFree();              ///////****2000//40*50
            theBall.GetComponent<Rigidbody>().AddForce(theBall.transform.forward * distance * 95 * progress, ForceMode.Impulse);

            progress = 0;
            //			yield return new WaitForSeconds(5);
            //			waitForPass=false;
        }
        progress = 0;
    }

    void OnGUI()
    {
        //		GUI.Label (new Rect (Screen.width / 2, Screen.height / 2, 100, 20), "waitForPass"+waitForPass + "");
        if (!PauseController.isPaused && !InitGame.matchcomplete && !InitGame.halfComplete)

        {
            if (!GameManager.SharedObject().IsGameReady || PauseController.isPaused)
                return;

            if (HasTheBall())
            {
                GUI.BeginGroup(new Rect(Screen.width - GetValue(53), GetValue(236) + GetValue(10) - GetValue(236 * progress), GetValue(43), GetValue(236 * progress)));
                GUI.DrawTexture(new Rect(GetValue(0), GetValue(0), GetValue(43), GetValue(236)), barTexture);
                GUI.EndGroup();

                if (sprintButtonPressed)
                    GUI.DrawTexture(sprintButtonRect, sprintButtonSel);
                else
                    GUI.DrawTexture(sprintButtonRect, sprintButton);

                if (passButtonPressed)
                    GUI.DrawTexture(passButtonRect, passButtonSel);
                else
                    GUI.DrawTexture(passButtonRect, passButton);

                if (shootButtonPressed)
                    GUI.DrawTexture(shootButtonRect, shootButtonSel);
                else
                    GUI.DrawTexture(shootButtonRect, shootButton);

                /*if(GUI.Button(shootButtonRect,"",shootButton))
                {
                    progress = 1;
                    StartCoroutine(KickTheBall());
                }*/
            }
            else if (ControllablePlayer() == transform && !HasTheBall())
            {
                if (sprintButtonPressed)
                    GUI.DrawTexture(sprintButtonRect, sprintButtonSel);
                else
                    GUI.DrawTexture(sprintButtonRect, sprintButton);

                if (Time.time - lastTackleTime > 3)
                {
                    if (tackleButtonPressed)
                        GUI.DrawTexture(tackleButtonRect, tackleButtonSel);
                    else
                        GUI.DrawTexture(tackleButtonRect, tackleButton);
                }
            }
        }

    }

    float GetValue(float value)
    {
        return value * Screen.height / 640f;
    }
}
