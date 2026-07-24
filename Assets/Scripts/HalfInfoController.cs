using UnityEngine;
using System.Collections;

public class HalfInfoController : MonoBehaviour {
	GameManager manager;
	// Use this for initialization
	void Start () {
		manager = GameManager.SharedObject();
	}
	
	// Update is called once per frame
	void Update () {
		if(manager.IsFirstHalf) ///*** in order to switch teamNames and scores
			LegacyGuiUtility.GetOrAddGUIText(gameObject).text = "1st";
		else
			LegacyGuiUtility.GetOrAddGUIText(gameObject).text = "2nd";
	}
}
