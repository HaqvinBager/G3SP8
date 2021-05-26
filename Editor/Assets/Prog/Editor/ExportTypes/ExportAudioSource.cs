using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct AudioSourceLink
{
    public int instanceID;
    public int soundIndex;
    public bool is3D;
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

        IronAudioSource[] allAudioSources = GameObject.FindObjectsOfType<IronAudioSource>();
        foreach (IronAudioSource source in allAudioSources)
        {
            AudioSourceLink audioSourceLink;
            audioSourceLink.instanceID = source.transform.GetInstanceID();
            audioSourceLink.soundIndex = source.mySoundIndex;
            audioSourceLink.is3D = source.myIs3D;
            collection.myAudioSources.Add(audioSourceLink);
        }

        return collection;
    }
}
