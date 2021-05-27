using System.Collections;
using System.Collections.Generic;
using UnityEngine;



public enum ELevelName
{
    Cottage_1,
    Cottage_2,
    Cottage_3,
    Basement_1_1,
    Basement_1_2,
    Basement_1_3,
    Basement_2
}

public class LevelTeleporter : MonoBehaviour
{
    [Header("Z-direction of teleporter = Z-direction of teleported object.")]
    public bool ok;
    [Header("Set name of the teleporter, IronWrought expects one of each.")]
    public ELevelName myTeleporterName;
    [Header("The name of the teleporter that this teleporter connects to.")]
    public ELevelName teleportTo;
    [Header("The position to place the teleported object that is teleported to this object.")]
    public Vector3 onTeleportToMePlaceOnPosition;
}
