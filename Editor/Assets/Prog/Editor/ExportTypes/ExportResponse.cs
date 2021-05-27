using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct ResponseData
{
    public string onResponseNotify;
    public int instanceID;
}

[System.Serializable]
public struct MoveResponse
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct RotateResponse
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct ResponseCollection
{
    public List<ResponseData> responses;
    public List<MoveResponse> moves;
    public List<RotateResponse> rotates;
}

public class ExportResponse 
{  
    public static ResponseCollection Export()
    {
        ResponseCollection collection = new ResponseCollection();
        collection.responses = new List<ResponseData>();
        collection.moves = new List<MoveResponse>();
        collection.rotates = new List<RotateResponse>();

        Response[] responses = GameObject.FindObjectsOfType<Response>();
        foreach(Response response in responses)
        {
            ResponseData data = new ResponseData();
            data.onResponseNotify = response.myLock.onLockNotify.name;
            data.instanceID = response.transform.GetInstanceID();
            collection.responses.Add(data);

            ExportRotateResponses(collection, response);
            ExportMoveResponses(collection, response);
        }
        return collection;
    }

    private static void ExportMoveResponses(ResponseCollection collection, Response response)
    {
        if (response.TryGetComponent(out ResponseMove move))
        {
            MoveResponse moveData = new MoveResponse();
            moveData.start = move.start;
            moveData.end = move.end;
            moveData.duration = move.duration;
            moveData.instanceID = move.GetInstanceID();
            collection.moves.Add(moveData);
        }
    }

    private static void ExportRotateResponses(ResponseCollection collection, Response response)
    {
        if (response.TryGetComponent(out ResponseRotate rotate))
        {
            RotateResponse rotateData = new RotateResponse();
            rotateData.start = rotate.start;
            rotateData.end = rotate.end;
            rotateData.duration = rotate.duration;
            rotateData.instanceID = rotate.GetInstanceID();
            collection.rotates.Add(rotateData);
        }
    }
}