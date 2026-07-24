using UnityEngine;
using System.Collections;

public class pointTheTargetForTank : MonoBehaviour {

	public Transform model;  //Follow
	public Transform player;  //Target
	public Transform positionPlayer;
	// Use this for initialization
	void Start () {
		if (player == null)
			player = GameObject.Find ("BBHelicopterApache").transform;
	}
	
	// Update is called once per frame
	void Update () {
		transform.position = new Vector3 (positionPlayer.position.x, transform.position.y, positionPlayer.position.z);
		if(player!=null)
		{
			Vector3 tfmPosition=player.position - model.position;
//			
			model.rotation = Quaternion.Slerp (model.rotation, Quaternion.LookRotation (tfmPosition), Time.deltaTime * 30);
		}
	}
}