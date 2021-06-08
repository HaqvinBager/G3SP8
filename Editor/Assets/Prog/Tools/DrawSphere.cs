using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class DrawSphere : MonoBehaviour
{
    [Range(0.1f, 5.0f)]
    public float radius = 1.0f;
    public Color color = Color.blue;

    private void OnDrawGizmos()
    {
        Gizmos.color = color * 0.40f;
        Gizmos.DrawSphere(transform.position, radius);
        Handles.Label(transform.position, gameObject.name);
    }

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = color * 0.70f;
        Gizmos.DrawSphere(transform.position, radius);
        Handles.Label(transform.position, gameObject.name);
    }

    public void SetColorGreen()
    {
        SetColor(Color.green);
    }

    public void SetColorRed()
    {
        SetColor(Color.red);
    }

    public void SetColor(Color aColor)
    {
        color = aColor;
    }
}
