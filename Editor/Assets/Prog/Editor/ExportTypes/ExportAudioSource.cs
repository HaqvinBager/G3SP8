using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct AudioSourceLink
{
    public int instanceID;
    public int soundIndex;
}

[System.Serializable]
public struct AudioSourceCollection
{
    public List<AudioSourceLink> myAudioSources;
}

public class ExportAudioSource
{
    public static AudioSourceCollection Export(string aSceneName)
    {
        AudioSourceCollection collection = new AudioSourceCollection();
        collection.myAudioSources = new List<AudioSourceLink>();

        StaticAudioSource[] allAudioSources = GameObject.FindObjectsOfType<StaticAudioSource>();
        foreach (StaticAudioSource source in allAudioSources)
        {
            AudioSourceLink audioSourceLink;
            audioSourceLink.instanceID = source.transform.GetInstanceID();
            audioSourceLink.soundIndex = source.mySoundIndex;
            collection.myAudioSources.Add(audioSourceLink);
        }

        return collection;
    }
}
