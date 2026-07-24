using UnityEngine;
using System.Collections;

public class AI_DefenderScript : MonoBehaviour
{
    private Transform[] defenders;
    private Transform[] midfielders;

    private Transform[] opponents;

    public Transform goal;
    private int moveSpeed = 5;
    private Vector3 InitialPosition = Vector3.zero;
    private Vector3 targetPosition = Vector3.zero;

    private GameObject FootBall;
    private BallScript FootBallScript;

    static bool Position1Available = true;
    static bool Position2Available = true;
    static bool Position3Available = true;

    private int zOffset = 0;
    private int xOffset = 0;

    private float progress = 0f;

    private float timeToPass = 0f;
    private float waitForPass = 0f;

    [HideInInspector]
    public bool isMoving = false;
    private float lastTime = 0f;

    private float AttackTime = 0f;
    private bool Attack = false;

    // Use this for initialization
    void Start()
    {
        /*AtPosition1 = false;
		AtPosition2 = false;
		AtPosition3 = false;*/

        InitialPosition = transform.position;
        targetPosition = transform.position;

        FootBall = GameObject.FindGameObjectWithTag("TheSoccerBall");
        FootBallScript = FootBall.GetComponent<BallScript>();

        if (Position1Available)
        {
            xOffset = 5;
            zOffset = 5;
            Position1Available = false;
        }
        else if (Position2Available)
        {
            xOffset = 5;
            zOffset = -5;
            Position2Available = false;
        }
        else if (Position3Available)
        {
            xOffset = -5;
            zOffset = -5;
            Position3Available = false;
        }

        GameObject[] defendersT = GameObject.FindGameObjectsWithTag("AIDefender");
        GameObject[] midfieldersT = GameObject.FindGameObjectsWithTag("AIMidfiielder");

        GameObject[] opponentsT = GameObject.FindGameObjectsWithTag("Player");

        defenders = new Transform[defendersT.Length];
        midfielders = new Transform[midfieldersT.Length];
        opponents = new Transform[opponentsT.Length];

        int i = 0;
        for (i = 0; i < defendersT.Length; i++)
            defenders[i] = defendersT[i].transform;

        for (i = 0; i < midfieldersT.Length; i++)
            midfielders[i] = midfieldersT[i].transform;

        for (i = 0; i < opponentsT.Length; i++)
            opponents[i] = opponentsT[i].transform;
    }

    bool TeamHasTheBall()
    {
        return (FootBallScript.ownerPlayer && FootBallScript.ownerPlayer.gameObject.name == transform.gameObject.name);
    }

    private void MoveForward()
    {
        targetPosition = FootBall.transform.position;

        if (FootBall.transform.position.x > 28)
            targetPosition = new Vector3(FootBall.transform.position.x + xOffset, 0, ((FootBall.transform.position.z + zOffset < 37 && FootBall.transform.position.z + zOffset > -37) ? FootBall.transform.position.z + zOffset : targetPosition.z));
        else
            targetPosition = InitialPosition;

        targetPosition.y = transform.position.y;
        float RotationSpeed = 100;

        if (!GameManager.SharedObject().IsGameReady)
            targetPosition = InitialPosition;

        Quaternion _lookRotation = transform.rotation;
        Vector3 _direction;

        _direction = (targetPosition - transform.position).normalized;

        if (_direction.magnitude > 0.2f)
            _lookRotation = Quaternion.LookRotation(_direction);

        Vector3 currentPosition = this.transform.position;

        if (Vector3.Distance(currentPosition, targetPosition) > 2f)
        {
            isMoving = true;

            if (Time.time - lastTime > 0.3f)
            {
                transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
                lastTime = Time.time;
            }

            transform.Translate(Vector3.forward * Time.deltaTime * moveSpeed * 0.65f);
        }
        else
        {
            isMoving = false;

            _direction = (FootBall.transform.position - transform.position).normalized;
            _lookRotation = Quaternion.LookRotation(_direction);
            _lookRotation.x = _lookRotation.z = 0f;
            transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
        }

        if (isMoving == false)
            GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
        else// if(animation["corriendo"].enabled == false)
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
    }

