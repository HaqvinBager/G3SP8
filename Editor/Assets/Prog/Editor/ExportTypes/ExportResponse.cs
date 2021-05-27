using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct ResponseData
{
    public string onResponseNotify;
    public int instanceID;
}

[System.Serializable]
public struct ResponseCollection
{
    public List<ResponseData> responses;
}

public class ExportResponse 
{  
    public static ResponseCollection Export()
    {
        ResponseCollection collection = new ResponseCollection();
        collection.responses = new List<ResponseData>();
        Response[] responses = GameObject.FindObjectsOfType<Response>();
        foreach(Response response in responses)
        {
            ResponseData data = new ResponseData();
            data.onResponseNotify = response.myLock.onLockNotify.name;
            data.instanceID = response.transform.GetInstanceID();
            collection.responses.Add(data);
        }
        return collection;
    }
}