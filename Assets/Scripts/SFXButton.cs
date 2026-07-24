using UnityEngine;
using System.Collections;

[ExecuteAlways]
public class SFXButton : MonoBehaviour
{
	public Texture onTexture, offTexture;
	private GUITexture legacyTexture;
	private bool pressedInside;
	private int lastToggleFrame = -1;

	void OnEnable()
	{
		SyncTexture();
	}

	void OnValidate()
	{
		SyncTexture();
	}

	void Start()
	{
		legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		SyncTexture();
	}

	void Update ()
	{
		if(legacyTexture == null)
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);

		SyncTexture();

		if(!Application.isPlaying)
			return;

		UpdatePointerInput();
	}
	
	void OnMouseDown()
	{
		ToggleSfx();
	}

	private void UpdatePointerInput()
	{
		if(legacyTexture == null)
			return;

		if(SoccerInput.PointerPressedThisFrame)
		{
			pressedInside = legacyTexture.HitTest(SoccerInput.PointerPosition);
		}

		if(SoccerInput.PointerReleasedThisFrame)
		{
			bool releaseInside = legacyTexture.HitTest(SoccerInput.PointerPosition);
			if(pressedInside && releaseInside)
				ToggleSfx();

			pressedInside = false;
		}

		for(int i = 0; i < SoccerInput.TouchCount; i++)
		{
			SoccerTouch touch = SoccerInput.GetTouch(i);
			Vector3 touchPosition = new Vector3(touch.position.x, touch.position.y, 0f);
			if(touch.phase == SoccerTouchPhase.Began && legacyTexture.HitTest(touchPosition))
			{
				pressedInside = true;
			}
			else if((touch.phase == SoccerTouchPhase.Ended || touch.phase == SoccerTouchPhase.Canceled) && pressedInside)
			{
				if(touch.phase == SoccerTouchPhase.Ended && legacyTexture.HitTest(touchPosition))
					ToggleSfx();

				pressedInside = false;
			}
		}
	}

	private void ToggleSfx()
	{
		if(lastToggleFrame == Time.frameCount)
			return;

		lastToggleFrame = Time.frameCount;
		AudioManager.isSFXOn = !AudioManager.isSFXOn;
		AudioManager.Save ();
	}

	private void SyncTexture()
	{
		if(legacyTexture == null)
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);

		if(legacyTexture == null)
			return;

		if(Application.isPlaying)
			legacyTexture.texture = AudioManager.isSFXOn ? onTexture : offTexture;
		else if(legacyTexture.texture == null)
			legacyTexture.texture = onTexture != null ? onTexture : offTexture;
	}
}
