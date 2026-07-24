using UnityEngine;
using System.Collections;

public class GoalCeleberationManager : MonoBehaviour
{
	int globlePlayerIndex;
	public Camera mCamera;
	private static GoalCeleberationManager manager;

	public GameObject[] players;

	void Awake()
	{
		foreach(GameObject p in players)
		{
			CPlayerScript cps = p.GetComponent<CPlayerScript>();
			p.SetActive(false);
		}

		gameObject.GetComponent<Camera> ().enabled = false;
	}

	void Start ()
	{
		manager = gameObject.GetComponent<GoalCeleberationManager> ();
	}

	public static void PlayCeleberation(int PlayerIndex)
	{
		manager.PlayCAnimations (PlayerIndex);
	}

	void PlayCAnimations(int PlayerIndex)
	{
//		foreach(GameObject p in players)
//		{
		GameObject p=players[PlayerIndex];
			p.SetActive(true);
			CPlayerScript cps = p.GetComponent<CPlayerScript>();
			cps.AnimatePlayer();
//		}
		mCamera.enabled = false;
		gameObject.GetComponent<Camera> ().enabled = true;
		globlePlayerIndex = PlayerIndex;
		Invoke ("Reset",2.5f);
	}

	void Reset()
	{
//		foreach(GameObject p in players)
//		{
		GameObject p=players[globlePlayerIndex];
			CPlayerScript cps = p.GetComponent<CPlayerScript>();
			cps.Reset();
			p.SetActive(false);
//		}
		mCamera.enabled = true;
		gameObject.GetComponent<Camera> ().enabled = false;
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
