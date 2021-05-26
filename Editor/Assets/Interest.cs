using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Interest : MonoBehaviour
{
    [Range(0.0f, 2.5f)]
    public float interest = 0.25f;

    public Transform cube;

    private void Awake()
    {
        cube.GetComponent<Renderer>().material.color = Color.black;
        cube = transform.GetChild(0).transform;
    }

    private void Update()
    {
        cube.GetComponent<Renderer>().material.color = Color.Lerp(Color.red, Color.green, Mathf.InverseLerp(1.0f, -1.0f, interest));
        transform.localScale = new Vector3(interest, transform.localScale.y, transform.localScale.z);    
    }
    
    public void SetInterestValue(float aValue)
    {
        interest = aValue * 0.25f;
    }
}
