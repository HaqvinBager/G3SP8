using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[Serializable]
public struct SpotLight
{
    public int instanceID;
    public float range;
    public Vector3 color;
    public float intensity;
    public float innerSpotAngle;
    public float outerSpotAngle;
    public int isVolumetric;
}

[Serializable]
public struct SpotLightCollection
{
    public List<SpotLight> spotLights;
}

public class ExportSpotLight 
{
    public static SpotLightCollection Export()
    {
        Debug.Log("Exporting SpotLights");
        Light[] allLights = GameObject.FindObjectsOfType<Light>();
        SpotLightCollection lightCollection = new SpotLightCollection();
        lightCollection.spotLights = new List<SpotLight>();

        foreach(Light light in allLights)
        {
            SpotLight spotLight = new SpotLight();

            if(light.type != LightType.Spot)
                continue;

            if (light.transform.parent == null)
                spotLight.instanceID = light.transform.GetInstanceID();
            else
                spotLight.instanceID = light.transform.parent.GetInstanceID();

            spotLight.range = light.range;
            spotLight.color = new Vector3(light.color.r, light.color.g, light.color.b);
            spotLight.intensity = light.intensity;
            spotLight.innerSpotAngle = light.innerSpotAngle;
            spotLight.outerSpotAngle = light.spotAngle;

            if(light.TryGetComponent(out VolumetricLightData aData))
            {
                spotLight.isVolumetric = aData.isVolumetric ? 1 : 0;
            }
            else
            {
                spotLight.isVolumetric = 0;
            }

            lightCollection.spotLights.Add(spotLight);
        }
        return lightCollection;
    }
}
