using UnityEngine;
using System.Collections;

public class BallvelocityScript : MonoBehaviour {
	float camDistance=30;
	BallScript bscript;
	// Use this for initialization
	void Start () {
		bscript = GetComponent<BallScript> ();
	}
	
	// Update is called once per frame
	void Update () {
//		if(transform.position.x<-35)
//		{
//			Camera.main.GetComponent<SmoothFollow> ().target=GameObject.Find("playerGoal").transform;
//		}
		if(bscript.ownerPlayer!=null)
		{
		if(bscript.ownerPlayer.tag=="Player")
			{
				if(camDistance>15)
					camDistance -= Time.deltaTime*7;
			}
			else
				if(camDistance<20)
					camDistance += Time.deltaTime*7;

		}
		else
		if (GetComponent<Rigidbody>().velocity.magnitude > 5)
		{
			if(camDistance<30)
				camDistance += Time.deltaTime*7;
		}
		else
		{
			if(camDistance>20)
				camDistance -= Time.deltaTime*7;
		}
		Camera.main.GetComponent<SmoothFollow> ().distance = camDistance;
	}
//	void OnGUI()
//	{
//		GUI.Label (new Rect (50, 50, 300, 20), ""+rigidbody.velocity.magnitude);
//	}
}
