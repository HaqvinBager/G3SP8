using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
[CustomEditor(typeof(Lock))]
public class LockEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        if(GUILayout.Button("[Optional] Open Lock Overview"))
        {
            Lock script = (Lock)target;
            List<GameObject> selectKeys = new List<GameObject>();
            Key[] keys = FindObjectsOfType<Key>();
            foreach(Key key in keys)
            {
                if(key.myLock == script)
                {
                    selectKeys.Add(key.gameObject);
                }
            }

           // Selection.objects = selectKeys.ToArray();
            LockEditorWindow.OpenWindow(selectKeys);
        }
    }
}


public class LockEditorWindow : EditorWindow
{
    public static void OpenWindow(List<GameObject> gameObjects)
    {
        LockEditorWindow window = EditorWindow.CreateWindow<LockEditorWindow>();

        Key[] keys = FindObjectsOfType<Key>();

        window.lockKeysMap = new Dictionary<Lock, List<Key>>();
        foreach (Key key in keys)
        {
            if(window.lockKeysMap.TryGetValue(key.myLock, out List<Key> lockKeys)){
                lockKeys.Add(key);
            }
            else
            {
                window.locks.Add(key.myLock);
                window.lockIDMap.Add(key.myLock.GetInstanceID(), key.myLock);
                window.lockKeysMap.Add(key.myLock, new List<Key>());
                window.lockKeysMap[key.myLock].Add(key);
            }
        }

        int posY = 0;
        window.windowRectMap.Add(window.locks[0], new Rect(0, posY, 300, 0));
        for (int i = 1; i < window.locks.Count; ++i)
        {
            posY += (window.lockKeysMap[window.locks[i - 1]].Count * 20);
            int posYNew = (i * 20) + posY;
            window.windowRectMap.Add(window.locks[i], new Rect(0, posYNew, 300, 0));
        }
    }

    List<Lock> locks = new List<Lock>();
    Dictionary<Lock, List<Key>> lockKeysMap = new Dictionary<Lock, List<Key>>();
    Dictionary<int, Lock> lockIDMap = new Dictionary<int, Lock>();
    Dictionary<Lock, Rect> windowRectMap = new Dictionary<Lock, Rect>();

    public void OnGUI()
    {
        BeginWindows();

        foreach(Lock lockKey in locks)
        {
             windowRectMap[lockKey] = GUILayout.Window(lockKey.GetInstanceID(), windowRectMap[lockKey], RenderWindow, lockKey.name);
        }
        EndWindows();     
    }

    void RenderWindow(int id)
    {
        Lock theLock = lockIDMap[id];
        List<Key> keys = lockKeysMap[theLock];
        foreach (Key key in keys)
        {
            EditorGUILayout.ObjectField("Key", key.gameObject, typeof(GameObject), true);
        }
        GUI.DragWindow();
    }
}