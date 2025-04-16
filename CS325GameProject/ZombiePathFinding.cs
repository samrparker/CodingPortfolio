using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class ZombiePathFinding : MonoBehaviour
{
    public GameObject target;
    private PlayerMovement playerMove;

    public Zone currentZone;
    public int zone;

    private Vector3 previousAngle;

    private Rigidbody2D rigidBody;

    private int health;

    public GameObject zombieManager;
    private ZombieManager zManager;
    // Start is called before the first frame update
    void Start()
    {
        playerMove = target.GetComponent<PlayerMovement>();
        previousAngle = new Vector3(0, 0, 0);
        rigidBody = GetComponent<Rigidbody2D>();
        health = 3;
        zManager = zombieManager.GetComponent<ZombieManager>();
    }

    // Update is called once per frame
    void Update()
    {
        GameObject trueTarget = target;
        Vector3 playerPosition = target.transform.position;
        Vector3 myPosition = transform.position;

        if(playerMove.zone == zone){
            trueTarget = target;
        }
        else {
            if(currentZone != null){

                double minDistance = double.MaxValue;
                GameObject minPoint = null;
                
                for(int z = 0; z < currentZone.zonePoints.Length; z++){
                    GameObject zonePoint = currentZone.zonePoints[z];

                    double distance = GetDistance(playerPosition, zonePoint.transform.position);
                    //distance += GetDistance(myPosition, zonePoint.transform.position);

                    if((minPoint == null || distance < minDistance)){
                        minDistance = distance;
                        minPoint = zonePoint;
                    }
                }
                trueTarget = minPoint;
                //Debug.Log(minPoint.name);
            }
        }

        Vector3 relative = transform.InverseTransformPoint(trueTarget.transform.position);
        relative.z = 0;
        float angle = Mathf.Atan2(relative.x, relative.y) * Mathf.Rad2Deg;
                
        Quaternion targetAngle = Quaternion.Euler(0, 0, -angle+previousAngle.z);
        if(angle != 0){

            transform.rotation = targetAngle;
            previousAngle.z = (-angle + previousAngle.z) % 360;
        }

        float sinRad = (angle+previousAngle.z + 90) * Mathf.Deg2Rad;
        float cosRad = (angle+previousAngle.z + 90) * Mathf.Deg2Rad;

        rigidBody.velocity = new Vector2(Mathf.Cos(cosRad) * 5f, Mathf.Sin(sinRad) * 5f);
    }

    private double GetDistance(Vector3 targetVector, Vector3 myVector){
        return (double) Math.Sqrt( Math.Pow( ((double)targetVector.x-myVector.x), 2) + Math.Pow(((double) targetVector.y-myVector.y), 2));
    }

    public bool Die(){
        if(health > 0){
            health--;
            return false;
        }
        else {
            zManager.zombiesLeft--;
            return true;
        }
    }
}
