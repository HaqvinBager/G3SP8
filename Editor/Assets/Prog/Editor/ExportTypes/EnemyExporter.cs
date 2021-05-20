using System.Collections;
using System.Collections.Generic;
using UnityEngine;



[System.Serializable]
public struct EnemyCollection
{
    public List<Enemy> enemies;
}

public class EnemyExporter 
{
    public static EnemyCollection Export(string aSceneName)
    {

        EnemyCollection collection = new EnemyCollection();
        collection.enemies = new List<Enemy>();

        EnemySettings[] allEnemies = GameObject.FindObjectsOfType<EnemySettings>();
        foreach(EnemySettings enemy in allEnemies)
        {
            enemy.settings.instanceID = enemy.transform.GetInstanceID();
            List<InterestPoint> points = new List<InterestPoint>();
            for(int i = 0; i < enemy.settings.points.Count; ++i)
            {
                if (enemy.settings.points[i].GetComponent<PatrolPoint>())
                {
                    InterestPoint interest = new InterestPoint();
                    interest.interestValue = enemy.settings.points[i].GetComponent<PatrolPoint>().myInterestValue;
                    interest.transform = enemy.settings.points[i].transform;
                    points.Add(interest);
                }
            }
            enemy.settings.interestPoints = points;
            collection.enemies.Add(enemy.settings);
        }

        return collection;
    }
}