    private void MoveTowardsTheBall()
    {
        if (GetComponent<Animation>()["tiro"].enabled == true || GetComponent<Animation>()["pase"].enabled == true) return;
        isMoving = true;

        targetPosition = FootBall.transform.position;

        if (FootBall.transform.position.x > 28)
        {
            AttackTime += Time.deltaTime;

            if (Attack == false && AttackTime > 4)
            {
                Attack = true;
                AttackTime = 0;
            }
            else if (Attack == true && AttackTime > 5)
            {
                AttackTime = 0;
                Attack = false;
            }

            if (Attack || FootBallScript.ownerPlayer == null)
                targetPosition = FootBall.transform.position;
            else
                targetPosition = new Vector3(FootBall.transform.position.x + 5, 0, FootBall.transform.position.z);
        }
        else
            targetPosition = InitialPosition;

        if (!GameManager.SharedObject().IsGameReady)
            targetPosition = InitialPosition;

        targetPosition.y = transform.position.y;
        float RotationSpeed = 100;

        Quaternion _lookRotation;
        Vector3 _direction;

        if (Time.time - lastTime > 0.3f)
        {
            _direction = (targetPosition - transform.position).normalized;
            _lookRotation = _direction != Vector3.zero ? Quaternion.LookRotation(_direction) : Quaternion.identity;
            _lookRotation.x = _lookRotation.z = 0f;
            transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
            lastTime = Time.time;
        }

        Vector3 currentPosition = this.transform.position;

        if (Vector3.Distance(currentPosition, targetPosition) > 0.4f)
            transform.Translate(Vector3.forward * Time.deltaTime * moveSpeed * 0.65f);
        else
        {
            isMoving = false;

            _direction = (FootBall.transform.position - transform.position).normalized;
            _lookRotation = _direction != Vector3.zero ? Quaternion.LookRotation(_direction) : Quaternion.identity;
            _lookRotation.x = _lookRotation.z = 0f;
            transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
        }
        GetComponent<Animation>()["pasos_atras"].speed = 2f;

        if (isMoving == false)
            GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
        else
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);
    }

    //
    private void MoveForGoal()
    {
        if (GetComponent<Animation>()["tiro"].enabled == true || GetComponent<Animation>()["pase"].enabled == true) return;
        isMoving = true;

        if (GetComponent<Animation>()["corriendo"].enabled == false)
            GetComponent<Animation>().Play("corriendo", PlayMode.StopAll);

        Vector3 targetPosition = goal.transform.position;
        targetPosition.y = transform.position.y;
        float RotationSpeed = 100;

        if (!GameManager.SharedObject().IsGameReady)
            targetPosition = InitialPosition;

        Vector3 _direction = (targetPosition - transform.position).normalized;

        Quaternion _lookRotation = Quaternion.LookRotation(_direction);
        _lookRotation.x = _lookRotation.z = 0f;
        transform.rotation = Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * RotationSpeed);
        //***** Move Towards the Goal while it has the ball*****\\\\\\
        if (Vector3.Distance(transform.position, targetPosition) > .5f)
            transform.Translate(Vector3.forward * Time.deltaTime * moveSpeed * 0.65f); //****StatesAndBehaviours.oppPlayerSpeed
    }
    void Update()
    {
        if (GameManager.SharedObject().OpponentMadeFoul || GameManager.SharedObject().PlayerMadeFoul || GameManager.SharedObject().PlayerGotCornerKick || GameManager.SharedObject().OpponentGotCornerKick)
        {
            transform.rotation = Quaternion.LookRotation((FootBall.transform.position - transform.position));

            if (GetComponent<Animation>()["reposo"].enabled == false)
                GetComponent<Animation>().Play("reposo", PlayMode.StopAll);
            return;
        }

        if (GameManager.SharedObject().PlayerGotCornerKick || GameManager.SharedObject().OpponentGotCornerKick)
        {
            gameObject.GetComponent<AI_DefenderScript>().enabled = false;
            gameObject.GetComponent<OCornerKickHandler>().enabled = true;
            return;
        }
        ///****Get The ball if its in range*****\\\\\
        if (Vector3.Distance(FootBall.transform.position, transform.position) < 0.5f && GameManager.SharedObject().IsGameReady)
            FootBallScript.SetOwnerIfPossible(transform);

        isMoving = false;
        ////***** Run towards the ball if its in range ****\\\\\
        if ((waitForPass <= 0 || Vector3.Distance(transform.position, FootBall.transform.position) < 60f) && isMoving == false && transform == ControllablePlayer() && !HasTheBall() && (FootBallScript.ownerPlayer == null || Vector3.Distance(transform.position, FootBall.transform.position) < 50f))////*******40***\\\\10
            MoveTowardsTheBall();

        if (waitForPass <= 0 && transform != ControllablePlayer() && !HasTheBall())
            MoveForward();

        if (transform == ControllablePlayer() && HasTheBall())
            MoveForGoal();

        if (isMoving == false && GetComponent<Animation>()["reposo"].enabled == false && GetComponent<Animation>()["tiro"].enabled == false && GetComponent<Animation>()["pase"].enabled == false && GetComponent<Animation>()["entrada"].enabled == false)
            GetComponent<Animation>().Play("reposo", PlayMode.StopAll);

        if (Vector3.Distance(transform.position, FootBall.transform.position) < 0.5f && transform == ControllablePlayer() && !HasTheBall() && GetComponent<Animation>()["entrada"].enabled == false && GameManager.SharedObject().IsGameReady)
        {
            if (FootBallScript.ownerPlayer)
            {
                if (FootBallScript.ownerPlayer.gameObject.GetComponent<Animation>())
                    FootBallScript.ownerPlayer.gameObject.GetComponent<Animation>().Play("entrada", PlayMode.StopAll);
            }
            FootBallScript.SetOwner(transform);
            timeToPass = 5f;
        }

        timeToPass -= Time.deltaTime;
        waitForPass -= Time.deltaTime;

        if (transform == ControllablePlayer() && HasTheBall())
            Debug.Log("time: " + timeToPass);

        if (transform == ControllablePlayer() && HasTheBall() && OpponentNearBy() && timeToPass < 0f)
            StartCoroutine(MakeAPass());
        else if (transform == ControllablePlayer() && HasTheBall() && (timeToPass < 0f || transform.position.x < 30f) && GameManager.SharedObject().IsGameReady)
        {
            transform.rotation = Quaternion.LookRotation((goal.position - transform.position).normalized);
            StartCoroutine(KickTheBall());
        }

        transform.position = new Vector3(transform.position.x, 0, transform.position.z);
    }

    void WaitForPass()
    {
        waitForPass = 1;
    }

    IEnumerator MakeAPass()
    {
        if (GetComponent<Animation>()["pase"].enabled == false)
            GetComponent<Animation>().Play("pase", PlayMode.StopAll);

        yield return new WaitForSeconds(0.3f);

        Quaternion _lookRotation;
        Vector3 _direction;

        Transform idealPlayer = null;

        float dot = -1f;
        foreach (Transform player in defenders)
        {
            Vector3 heading = player.position - transform.position;
            if (Vector3.Dot(heading, transform.forward) > dot)
            {
                dot = Vector3.Dot(heading, transform.forward);
                idealPlayer = player;
            }
        }

        foreach (Transform player in midfielders)
        {
            Vector3 heading = player.position - transform.position;
            if (Vector3.Dot(heading, transform.forward) > dot)
            {
                dot = Vector3.Dot(heading, transform.forward);
                idealPlayer = player;
            }
        }

        /*
		idealPlayer.GetComponent<ComputerPlayer> ().WaitForPass ();
		*/

        FootBallScript.SetFree();


        //find the vector pointing from our position to the target
        _direction = (idealPlayer.position - transform.position).normalized;

        //create the rotation we need to be in to look at the target
        _lookRotation = Quaternion.LookRotation(_direction);
        _lookRotation.x = _lookRotation.z = 0f;
        transform.rotation = _lookRotation;//Quaternion.Slerp(transform.rotation, _lookRotation, Time.deltaTime * 9999999999);

        Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 25 * progress, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
        FootBall.transform.rotation = shotAngle;

        FootBall.transform.rotation = shotAngle;
        FootBall.GetComponent<Rigidbody>().AddForce(FootBall.transform.forward * 300, ForceMode.Impulse);

        progress = 0;
    }

    IEnumerator KickTheBall()
    {
        if (FootBallScript.isKicked == false)
        {
            if (GetComponent<Animation>()["tiro"].enabled == false)
                GetComponent<Animation>().Play("tiro", PlayMode.StopAll);

            yield return new WaitForSeconds(0.3f);

            AudioManager.PlayKickSound();

            FootBallScript.SetFree();
            FootBallScript.isKicked = true;

            Quaternion shotAngle = Quaternion.Euler(new Vector3(transform.rotation.eulerAngles.x - 40, transform.rotation.eulerAngles.y, transform.rotation.eulerAngles.z));
            FootBall.transform.rotation = shotAngle;
            FootBall.GetComponent<Rigidbody>().AddForce(FootBall.transform.forward * 1500, ForceMode.Impulse);
        }

        yield return null;
    }

    Transform ControllablePlayer()
    {
        if (HasTheBall()) return transform;

        Transform idealPlayer = transform;

        foreach (Transform player in defenders)
        {
            if (Vector3.Distance(FootBall.transform.position, player.position) - Vector3.Distance(FootBall.transform.position, idealPlayer.position) < 1f)
                idealPlayer = player;
        }
        return idealPlayer;
    }

    private bool OpponentNearBy()
    {
        Transform opponentNB = null;

        foreach (Transform opponent in opponents)
        {
            if (Vector3.Distance(transform.position, opponent.position) < 2f)
                opponentNB = opponent;
        }
        return opponentNB != null;
    }

    bool HasTheBall()
    {
        return (FootBallScript.ownerPlayer == transform);
    }
}
