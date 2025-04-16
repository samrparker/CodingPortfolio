using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ZombieManager : MonoBehaviour
{
    public GameObject[] zombiesArray;

    private int zombiesToSpawn;
    public int zombiesLeft;

    private int spawnTimer;

    public GameObject[] spawnPoints;

    private int day;

    private int spawnPlace;
    private int spawnPointPlace;

    public GameObject dayTextField;
    public GameObject zombiesTextField;
    // Start is called before the first frame update
    void Start()
    {
        zombiesToSpawn = LevelManager.GetZombieNum();
        zombiesLeft = zombiesToSpawn;
        spawnTimer = 0;
        spawnPlace = 0;
        spawnPointPlace = 0;
        day = LevelManager.GetDay();
    }

    // Update is called once per frame
    void Update()
    {
        if(spawnTimer > 0){
            spawnTimer--;
        }
        else{
            spawnTimer = 1000;
            SpawnZombie();
        }

        dayTextField.GetComponent<TextMeshProUGUI>().text = "Day: " + day;
        zombiesTextField.GetComponent<TextMeshProUGUI>().text = "Zombies Left: " + zombiesLeft;
    }

    private void SpawnZombie(){
        if(zombiesToSpawn > spawnPlace){
            zombiesArray[spawnPlace].SetActive(true);
            zombiesArray[spawnPlace].transform.position = spawnPoints[spawnPointPlace].transform.position;
            spawnPlace++;
            switch(spawnPointPlace){
                case 0:
                    spawnPointPlace = 3;
                    break;
                case 1:
                    spawnPointPlace = 0;
                    break;
                case 2:
                    spawnPointPlace = 1;
                    break;
                case 3:
                    spawnPointPlace = 2;
                    break;

            }
        }
        else {
            Debug.Log("END");
        }
    }
}
