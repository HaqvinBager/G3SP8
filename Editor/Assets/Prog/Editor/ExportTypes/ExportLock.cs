using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct LockData
{
    public string onNotifyName;
    public int onNotify;
    public int onKeyCreateNotify;
    public int onKeyInteractNotify;
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
            if(aLock.onLockNotify == null)
            {
                Debug.LogWarning("Missing a event type on lock, make sure that all event slots are filled", aLock);
                continue;
            }
            LockData data = new LockData();
            data.onNotifyName = aLock.onLockNotify.name;
            data.onNotify = aLock.onLockNotify.GetInstanceID();
            data.onKeyCreateNotify = aLock.onKeyCreateNotify.GetInstanceID();
            data.onKeyInteractNotify = aLock.onKeyInteractNotify.GetInstanceID();
            data.interactionType = (int)aLock.interactionType;
            data.instanceID = aLock.transform.GetInstanceID();
            collection.locks.Add(data);
        }
        return collection;
    }
}
