using UnityEngine;
using System.Collections;

public class PCornerKickHandler : MonoBehaviour
{
    public Texture passButton;
    public Texture passButtonSel;
    private bool passButtonPressed;
    private Rect passButtonRect;

    [HideInInspector]
    public BallScript ballScript;
    private Transform[] players;

    private Player playerScript;

    Transform player1, player2, player3, player4;
    Vector3 position1, position2, position3, position4;
    private GameObject FootBall;

    private bool throwing = false;

    public GameObject mCam;
    public GameObject sCam;

    void Start()
    {
        passButtonRect = new Rect(Screen.width - GetValue(150), Screen.height - GetValue(150) - GetValue(130), GetValue(110), GetValue(110));

        playerScript = gameObject.GetComponent<Player>();

        FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
        ballScript = FootBall.GetComponent<BallScript>();

        GameObject[] playersT = GameObject.FindGameObjectsWithTag("Player");

        players = new Transform[playersT.Length];

        int i = 0;
        for (i = 0; i < playersT.Length; i++)
            players[i] = playersT[i].transform;

        player1 = players[0];
        player2 = players[1];
        player3 = players[2];
        player4 = players[3];
    }

    void Update()
    {
        if (Player.noControls)
        {
            if (GetComponent<Animation>()["reposo"].enabled == false)
                GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
            return;
        }

        if (!GameManager.SharedObject().OpponentGotCornerKick && !GameManager.SharedObject().PlayerGotCornerKick)
        {
            gameObject.GetComponent<PCornerKickHandler>().enabled = false;
            ballScript.enabled = true;
            ballScript.SetFree();

            if (!mCam.activeInHierarchy)
                mCam.SetActive(true);

            if (sCam.activeInHierarchy)
            {
                sCam.SetActive(false);
                sCam.GetComponent<SmoothFollow>().target = null;
            }

            playerScript.enabled = true;
            throwing = false;
            return;
        }

        if (transform == player1)
        {
            if (GameManager.SharedObject().PlayerGotCornerKick)
            {
                Vector3 targetPosition = Vector3.zero;

                if (GameManager.SharedObject().foulPosition.z < 0)
                    targetPosition = new Vector3(GameManager.SharedObject().foulPosition.x, 0f, -39f);
                else
                    targetPosition = new Vector3(GameManager.SharedObject().foulPosition.x, 0f, 39f);

                if (Vector3.Distance(transform.position, targetPosition) > 3)
                    transform.position = targetPosition;

                if (mCam.activeInHierarchy)
                    mCam.SetActive(false);

                if (sCam.activeInHierarchy == false)
                {
                    sCam.SetActive(true);
                    sCam.GetComponent<SmoothFollow>().target = transform;
                }

                if (GetComponent<Animation>()["reposo"].enabled == false && throwing == false)
                    GetComponent<Animation>().Play("reposo", PlayMode.StopAll);

                transform.LookAt(FootBall.transform.position);
#if !UNITY_EDITOR
				float x = GameObject.Find("Single Joystick").GetComponent<Joystick>().position.x;

				if(x < 0)
					transform.Translate(Vector3.right*Time.deltaTime*-1);
				else if(x > 0)
					transform.Translate(Vector3.right*Time.deltaTime*1);
				
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
								GetComponent<Animation>().Play("saque_esquina", PlayMode.StopAll);
								throwing = true;
								passButtonPressed = false;
								//PASS CODE HERE..
								StartCoroutine(PassTheBall());
								break;
							}
						}
					}
				}
#endif

#if UNITY_EDITOR
                if (SoccerInput.HorizontalAxis < 0)
                    transform.Translate(Vector3.right * Time.deltaTime * -1);
                else if (SoccerInput.HorizontalAxis > 0)
                    transform.Translate(Vector3.right * Time.deltaTime * 1);

                if (SoccerInput.PrimaryActionPressed)
                    passButtonPressed = true;
                else if (passButtonPressed == true)
                {
                    GetComponent<Animation>().Play("saque_esquina", PlayMode.StopAll);
                    throwing = true;
                    passButtonPressed = false;
                    //PASS CODE HERE..
                    StartCoroutine(PassTheBall());
                }
#endif

                if (throwing == true)
                {
                    Vector3 currentPosition = this.transform.position;

                    //if(Vector3.Distance(currentPosition, FootBall.transform.position) > .4f)
                    transform.forward *= (600 * Time.deltaTime);
                }
            }
            else
            {
                transform.rotation = Quaternion.LookRotation((GameManager.SharedObject().foulPosition - transform.position).normalized);

                if (GetComponent<Animation>()["reposo"].enabled == false)
                    GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
            }
        }
        else if (transform == player2)
        {
            //			if(GameManager.SharedObject().PlayerGotCornerKick)
            //			{
            if (GameManager.SharedObject().foulPosition.z < 0)
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 8, 0, -2f);
            else
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 8, 0, -2f);
            //			}
            //			else
            //			{
            //				if(GameManager.SharedObject().foulPosition.z < 0)
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z+8f);
            //				else
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-3,0,GameManager.SharedObject().foulPosition.z-8f);
            //			}
        }
        else if (transform == player3)
        {
            //			if(GameManager.SharedObject().PlayerGotCornerKick)
            //			{
            if (GameManager.SharedObject().foulPosition.z < 0)
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 10f, 0, 2f);
            else
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 10f, 0, 2f);
            //			}
            //			else
            //			{
            //				if(GameManager.SharedObject().foulPosition.z < 0)
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-8f,0,8f);
            //				else
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-8f,0,-8f);
            //			}
        }
        else if (transform == player4)
        {
            //			if(GameManager.SharedObject().PlayerGotCornerKick)
            //			{
            if (GameManager.SharedObject().foulPosition.z < 0)
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 5f, 0, 0);
            else
                transform.position = new Vector3(GameManager.SharedObject().foulPosition.x + 5f, 0, 0);
            //			}
            //			else
            //			{
            //				if(GameManager.SharedObject().foulPosition.z < 0)
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,8f);
            //				else
            //					transform.position = new Vector3(GameManager.SharedObject().foulPosition.x-5f,0,-8f);
            //			}
        }

        if (transform != player1)
        {
            transform.rotation = Quaternion.LookRotation((GameManager.SharedObject().foulPosition - transform.position).normalized);
            if (GetComponent<Animation>()["reposo"].enabled == false)
                GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
        }
    }

    IEnumerator PassTheBall()
    {
        yield return new WaitForSeconds(0.95f);

        AudioManager.PlayKickSound();

        FootBall.GetComponent<BallScript>().SetFree();
        //FootBall.rigidbody.velocity = transform.forward * 15f;//(new Vector3(player2.position.x,FootBall.transform.position.y,player2.position.z) - FootBall.transform.position).normalized * 15; 

        Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 20, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
        FootBall.transform.rotation = shotAngle;
        FootBall.GetComponent<Rigidbody>().AddForce(FootBall.transform.forward * 2200, ForceMode.Impulse);

        GameManager.SharedObject().PlayerGotCornerKick = false;
        GameManager.SharedObject().OpponentGotCornerKick = false;
        GameManager.SharedObject().IsGameReady = true;
    }

    void OnGUI()
    {

        if (!PauseController.isPaused)
        {
            if (transform == player1 && GameManager.SharedObject().PlayerGotCornerKick)
            {
                if (passButtonPressed)
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
}
