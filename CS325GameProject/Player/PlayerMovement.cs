using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerMovement : MonoBehaviour
{
    public GameObject camera;

    private Camera cam;

    private Transform cameraTransform;

    private Rigidbody2D rigidBody;

    private Transform trans;
    private Vector3 previousAngle;

    private PlayerGun playerGun;

    public int zone;
    public Zone currentZone;
    
    //public AudioSource music;

    // Start is called before the first frame update
    void Start()
    {
        Cursor.visible = false;
        Cursor.lockState = CursorLockMode.Confined;
        cameraTransform = camera.GetComponent<Transform>();

        cam = Camera.main;

        rigidBody = this.GetComponent<Rigidbody2D>();

        trans = this.GetComponent<Transform>();
        previousAngle = new Vector3(0, 0, 0);

        playerGun = this.GetComponent<PlayerGun>();
    }

    // Update is called once per frame
    void Update()
    {
        
        Vector3 relative = trans.InverseTransformPoint(cam.ScreenToWorldPoint(new Vector3(Input.mousePosition.x, Input.mousePosition.y, 0)));
        relative.z = 0;
        float angle = Mathf.Atan2(relative.x, relative.y) * Mathf.Rad2Deg;
        
        Quaternion target = Quaternion.Euler(0, 0, -angle+previousAngle.z);
        if(angle != 0){

            transform.rotation = target;
            previousAngle.z = (-angle + previousAngle.z) % 360;
        }

        float directionX = Input.GetAxisRaw("Horizontal");
        float directionY = Input.GetAxisRaw("Vertical");

        rigidBody.velocity = new Vector2(directionX * 7f, directionY * 7f);

        playerGun.UpdateReticle();
        playerGun.SetRelative(relative);

        if(Input.GetAxisRaw("Fire1") == 1){
            float sinRad = (angle+previousAngle.z + 90) * Mathf.Deg2Rad;
            float cosRad = (angle+previousAngle.z + 90) * Mathf.Deg2Rad;

            playerGun.Fire(Mathf.Cos(cosRad), Mathf.Sin(sinRad), target);
        }
        Vector3 cameraPosition = trans.position;
        cameraPosition.z = -10;
        cameraTransform.position = cameraPosition;
    }
}
