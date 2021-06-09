using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[System.Serializable]
public struct ListenerData
{
    public int onResponseNotify;
    public int instanceID;
}

[System.Serializable]
public struct ResponseMoveData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct ResponseRotateData
{
    public Vector3 start;
    public Vector3 end;
    public float duration;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct ResponsePrintData
{
    public string data;
    public int instanceID;
}

[System.Serializable]
public struct ResponseToggleData
{
    public string type;
    public int enableOnStartup;
    public int enableOnNotify;
    public int target;
    public int instanceID;
}

[System.Serializable]
public struct ResponsePlayAudioData
{
    public int soundEffect;
    public bool is3D;
    public Vector3 coneDirection;
    public float minAttenuationAngle;
    public float maxAttenuationAngle;
    public float minAttenuationDistance;
    public float maxAttenuationDistance;
    public float minimumVolume;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct ResponsePlayVoiceData
{
    public int voiceLine;
    public bool is3D;
    public Vector3 coneDirection;
    public float minAttenuationAngle;
    public float maxAttenuationAngle;
    public float minAttenuationDistance;
    public float maxAttenuationDistance;
    public float minimumVolume;
    public int instanceID;
}

[System.Serializable]
public struct ListenerCollection
{
    public List<ListenerData> listeners;
    public List<ResponseMoveData> responseMoves;
    public List<ResponseRotateData> responseRotates;
    public List<ResponsePrintData> responsePrints;
    public List<ResponseToggleData> responseToggles;
    public List<ResponsePlayAudioData> responseAudios;
    public List<ResponsePlayVoiceData> responseVoices;
    public List<ResponseTeleporterData> responseTeleporter;
}

[System.Serializable]
public struct ResponseTeleporterData
{
    public Vector3 teleportToPos;
    public Vector3 teleportToRot;
    public int teleporterName;
    public int teleporterTarget;
    public float timeUntilTeleport;
    public int instanceID;
}

public class ExportListener
{
    public static ListenerCollection Export()
    {
        ListenerCollection collection = new ListenerCollection();
        collection.listeners = new List<ListenerData>();
        collection.responseMoves = new List<ResponseMoveData>();
        collection.responseRotates = new List<ResponseRotateData>();
        collection.responsePrints = new List<ResponsePrintData>();
        collection.responseToggles = new List<ResponseToggleData>();
        collection.responseAudios = new List<ResponsePlayAudioData>();
        collection.responseVoices = new List<ResponsePlayVoiceData>();
        collection.responseTeleporter = new List<ResponseTeleporterData>();

        Listener[] listeners = GameObject.FindObjectsOfType<Listener>();
        foreach (Listener listener in listeners)
        {
            if(listener.myLock == null)
            {
                Debug.LogWarning("This Listener does not have a Lock. Please add one if you want to see the Response Behaviour play out. <3 /Axel Mcfluffykins", listener.gameObject);
                continue;
            }

            if(listener.myLock.onLockNotify == null)
            {
                Debug.LogWarning("This lock does not have a onLockNotify, this response will never trigger. Please add an onLockNotify to the lock to be able to trigger the event", listener.myLock);
                continue;
            }

            ListenerData data = new ListenerData();
            data.onResponseNotify = listener.myLock.onLockNotify.GetInstanceID();
            data.instanceID = listener.transform.GetInstanceID();
            collection.listeners.Add(data);

            ExportRotateResponses(ref collection.responseRotates, listener);
            ExportMoveResponses(ref collection.responseMoves, listener);
            ExportPrintResponses(ref collection.responsePrints, listener);
            ExportToggleResponses(ref collection.responseToggles, listener);
            ExportPlayAudioResponses(ref collection.responseAudios, listener);
            ExportPlayVoiceResponses(ref collection.responseVoices, listener);
            ExportTeleportResponses(ref collection.responseTeleporter, listener);
        }
        return collection;
    }

    private static void ExportToggleResponses(ref List<ResponseToggleData> toggles, Listener listener)
    {
        if (listener.TryGetComponent(out ResponseToggle obj))
        {
            ResponseToggleData data = new ResponseToggleData();
            data.instanceID = obj.transform.GetInstanceID();
            if (obj.aTargetType.GetType() == typeof(Light))
            {
                Light light = obj.aTargetType as Light;
                data.type = light.type.ToString();
            }
            else
                data.type = obj.aTargetType.GetType().Name;
            
            if (obj.aTarget == null)
                Debug.Log("Missing Reference", obj);
            data.target = obj.aTarget.transform.GetInstanceID();
            data.enableOnStartup = obj.enabledOnStart ? 1 : 0;
            data.enableOnNotify = obj.enableOnNotify ? 1 : 0;
            toggles.Add(data);
        }
    }

