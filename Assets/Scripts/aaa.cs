using UnityEngine;
using System.Collections;

public class aaa : MonoBehaviour {

	public Transform pivot;
	public float x,y,z;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
		transform.RotateAround (pivot.position,new Vector3(x,y,z),60*Time.deltaTime);
	}
}
