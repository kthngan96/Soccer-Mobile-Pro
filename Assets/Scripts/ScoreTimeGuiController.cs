using UnityEngine;
using System.Collections;

public class ScoreTimeGuiController : MonoBehaviour {
	public float yPosition=0;
	// Use this for initialization
	void Start () {
//		yPosition = transform.position.y;
	}

//	void FixedUpdate ()
//	{
//		if(PauseController.isPaused)
//		{
//			if(yPosition!=2)
//			{
//				yPosition+=Time.deltaTime;
//				transform.position=new Vector3(0,yPosition,0);
//			}
//		}
//	}

	// Update is called once per frame
	void Update () {
	if(PauseController.isPaused)
		{
			if(transform.position.y!=2)
			{
//				yPosition+=Time.unscaledDeltaTime;
				transform.position=new Vector3(0,2,0);

			}


		}
		else if(transform.position.y!=0)
		{
			transform.position=new Vector3(0,0,0);
		}
	}
}
