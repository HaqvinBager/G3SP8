using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct KeyData
{
    public string onKeyCreateNotifyName;
    public string onKeyInteractNotifyName;
    public int onKeyCreateNotify;
    public int onKeyInteractNotify;
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
    public float minAttenuationDistance;
    public float maxAttenuationDistance;
    public float minimumVolume;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct ActivationPlayVoiceData
{
    public int voiceLine; //sceneIndex
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
public struct ActivationNextLevelData
{
    public string target;
    public float delay;
    public int instanceID;
}

[System.Serializable]
public struct ActivationLightData
{
    public Vector3 color;
    public float setIntensity;
    public int targetInstanceID;
    public string targetType;
    public int instanceID;
}

[System.Serializable]
public struct ActivationPlayVFXData
{
    public float duration;
    public int instanceID;
}


[System.Serializable]
public struct KeyCollection
{
    public List<KeyData> keys;
    public List<ActivationMoveData> activationMoves;
    public List<ActivationRotateData> activationRotates;
    public List<ActivationPlayAudioData> activationAudios;
    public List<ActivationPlayVoiceData> activationVoices;
    public List<ActivationNextLevelData> activationNextLevel;
    public List<ActivationLightData> activationLights;
    public List<ActivationPlayVFXData> activationPlayVFXes;
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
        collection.activationVoices = new List<ActivationPlayVoiceData>();
        collection.activationNextLevel = new List<ActivationNextLevelData>();
        collection.activationLights = new List<ActivationLightData>();
        collection.activationPlayVFXes = new List<ActivationPlayVFXData>();

        Key[] keys = GameObject.FindObjectsOfType<Key>();
        foreach (Key key in keys)
        {
            KeyData data = new KeyData();
            data.hasLock = key.myLock != null ? 1 : 0;

            if (data.hasLock == 1)
            {
                data.onKeyCreateNotifyName = key.myLock.onKeyCreateNotify.name;
                data.onKeyInteractNotifyName = key.myLock.onKeyInteractNotify.name;
                data.onKeyCreateNotify = key.myLock.onKeyCreateNotify.GetInstanceID();
                data.onKeyInteractNotify = key.myLock.onKeyInteractNotify.GetInstanceID();
            }

            data.instanceID = key.transform.GetInstanceID();
            collection.keys.Add(data);

            ExportRotateActivations(ref collection.activationRotates, key);
            ExportMoveActivations(ref collection.activationMoves, key);
            ExportPlayAudioActivations(ref collection.activationAudios, key);
            ExportPlayVoiceActivations(ref collection.activationVoices, key);
            ExportNextLevelActivations(ref collection.activationNextLevel, key);
            ExportLightActivations(ref collection.activationLights, key);
            ExportPlayVFXActivations(ref collection.activationPlayVFXes, key);
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
        //if (key.TryGetComponent(out ActivationPlayAudio playAudio))
        foreach (ActivationPlayAudio playAudio in key.GetComponents<ActivationPlayAudio>())
        {
            ActivationPlayAudioData playAudioData = new ActivationPlayAudioData();
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
            collection.Add(playAudioData);
        }
    }

    private static void ExportPlayVoiceActivations(ref List<ActivationPlayVoiceData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationPlayVoice playVoice))
        {
            ActivationPlayVoiceData playVoiceData = new ActivationPlayVoiceData();
            playVoiceData.coneDirection = playVoice.myConeDirection;
            playVoiceData.is3D = playVoice.myIs3D;
            playVoiceData.minAttenuationAngle = playVoice.myMinAttenuationAngle;
            playVoiceData.maxAttenuationAngle = playVoice.myMaxAttenuationAngle;
            playVoiceData.minAttenuationDistance = playVoice.myMinAttenuationDistance;
            playVoiceData.maxAttenuationDistance = playVoice.myMaxAttenuationDistance;
            playVoiceData.minimumVolume = playVoice.myMinimumVolume;
            playVoiceData.instanceID = playVoice.transform.GetInstanceID();
            playVoiceData.voiceLine = (int)playVoice.dialogueScene;
            collection.Add(playVoiceData);
        }
    }

    private static void ExportNextLevelActivations(ref List<ActivationNextLevelData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationNextLevel nextLevel))
        {
            ActivationNextLevelData nextLevelData = new ActivationNextLevelData();
            nextLevelData.target = nextLevel.myTarget.name.Substring(0, nextLevel.myTarget.name.LastIndexOf("_Scenes"));
            nextLevelData.delay = nextLevel.myDelay;
            nextLevelData.instanceID = nextLevel.transform.GetInstanceID();
            collection.Add(nextLevelData);
        }
    }

    private static void ExportLightActivations(ref List<ActivationLightData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationLight behavior))
        {
            if(behavior.IsNullPrintWarning(behavior.target, "Fix this by adding a Target =) Hälsningar, Axel Savage"))
                return;
            if (behavior.IsNullPrintWarning(behavior.targetLight, "Fix this by adding a Target Light =) Hälsningar, Axel Savage"))
                return;

            ActivationLightData data = new ActivationLightData();
            data.targetInstanceID = behavior.target.GetInstanceID();
            data.color = new Vector3(behavior.setColor.r, behavior.setColor.g, behavior.setColor.b);
            data.targetType = behavior.targetLight.type.ToString();
            data.setIntensity = behavior.setIntensity;
            data.instanceID = behavior.transform.GetInstanceID();
            collection.Add(data);
        }
    }

    private static void ExportPlayVFXActivations(ref List<ActivationPlayVFXData> collection, Key key)
    {
        if (key.TryGetComponent(out ActivationPlayVFX playVFX))
        {
            ActivationPlayVFXData playVFXData = new ActivationPlayVFXData();
            playVFXData.duration = playVFX.duration;
            playVFXData.instanceID = playVFX.transform.GetInstanceID();
            collection.Add(playVFXData);
        }
    }
}