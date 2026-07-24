using UnityEngine;
using System.Collections;

public class ColliderStopTheBall : MonoBehaviour
{
	void OnCollisionEnter(Collision collision)
	{
		foreach (ContactPoint contact in collision.contacts)
		{
			if(contact.otherCollider.tag == "TheSoccerBall")
			{
				contact.otherCollider.gameObject.GetComponent<Rigidbody>().velocity = Vector3.zero;
				contact.otherCollider.gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
				break;
			}
		}
	}
}
