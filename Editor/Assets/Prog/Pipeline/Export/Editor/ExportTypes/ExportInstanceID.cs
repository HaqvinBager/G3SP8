using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct InstanceID
{
    public int instanceID;
    public string name;
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

                sceneIDCollection.Ids.Add(id);//transform.GetInstanceID());

                //InstanceName instanceName = new InstanceName();
                //instanceName.id = transform.GetInstanceID();
                //instanceName.name = transform.name;
                //names.Add(instanceName);
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
                sceneIDCollection.Ids.Add(id);

                //InstanceName instanceName = new InstanceName();
                //instanceName.id = prefabParent.transform.GetInstanceID();
                //instanceName.name = prefabParent.name;
                //names.Add(instanceName);


                if (prefabParent.name.Contains("BP_"))
                {
                    foreach(Transform childTransform in prefabParent.transform)
                    {
                        InstanceID childID = new InstanceID();
                        id.instanceID = childTransform.GetInstanceID();
                        id.name = childTransform.name;
                        sceneIDCollection.Ids.Add(childID);

                        //sceneIDCollection.Ids.Add(childTransform.GetInstanceID());
                        //InstanceName childInstanceName = new InstanceName();
                        //instanceName.id = childTransform.GetInstanceID();
                        //instanceName.name = childTransform.name;
                        //names.Add(childInstanceName);
                    }
                }
            }
        }

        sceneIDCollection.sceneName = aSceneName;
        return sceneIDCollection;
    }
}
