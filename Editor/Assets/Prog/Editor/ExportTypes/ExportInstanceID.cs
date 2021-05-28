using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct InstanceID
{
    public int instanceID;
    public string name;
    public string tag;
}

[System.Serializable]
public struct InstanceIDCollection
{
    public string sceneName;
    public List<InstanceID> Ids; 
}



public class ExportInstanceID 
{
    public static InstanceIDCollection Export(string aSceneName)
    {
        Transform[] transforms = GameObject.FindObjectsOfType<Transform>();
        InstanceIDCollection sceneIDCollection = new InstanceIDCollection();
        sceneIDCollection.Ids = new List<InstanceID>();
 
        foreach(Transform transform in transforms)
        {
            //Denna funktion tar in det objekt vi loopar igenom just nu, kan t.ex vara en "pointLight" från en GameObject.FindObjectsOfType<Light>();
            //out GameObject prefabParent == Parenten (prefab-parent) som denna tillhör! Alltså ska pointlights ligga som children i en tom Prefab! 
            //(Som med alla andra objekt!)
            if(transform.TryGetComponent(out Collider collider))
            {
                if (sceneIDCollection.Ids.Exists(e => e.instanceID == transform.GetInstanceID()))
                    continue;

                InstanceID id = new InstanceID();
                id.instanceID = transform.GetInstanceID();
                id.name = transform.name;
                id.tag = transform.tag;

                sceneIDCollection.Ids.Add(id);//transform.GetInstanceID());
            }   
            else if (Json.TryIsValidExport(transform, out GameObject prefabParent))
            {
                //Kollar bara om vi redan har lagt till denna id (Eftersom vi kollar Parent & Child objekt,
                //Men bara vill spara Parent Object ID:et
                if (sceneIDCollection.Ids.Exists(e => e.instanceID == prefabParent.transform.GetInstanceID()))
                    continue;

                InstanceID id = new InstanceID();
                id.instanceID = prefabParent.transform.GetInstanceID();
                id.name = prefabParent.transform.name;
                id.tag = prefabParent.transform.tag;
                sceneIDCollection.Ids.Add(id);

                if (prefabParent.name.Contains("BP_"))
                {
                    foreach(Transform childTransform in prefabParent.transform)
                    {
                        InstanceID childID = new InstanceID();
                        id.instanceID = childTransform.GetInstanceID();
                        id.name = childTransform.name;
                        id.tag = childTransform.tag;
                        sceneIDCollection.Ids.Add(childID);
                    }
                }
            }
        }

        sceneIDCollection.sceneName = aSceneName;
        return sceneIDCollection;
    }

    static bool SaveToExport(GameObject gameObject, ref InstanceIDCollection collection)
    {
        if (collection.Ids.Exists(e => e.instanceID == gameObject.transform.GetInstanceID()))
        {
            return false;
        }

        InstanceID id = new InstanceID();
        id.instanceID = gameObject.transform.GetInstanceID();
        id.name = gameObject.name;
        id.tag = gameObject.tag;
        collection.Ids.Add(id);
        return true;
    }

}
