using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct KeyData
{
    public string onCreateNotify;
    public string onInteractNotify;
    public int interactionType;
    public int instanceID;
}

[System.Serializable]
public struct KeyCollection
{
    public List<KeyData> keys;
}

public class ExportKey 
{

    public static KeyCollection Export()
    {
        KeyCollection collection = new KeyCollection();
        collection.keys = new List<KeyData>();

        Key[] keys = GameObject.FindObjectsOfType<Key>();
        foreach(Key key in keys)
        {
            if (key.myLock == null)
                continue;

            KeyData data = new KeyData();
            data.onCreateNotify = key.myLock.onKeyCreateNotify.name;
            data.onInteractNotify = key.myLock.onKeyInteractNotify.name;
            data.interactionType = (int)key.interactionType;
            data.instanceID = key.transform.GetInstanceID();
            collection.keys.Add(data);
        }
        return collection;
    }

}
