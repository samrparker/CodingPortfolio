using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LevelManager : MonoBehaviour
{
    public static LevelManager Instance;

    private static int day;

    private static int[] nightZombieNumbers;

    private void Awake()
    {
        //If the player comes back to the quiz scene, this object will be in the scene, and so should be deleted.
        if(Instance != null)
        {
            Destroy(gameObject);
            return;
        }

        //Sets the Instance variable to this object, the designates this object to not be deleted when another scene is loaded.
        Instance = this;
        day = 1;
        nightZombieNumbers = new int[] {10, 15, 25};
        DontDestroyOnLoad(gameObject);
    }

    public static int GetDay(){
        return day;
    }

    public void NextDay(){
        day++;
    }

    public static int GetZombieNum(){
        return nightZombieNumbers[day-1];
    }
}
