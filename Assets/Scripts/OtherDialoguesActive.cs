using UnityEngine;
using System.Collections;

public class OtherDialoguesActive : MonoBehaviour {
	public GameObject matchCompleted,halfCompleted;
	public bool isOtherDialogueActive;
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () 
	{
		if(matchCompleted==null||halfCompleted==null)
		{
			isOtherDialogueActive=true;
		}
		else
			isOtherDialogueActive=false;
	}
}
