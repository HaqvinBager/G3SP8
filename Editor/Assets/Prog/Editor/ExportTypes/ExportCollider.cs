﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

[System.Serializable]
public struct ColliderLink
{
    public int instanceID;
    public Vector3 positionOffest;
    public int colliderType;
    public Vector3 boxSize;
    public float sphereRadius;
    public float capsuleHeight;
    public float capsuleRadius;
    public bool isStatic;
    public bool isKinematic;
    public bool isTrigger;
    public float mass;
    public Vector3 localMassPosition;
    public Vector3 inertiaTensor;
    public float dynamicFriction;
    public float staticFriction;
    public float bounciness;
    public int layer;
}
// 1 - BoxCollider
// 2 - SphereCollider
// 3 - CapsuleCollider
// 4 - MeshCollider
enum IronColliderType
{
    BoxCollider = 1,
    SphereCollider = 2,
    CapsuleCollider = 3,
    MeshCollider = 4,
}

[System.Serializable]
public struct ColliderCollection
{
    public List<ColliderLink> colliders;
}

public class ExportCollider : MonoBehaviour
{
    public static ColliderCollection Export(string aSceneName, List<int> validInstanceIDs)
    {
        ColliderCollection colliderCollection = new ColliderCollection();
        colliderCollection.colliders = new List<ColliderLink>();
        Collider[] colliders = GameObject.FindObjectsOfType<Collider>();
        foreach (Collider collider in colliders)
        {
            if (validInstanceIDs.Contains(collider.transform.GetInstanceID()))
            {
                if (collider.GetComponent<Rigidbody>() == null)
                {
                    if(collider.GetComponent<Key>() == null)
                        Debug.LogWarning("Skipping Object. Missing Rigidbody Component, Please add one if you want to Export this collider =)", collider.gameObject);
                    else
                    {
                        if (collider.GetType() == typeof(BoxCollider))
                        {
                            ColliderLink triggerLink = new ColliderLink();
                            BoxCollider boxCollider = collider as BoxCollider;
                            triggerLink.instanceID = collider.transform.GetInstanceID();
                            triggerLink.isStatic = collider.gameObject.isStatic;
                            triggerLink.isTrigger = collider.isTrigger;
                            triggerLink.layer = 1 << collider.gameObject.layer;
                            triggerLink.colliderType = (int)IronColliderType.BoxCollider;
                            triggerLink.positionOffest = boxCollider.center;
                            triggerLink.boxSize = boxCollider.size;
                            colliderCollection.colliders.Add(triggerLink);
                            Debug.Log("NOT Expecting: ClockHand(1) == " + collider.gameObject.name);

                        }
                    }

                    continue;
                }

                ColliderLink link = new ColliderLink();
                link.instanceID = collider.transform.GetInstanceID();
                link.isStatic = collider.gameObject.isStatic;
                link.isTrigger = collider.isTrigger;
                link.layer = 1 << collider.gameObject.layer;             
                
                Rigidbody rigidbody = collider.GetComponent<Rigidbody>();
                link.mass = rigidbody.mass;
                link.localMassPosition = rigidbody.centerOfMass;
                link.inertiaTensor = rigidbody.inertiaTensor;
                link.isKinematic = rigidbody.isKinematic;

                link.dynamicFriction = collider.material.dynamicFriction;
                link.staticFriction = collider.material.staticFriction;
                link.bounciness = collider.material.bounciness;
                

                if (collider.GetType() == typeof(BoxCollider))
                {
                    BoxCollider boxCollider = collider as BoxCollider;
                    link.colliderType = (int)IronColliderType.BoxCollider;
                    link.positionOffest = boxCollider.center;
                    link.boxSize = boxCollider.size;
                    //Debug.Log("Expecting: ClockHand(1) == " + collider.gameObject.name);
                }
                else if (collider.GetType() == typeof(SphereCollider))
                {
                    SphereCollider sphereCollider = collider as SphereCollider;
                    link.colliderType = (int)IronColliderType.SphereCollider;
                    link.positionOffest = sphereCollider.center;
                    link.sphereRadius = sphereCollider.radius;
                }
                else if (collider.GetType() == typeof(CapsuleCollider))
                {
                    CapsuleCollider capsuleCollider = collider as CapsuleCollider;
                    link.colliderType = (int)IronColliderType.CapsuleCollider;
                    link.positionOffest = capsuleCollider.center;
                    link.capsuleHeight = capsuleCollider.height;
                    link.capsuleRadius = capsuleCollider.radius;
                }
                else if (collider.GetType() == typeof(MeshCollider))
                {
                    link.colliderType = (int)IronColliderType.MeshCollider;
                }
                else
                {
                    Debug.LogError("We don't accept This type of Collider: " + collider.GetType(), collider);
                }
                colliderCollection.colliders.Add(link);
            }

        }
        return colliderCollection;
    }
}
