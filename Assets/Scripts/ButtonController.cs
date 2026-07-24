using UnityEngine;
using System.Collections;

/*
 * This Class can be used with GUITexture Component to simulate a standard button.
 * Basic button functionality can be controlled from the Editor window so you dont
 * need to code at all.
*/

[ExecuteAlways]
[RequireComponent (typeof (GUITexture))]
public class ButtonController : MonoBehaviour
{
	public Texture hoverTexture;

	[HideInInspector]
	public Texture normalTexture;
	private GUITexture legacyTexture;

	void OnEnable()
	{
		EnsureTextureAssigned();
	}

	void OnValidate()
	{
		EnsureTextureAssigned();
	}
	
	void OnMouseEnter()
	{
//		if(hoverTexture != null)
//			guiTexture.texture = hoverTexture;
	}

	void OnMouseExit() 
	{
		if(hoverTexture != null)
			GetLegacyTexture().texture = normalTexture;
	}

	void OnMouseDown()
	{
		if(hoverTexture != null)
			GetLegacyTexture().texture = hoverTexture;
	}
	
	void OnMouseUp()
	{
		if(hoverTexture != null)
			GetLegacyTexture().texture = normalTexture;
	}

	// Update is called once per frame
	void Start ()
	{
		legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		EnsureTextureAssigned();
//		Rect rect = guiTexture.pixelInset;
//		rect.x = -GetValue (rect.width)/2;
//		rect.y = -GetValue (rect.height) / 2;
//		rect.width = GetValue (rect.width);
//		rect.height = GetValue (rect.height);
//		guiTexture.pixelInset = rect;

		//transform.position = new Vector3 (transform.position.x * Screen.width / 960f,transform.position.y,transform.position.z);
	}

//	float GetValue(float value)
//	{
//		return value * Screen.width / 960f;
//	}

	private GUITexture GetLegacyTexture()
	{
		if (legacyTexture == null)
		{
			legacyTexture = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
		}

		return legacyTexture;
	}

	private void EnsureTextureAssigned()
	{
		GUITexture guiTexture = GetLegacyTexture();
		if (guiTexture == null)
			return;

		if (normalTexture == null)
			normalTexture = guiTexture.texture != null ? guiTexture.texture : hoverTexture;

		if (guiTexture.texture == null)
			guiTexture.texture = normalTexture;
	}
}
