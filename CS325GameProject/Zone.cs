using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Zone : MonoBehaviour
{
    public int ZoneID;

    public GameObject[] zonePoints;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerStay2D(Collider2D collision){

        if(collision.gameObject.name == "Player"){
            collision.gameObject.GetComponent<PlayerMovement>().currentZone = this;
            collision.gameObject.GetComponent<PlayerMovement>().zone = ZoneID;
        }
        else if(collision.gameObject.name.Contains("Zombie")){
            collision.gameObject.GetComponent<ZombiePathFinding>().currentZone = this;
            collision.gameObject.GetComponent<ZombiePathFinding>().zone = ZoneID;
        }
    }
}
