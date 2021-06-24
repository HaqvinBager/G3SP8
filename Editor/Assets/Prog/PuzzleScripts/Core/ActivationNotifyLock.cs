using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ActivationNotifyLock : MonoBehaviour, IKey
{
    public Lock aLockToNotify = null;

    void IKey.Remove()
    {
        DestroyImmediate(this);
    }
}
