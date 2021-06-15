using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class PrefabProtector : MonoBehaviour
{
    //public bool smartAddCollider = false;

    private bool myHasCollider = false;

    private void Update()
    {
        if (transform.childCount > 0)
        {
            if (transform.GetChild(0).hasChanged)
            {
                transform.GetChild(0).localPosition = Vector3.zero;
                transform.GetChild(0).localRotation = Quaternion.identity;
                transform.GetChild(0).localScale = Vector3.one;
                transform.GetChild(0).hasChanged = false;
            }
        }

        if (TryGetComponent(out Collider collider))
        {
            if (collider.GetType() == typeof(MeshCollider))
            {
                MeshCollider meshCollider = collider as MeshCollider;
                meshCollider.sharedMesh = GetComponentInChildren<MeshFilter>().sharedMesh;
            }
            else if (collider.GetType() == typeof(BoxCollider))
            {
                //if (smartAddCollider)
                //{             
                bool hasCollider = GetComponent<BoxCollider>();
                if (myHasCollider != hasCollider)
                {
                    if (transform.childCount > 0)
                    {
                        if (transform.GetChild(0).TryGetComponent(out Renderer rend))
                        {
                            if (GetComponent<Rigidbody>() == null)
                            {
                                gameObject.AddComponent<Rigidbody>().isKinematic = true;
                            }

                            //BoxCollider childCollider = rend.gameObject.AddComponent<BoxCollider>();
                            //BoxCollider newCollider = GetComponent<BoxCollider>();
                            //newCollider.size = childCollider.size;
                            //if (newCollider.size.x <= 0.0f || newCollider.size.y <= 0.0f || newCollider.size.z <= 0.0f)
                            //{
                            //    Debug.LogWarning("Box Collider Size is == 0! This will cause a crash in IronWrought", newCollider);
                            //}
                            //newCollider.center = childCollider.center;
                            //DestroyImmediate(childCollider);
                        }
                    }
                }

                BoxCollider boxCollider = collider as BoxCollider;
                if (boxCollider.size.x <= 0.0f || boxCollider.size.y <= 0.0f || boxCollider.size.z <= 0.0f)
                {
                    Debug.LogWarning("Box Collider Size is == 0! This will cause a crash in IronWrought", boxCollider);
                }

                if (GetComponent<Rigidbody>() == null)
                {
                    if (GetComponent<Key>() == null)
                        gameObject.AddComponent<Rigidbody>();
                    else
                        collider.isTrigger = true;

                }
            }
        }
        myHasCollider = GetComponent<BoxCollider>();
    }
}
