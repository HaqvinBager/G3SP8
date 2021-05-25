using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[System.Serializable]
public struct Target
{
    public Target(Transform aTransform)
    {
        target = aTransform;
        newVisibility = true;
    }
    public bool newVisibility;
    public Transform target;
}

public class SetVisibility : MonoBehaviour
{
    public List<Transform> targets = new List<Transform>();

    public List<Target> testList = new List<Target>();
    public AnimationClip clip;

    static bool test = false;
    public void AnimationTest()
    {
        if (!test)
        {
            //var bindings = AnimationUtility.GetCurveBindings(clip);

            foreach(var binding in AnimationUtility.GetCurveBindings(clip))
            {
                AnimationCurve curve = AnimationUtility.GetEditorCurve(clip, binding);

                Debug.Log(binding.path + "/" + binding.propertyName + ", Keys: ");

                foreach(var key in curve.keys)
                {
                    Debug.Log("Time: " + key.time + " Value: " + key.value);
                }


            }

            //AnimationCurve curve = AnimationUtility.GetEditorCurve(clip)




            //EditorCurveBinding binding = EditorCurveBinding.DiscreteCurve("", typeof(Vector3), "Position");
            //AnimationClipCurveData data = new AnimationClipCurveData(binding);
            //
            //Debug.Log(data);
            //var keys = data.curve.keys;
            //Debug.Log("Time: " + keys[0].time + " Value: " + keys[0].value);
        }
        test = true;
    }

    private void OnDrawGizmosSelected()
    {
        AnimationTest();
    }
}

