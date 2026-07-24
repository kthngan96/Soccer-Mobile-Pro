using UnityEngine;
using System.Collections;

public class OpponentPosition : MonoBehaviour
{
	public Transform InitialPositonTransform, SecondaryPositonTransform;
	private Vector3 InitialPosition, SecondaryPosition;
	
	private AI_Striker playerScript;

	// Use this for initialization
	void Start () {
		playerScript = InitialPositonTransform.GetComponent<AI_Striker> ();
		InitialPosition = InitialPositonTransform.position;
		SecondaryPosition = SecondaryPositonTransform.position;
	}
	
	// Update is called once per frame
	void FixedUpdate ()
	{
		if(PlayerPosition.PlayerTurn)
			playerScript.InitialPosition = InitialPosition;
		else
			playerScript.InitialPosition = SecondaryPosition;

		if(PlayerPosition.PlayerTurn == false && Vector3.Distance(transform.position, SecondaryPosition) < 1f)
		{
			GameManager.SharedObject().IsGameReady = true;
		}
	}
}
