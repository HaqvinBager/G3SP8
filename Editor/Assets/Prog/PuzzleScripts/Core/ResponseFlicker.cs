using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Light))]
public class ResponseFlicker : MonoBehaviour, IListener
{
    public float mySpeed = 1.0f;
    public Vector2 myMinMaxIntensity = new Vector2(0.25f, 10.0f);

    private SDefaultSettings mySettings 
    { 
        get => new SDefaultSettings {
            myRange = 0.57f,
            mySpotAngles = new Vector2(16.768f, 55.371f),
            myColor = new Color(255.0f / 255.0f, 231.0f / 255.0f, 198.0f / 255.0f, 255.0f / 255.0f),
            myIntensity = 1.10f,
            myLightType = LightType.Spot
        };
    }

    private void OnValidate()
    {
        if (TryGetComponent(out Light light))
        {
            SDefaultSettings settings = mySettings;
            light.range = settings.myRange;
            light.spotAngle = settings.mySpotAngles.y;
            light.innerSpotAngle = settings.mySpotAngles.x;
            light.color = settings.myColor;
            light.type = settings.myLightType;
        }
    }


    public void Remove()
    {
#if UNITY_EDITOR
        DestroyImmediate(this);
#endif
    }
}

public struct SDefaultSettings //This is just here to make life a little easier when setting up your Flickering Light!
{
    public float myRange;
    public Vector2 mySpotAngles;
    public Color myColor;
    public float myIntensity;
    public LightType myLightType;
}



