using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DrawSphere : MonoBehaviour
{
    [Range(0.1f, 5.0f)]
    public float radius = 1.0f;
    public Color color = Color.blue;

    private void OnDrawGizmos()
    {
        Gizmos.color = color * 0.70f;
        Gizmos.DrawSphere(transform.position, radius);
    }

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = color;
        Gizmos.DrawSphere(transform.position, radius);
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
