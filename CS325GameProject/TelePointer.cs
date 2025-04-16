using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TelePointer : MonoBehaviour
{
    public GameObject telePoint;

    private void OnTriggerEnter2D(Collider2D collision){

        collision.gameObject.transform.position = telePoint.transform.position;
    }
}