    private static void ExportPrintResponses(ref List<ResponsePrintData> prints, Listener listener)
    {
        if (listener.TryGetComponent(out ResponsePrint obj))
        {
            ResponsePrintData data = new ResponsePrintData();
            data.instanceID = obj.transform.GetInstanceID();
            data.data = obj.message;
            prints.Add(data);
        }
    }

    private static void ExportMoveResponses(ref List<ResponseMoveData> moves, Listener listener)
    {
        if (listener.TryGetComponent(out ResponseMove move))
        {
            ResponseMoveData moveData = new ResponseMoveData();
            moveData.start = move.start;
            moveData.end = move.end;
            moveData.duration = move.duration;
            moveData.delay = move.delay;
            moveData.instanceID = move.transform.GetInstanceID();
            moves.Add(moveData);
        }
    }

    private static void ExportRotateResponses(ref List<ResponseRotateData> collection, Listener Response)
    {
        if (Response.TryGetComponent(out ResponseRotate rotate))
        {
            ResponseRotateData rotateData = new ResponseRotateData();
            rotateData.start = rotate.start;
            rotateData.end = rotate.end;
            rotateData.duration = rotate.duration;
            rotateData.delay = rotate.delay;
            rotateData.instanceID = rotate.transform.GetInstanceID();
            collection.Add(rotateData);
        }
    }

    private static void ExportPlayAudioResponses(ref List<ResponsePlayAudioData> collection, Listener Response)
    {
        if (Response.TryGetComponent(out ResponsePlayAudio playAudio))
        {
            ResponsePlayAudioData playAudioData = new ResponsePlayAudioData();
            playAudioData.coneDirection = playAudio.myConeDirection;
            playAudioData.is3D = playAudio.myIs3D;
            playAudioData.minAttenuationAngle = playAudio.myMinAttenuationAngle;
            playAudioData.maxAttenuationAngle = playAudio.myMaxAttenuationAngle;
            playAudioData.minAttenuationDistance = playAudio.myMinAttenuationDistance;
            playAudioData.maxAttenuationDistance = playAudio.myMaxAttenuationDistance;
            playAudioData.minimumVolume = playAudio.myMinimumVolume;
            playAudioData.instanceID = playAudio.transform.GetInstanceID();
            playAudioData.soundEffect = (int)playAudio.soundEffect;
            playAudioData.delay = playAudio.myDelay;
            // Debug.Log("Audio: " + playAudioData.soundEffect.ToString());
            collection.Add(playAudioData);
        }
    }

    private static void ExportPlayVoiceResponses(ref List<ResponsePlayVoiceData> collection, Listener Response)
    {
        if (Response.TryGetComponent(out ResponsePlayVoice playVoice))
        {
            ResponsePlayVoiceData playVoiceData = new ResponsePlayVoiceData();
            playVoiceData.coneDirection = playVoice.myConeDirection;
            playVoiceData.is3D = playVoice.myIs3D;
            playVoiceData.minAttenuationAngle = playVoice.myMinAttenuationAngle;
            playVoiceData.maxAttenuationAngle = playVoice.myMaxAttenuationAngle;
            playVoiceData.minAttenuationDistance = playVoice.myMinAttenuationDistance;
            playVoiceData.maxAttenuationDistance = playVoice.myMaxAttenuationDistance;
            playVoiceData.minimumVolume = playVoice.myMinimumVolume;
            playVoiceData.instanceID = playVoice.transform.GetInstanceID();
            playVoiceData.voiceLine = (int)playVoice.voiceLine;
            // Debug.Log("Audio: " + playAudioData.soundEffect.ToString());
            collection.Add(playVoiceData);
        }
    }
    private static void ExportTeleportResponses(ref List<ResponseTeleporterData> collection, Listener Response)
    {
        if (Response.TryGetComponent(out ActivationTeleporter teleporter))
        {
            ResponseTeleporterData teleporterData = new ResponseTeleporterData();
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