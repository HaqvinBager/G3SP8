using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class PatrolPoint : MonoBehaviour
{
    public float myInterestValue;
    public float myBonusInterestValue;
    public float myValue = 0.0f;
    public UnityEvent<float> onInterestValueChanged = new UnityEvent<float>();

    private void Awake()
    {
        if(transform.childCount > 0)
        {
            transform.GetChild(0).gameObject.SetActive(true);
        }

        var collider = gameObject.AddComponent<SphereCollider>();
        collider.radius = 1.17f;
    }

    private void OnDestroy()
    {
        if (transform.childCount > 0)
        {
            transform.GetChild(0).gameObject.SetActive(false);
        }
    }


    private void Update()
    {
        myBonusInterestValue =  Mathf.Max(-15.0f, myBonusInterestValue - Time.deltaTime);
        myValue = GetInterestValue();
        onInterestValueChanged.Invoke(myBonusInterestValue);
    }

    internal void AddBonusValue(int v)
    {
        myBonusInterestValue = Mathf.Min(15.0f, myBonusInterestValue + v);
    }

    internal float GetInterestValue()
    {
        return myInterestValue + myBonusInterestValue;
    }
}
