using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerGun : MonoBehaviour
{
    public GameObject reticle;
    private Transform reticleTransform;
    private Rigidbody2D reticleBody;

    private Vector3 relativeMousePosition;

    public GameObject[] bulletArray;

    private Bullet[] bullets;

    private int shootCooldown;

    public GameObject gunNoise;

    // Start is called before the first frame update
    void Start()
    {
        reticleTransform = reticle.GetComponent<Transform>();
        reticleBody = reticle.GetComponent<Rigidbody2D>();
        bullets = new Bullet[bulletArray.Length];
        for(int z = 0; z < bulletArray.Length; z++){
            bullets[z] = bulletArray[z].GetComponent<Bullet>();
        }
        shootCooldown = 0;
    }

    void Update(){
        if(shootCooldown > 0){
            shootCooldown--;
        }
    }

    public void UpdateReticle(){
        Vector3 newPosition = Camera.main.ScreenToWorldPoint(new Vector3(Input.mousePosition.x, Input.mousePosition.y, 0));
        newPosition.z = 0;
        reticleTransform.position = newPosition;
    }

    public void SetRelative(Vector3 relative){
        relativeMousePosition = relative;
    }

    public void Fire(float xVelocity, float yVelocity, Quaternion angle){
        
        if(shootCooldown == 0){
            shootCooldown++;
            for(int z = 0; z < bullets.Length; z++){

                if(!bullets[z].fired){
                    
                    AudioSource gunSource = gunNoise.GetComponent<AudioSource>();
                    gunSource.Play();
                    shootCooldown = 300;
                    Vector3 bulletPosition = transform.position;
                    bullets[z].SetPosition(bulletPosition);

                    bullets[z].Shoot(xVelocity, yVelocity, angle);
                    break;
                }
            }
        }
        else {
            shootCooldown--;
        }
        
    }
}
