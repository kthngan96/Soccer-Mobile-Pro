using UnityEngine;
using System.Collections;

public class scorefieldFlagePosition2 : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		if (GameManager.SharedObject ().IsFirstHalf)
			transform.position = new Vector3 (0.324f, transform.position.y, transform.position.z);
		else
			transform.position = new Vector3 (0.08f, transform.position.y, transform.position.z);
	}
}
