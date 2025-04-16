using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bullet : MonoBehaviour
{
    public bool fired;
    private float directionX;
    private float directionY;
    private Quaternion angle;

    private Rigidbody2D rigidBody;

    void Start(){
        rigidBody = this.GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        rigidBody.velocity = new Vector2(directionX * 21f, directionY * 21f);
    }

    public void Shoot(float speedX, float speedY, Quaternion angleToShoot){
        fired = true;

        SetSpeeds(speedX, speedY);

        transform.rotation = angleToShoot;
        angle = angleToShoot;
    }

    public void SetSpeeds(float speedX, float speedY){
        directionX = speedX;
        directionY = speedY;
        rigidBody.velocity = new Vector2(directionX * 35f, directionY * 35f);
    }

    public void Disable(){
        fired = false;
        directionX = 0;
        directionY = 0;
        transform.position = new Vector3(0, -20, 0);
    }

    public void SetPosition(Vector3 position){
        transform.position = position;
    }

    private void OnTriggerEnter2D(Collider2D collision){

        if(collision.gameObject.name == "HouseTileMap"){
            Disable();
        }
        if(collision.gameObject.name.Contains("Zombie")){
            Disable();
            if(collision.gameObject.GetComponent<ZombiePathFinding>().Die()){
                collision.gameObject.SetActive(false);
            }
        }
    }
}
