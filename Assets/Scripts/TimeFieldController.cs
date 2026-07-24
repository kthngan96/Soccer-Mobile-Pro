using UnityEngine;
using System.Collections;

public class TimeFieldController : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update ()
	{
		if(GameManager.SharedObject().IsGameReady == false)
			return;

		int minutes = GameManager.SharedObject ().GameTime / 60;
		float seconds = GameManager.SharedObject ().GameTime % 60;

		LegacyGuiUtility.GetOrAddGUIText(gameObject).text = (minutes<10?"0":"")+minutes + ":"+(seconds<10?"0":"")+ seconds;

		//if(minutes)
	}
}
