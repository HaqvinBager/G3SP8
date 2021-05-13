using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class CustomGeneratorController : MonoBehaviour
{
    public int requiredCount = 3;
    private int currentCount = 0;

    public UnityEvent onCountComplete = new UnityEvent();

    public void RegisterAdd()
    {
        currentCount++;
        if(currentCount >= requiredCount)
        {
            onCountComplete?.Invoke();
        }
    }
}
