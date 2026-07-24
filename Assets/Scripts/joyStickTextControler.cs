using UnityEngine;
using System.Collections;

public class joyStickTextControler : MonoBehaviour
{
    public Texture[] joystickTexture;
    public GameObject backgroundTexture;
    public static float joyStickPosition;
    // Use this for initialization
    void Start()
    {


    }

    // Update is called once per frame
    void Update()
    {

        joyStickPosition = GetComponent<Joystick>().defaultRect.x;
        if (LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x == joyStickPosition)
        {
            LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture = joystickTexture[0];
            LegacyGuiUtility.GetOrAddGUITexture(backgroundTexture).texture = joystickTexture[1];
        }
        else
        {
            LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture = joystickTexture[2];
            LegacyGuiUtility.GetOrAddGUITexture(backgroundTexture).texture = joystickTexture[3];
        }
        if (PauseController.isPaused || InitGame.halfComplete || InitGame.matchcomplete)
        {
            if (transform.position.y == 0f)
            {
                transform.position = new Vector3(0, 20, 3);
                backgroundTexture.transform.position = new Vector3(0.05f, 20, 2.870852f);
            }
        }
        else
            if (transform.position.y == 20)
        {
            transform.position = new Vector3(0, 0, 3);
            backgroundTexture.transform.position = new Vector3(0.05f, 0.06f, 2.870852f);
        }
        //
        //		if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        ////			if(joystick.position.x==-7.629394e-08f)
        //		{
        ////			print ("tap>0");
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[0];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[4];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[2];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[3];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[1];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[8];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y>59f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[5];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x>82f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[6];
        //		}
        //		else if(LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x<72f&&LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.y<69f)
        //		{
        //			LegacyGuiUtility.GetOrAddGUITexture(gameObject).texture=joystickTexture[7];
        //		}
        //		LegacyGuiUtility.GetOrAddGUITexture(gameObject).pixelInset.x
    }
    //	void OnGUI()
    //	{
    //		GUI.Label (new Rect (Screen.width / 2, Screen.height / 2, 300, 20), "" + GetComponent<GUITexture> ().pixelInset);
    //		GUI.Label (new Rect (Screen.width / 2, Screen.height / 2+20, 300, 20), "" + joyStickPosition);
    //
    //	}
}
