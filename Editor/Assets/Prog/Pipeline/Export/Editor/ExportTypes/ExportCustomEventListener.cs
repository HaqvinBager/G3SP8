using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEditor.Events;
using UnityEngine.Events;
using System.Reflection;
/*
* Component Enabled(true/false) = "set_enabled"
*/

public struct SCustomEvent
{
    int instanceID;
    
}

public class ExportCustomEventListener
{
    [MenuItem("Export/Export Custom Event Listeners")]
    public static void Export()
    {
        CustomEventListener[] listeners = GameObject.FindObjectsOfType<CustomEventListener>();

        foreach (CustomEventListener listener in listeners)
        {
            int eventCount = listener.onResponse.GetPersistentEventCount();
            for (int i = 0; i < eventCount; ++i)
            {
                Object target = listener.onResponse.GetPersistentTarget(i);
                string methodName = listener.onResponse.GetPersistentMethodName(i);

                SerializedObject serObj = new SerializedObject(target);
                var it = serObj.GetIterator();
                while (it.Next(true))
                {
                    Debug.Log(it?.boolValue, it?.objectReferenceValue);
                }


                MethodInfo methodInfo = target.GetType().GetMethod(methodName);
                ParameterInfo[] paramInfo = methodInfo.GetParameters();
                System.Type[] types = new System.Type[paramInfo.Length];
                int index = 0;
                foreach(ParameterInfo param in paramInfo)
                {
                    types[index] = param.ParameterType;
                    Debug.Log(param.ParameterType.ToString());
                    index++;
                }

                MethodBody body = methodInfo.GetMethodBody();
                IList<LocalVariableInfo> localVariable = body?.LocalVariables;
                int typeIndex = 0;
                if(localVariable != null)
                    foreach(var lv in localVariable)
                    {
                        
                        //Debug.Log(lv.LocalType.FullName + " = " + UnityEvent.GetValidMethodInfo(target, methodName, types).;
                        typeIndex++;
                    }

                //System.Type[] types = new System.Type[1];
                //types[0] = typeof(bool);
                //MethodInfo methodInfo = UnityEvent.GetValidMethodInfo(target, methodName, types);
                //methodInfo.

                Debug.Log("Target:  " + target.name + " Method:  " + listener.onResponse.GetPersistentMethodName(i), target);
            }
        }
    }
}
