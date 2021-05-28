using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct LockData
{
    public string onNotify;
    public string onKeyCreateNotify;
    public string onKeyInteractNotify;
    public int interactionType;
    public int instanceID;
}

[System.Serializable]
public struct LockCollection
{
    public List<LockData> locks;
}

public class ExportLock 
{
    public static LockCollection Export()
    {
        LockCollection collection = new LockCollection();
        collection.locks = new List<LockData>();

        Lock[] locks = GameObject.FindObjectsOfType<Lock>();
        foreach(Lock aLock in locks)
        {
            LockData data = new LockData();
            data.onNotify = aLock.onLockNotify.name;
            data.onKeyCreateNotify = aLock.onKeyCreateNotify?.name;
            data.onKeyInteractNotify = aLock.onKeyInteractNotify?.name;
            data.interactionType = (int)aLock.interactionType;
            data.instanceID = aLock.transform.GetInstanceID();
            collection.locks.Add(data);
        }
        return collection;
    }
}
