using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct KeyData
{
    public string onKeyCreateNotify;
    public string onKeyInteractNotify;
    public int hasLock;
    public int instanceID;
}

[System.Serializable]
public struct ActivationMoveData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct ActivationRotateData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public int instanceID;
}

[System.Serializable]
public struct ActivationPlayAudioData
{
    public int soundEffect;
    public bool is3D;
    public Vector3 coneDirection;
    public float minAttenuationAngle;
    public float maxAttenuationAngle;
    public float minimumVolume;
    public int instanceID;
}

[System.Serializable]
public struct ActivationTeleporterData
{
    public Vector3 teleportToPos;
    public Vector3 teleportToRot;
    public int teleporterName;
    public int teleporterTarget;
    public float timeUntilTeleport;
    public int instanceID;
}

[System.Serializable]
public struct KeyCollection
{
    public List<KeyData> keys;
    public List<ActivationMoveData> activationMoves;
    public List<ActivationRotateData> activationRotates;
    public List<ActivationPlayAudioData> activationAudios;
    public List<ActivationTeleporterData> activationTeleporters;
}

public class ExportKey
{
    public static KeyCollection Export()
    {
        KeyCollection collection = new KeyCollection();
        collection.keys = new List<KeyData>();
        collection.activationMoves = new List<ActivationMoveData>();
        collection.activationRotates = new List<ActivationRotateData>();
        collection.activationAudios = new List<ActivationPlayAudioData>();
        collection.activationTeleporters = new List<ActivationTeleporterData>();

        Key[] keys = GameObject.FindObjectsOfType<Key>();
        foreach (Key key in keys)
        {
            KeyData data = new KeyData();
            data.onKeyCreateNotify = key.myLock?.onKeyCreateNotify?.name;
            data.onKeyInteractNotify = key.myLock?.onKeyInteractNotify?.name;
            data.hasLock = key.myLock != null ? 1 : 0;
            data.instanceID = key.transform.GetInstanceID();
            collection.keys.Add(data);

            ExportRotateActivations(ref collection.activationRotates, key);
            ExportMoveActivations(ref collection.activationMoves, key);
            ExportPlayAudioActivations(ref collection.activationAudios, key);
            ExportTeleportActivations(ref collection.activationTeleporters, key);
        }
        return collection;
    }

    private static void ExportMoveActivations(ref List<ActivationMoveData> moves, Key key)
    {
        if (key.TryGetComponent(out ActivationMove move))
        {
            ActivationMoveData moveData = new ActivationMoveData();
            moveData.start = move.start;
            moveData.end = move.end;
            moveData.duration = move.duration;
            moveData.instanceID = move.transform.GetInstanceID();
            moves.Add(moveData);
        }
    }

    private static void ExportRotateActivations(ref List<ActivationRotateData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationRotate rotate))
        {
            ActivationRotateData rotateData = new ActivationRotateData();
            rotateData.start = rotate.start;
            rotateData.end = rotate.end;
            rotateData.duration = rotate.duration;
            rotateData.instanceID = rotate.transform.GetInstanceID();
            collection.Add(rotateData);
        }
    }

    private static void ExportPlayAudioActivations(ref List<ActivationPlayAudioData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationPlayAudio playAudio))
        {
            ActivationPlayAudioData playAudioData = new ActivationPlayAudioData();
            playAudioData.coneDirection = playAudio.myConeDirection;
            playAudioData.is3D = playAudio.myIs3D;
            playAudioData.minAttenuationAngle = playAudio.myMinAttenuationAngle;
            playAudioData.maxAttenuationAngle = playAudio.myMaxAttenuationAngle;
            playAudioData.minimumVolume = playAudio.myMinimumVolume;
            playAudioData.instanceID = playAudio.transform.GetInstanceID();
            playAudioData.soundEffect = (int)playAudio.soundEffect;         
            collection.Add(playAudioData);
        }
    }

    private static void ExportTeleportActivations(ref List<ActivationTeleporterData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationTeleporter teleporter))
        {
            ActivationTeleporterData teleporterData = new ActivationTeleporterData();
            teleporterData.teleporterName = (int)teleporter.myName;
            teleporterData.teleporterTarget = (int)teleporter.myTarget;
            teleporterData.timeUntilTeleport = teleporter.myTeleportTimer;
            teleporterData.teleportToPos = teleporter.onTeleportToMePosition;
            teleporterData.teleportToRot = teleporter.onTeleportToMeRotation;
            teleporterData.instanceID = teleporter.transform.GetInstanceID();
            collection.Add(teleporterData);
        }
    }
}

//[System.Serializable]
//public struct KeyData
//{
//    public string onCreateNotify;
//    public string onInteractNotify;
//    public int interactionType;
//    public int instanceID;
//}

//[System.Serializable]
//public struct KeyCollection
//{
//    public List<KeyData> keys;
//}

//public class ExportKey 
//{

//    public static KeyCollection Export()
//    {
//        KeyCollection collection = new KeyCollection();
//        collection.keys = new List<KeyData>();

//        Key[] keys = GameObject.FindObjectsOfType<Key>();
//        foreach(Key key in keys)
//        {
//            if (key.myLock == null)
//                continue;

//            KeyData data = new KeyData();
//            data.onCreateNotify = key.myLock.onKeyCreateNotify.name;
//            data.onInteractNotify = key.myLock.onKeyInteractNotify.name;
//            data.interactionType = (int)key.interactionType;
//            data.instanceID = key.transform.GetInstanceID();
//            collection.keys.Add(data);
//        }
//        return collection;
//    }

//}
