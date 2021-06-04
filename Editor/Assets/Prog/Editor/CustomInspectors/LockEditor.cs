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

        if (GUILayout.Button("[Optional] Open Lock Overview"))
        {
            Lock script = (Lock)target;
            List<GameObject> selectKeys = new List<GameObject>();
            Key[] keys = FindObjectsOfType<Key>();
            foreach (Key key in keys)
            {
                if (key.myLock == null)
                    continue;

                if (key.myLock == script)
                {
                    selectKeys.Add(key.gameObject);
                }
            }
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
            if (key.myLock == null)
                continue;

            if (window.lockKeysMap.TryGetValue(key.myLock, out List<Key> lockKeys))
            {
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
        for (int i = 1; i < window.locks.Count - 1; ++i)
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


    //SceneSetup[] sceneSetups = null;
    //bool showRestoreButton = false;
    //List<GameObject> hideGameObjects = new List<GameObject>();

    public void OnGUI()
    {
        BeginWindows();

        foreach (Lock lockKey in locks)
        {
            if (windowRectMap.TryGetValue(lockKey, out Rect rect))
            {
                rect = GUILayout.Window(lockKey.GetInstanceID(), windowRectMap[lockKey], RenderWindow, lockKey.name);
                //windowRectMap[lockKey] =
            }
        }
        EndWindows();
    }

    private void OnDestroy()
    {
        locks.Clear();
        lockKeysMap.Clear();
        lockIDMap.Clear();
        windowRectMap.Clear();
        //hideGameObjects.ForEach(e => e.SetActive(true));
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

//if (!showRestoreButton)
//{
//    if (GUILayout.Button("Isolate"))
//    {
//        GameObject[] gos = GameObject.FindObjectsOfType<GameObject>();
//        foreach (GameObject go in gos)
//        {
//            if (!go.activeInHierarchy)
//                continue;


//            if (go.TryGetComponent(out Key key))
//            {
//                if (keys.Contains(key))
//                {
//                    continue;
//                }
//            }
//            else if (go.GetComponentInParent<Key>() != null)
//            {
//                continue;
//            }

//            hideGameObjects.Add(go);
//        }
//    }

//    hideGameObjects.ForEach(e => e.SetActive(false));
//    showRestoreButton = true;
//}

//if (showRestoreButton)
//{
//    if (GUILayout.Button("Restore"))
//    {
//        hideGameObjects.ForEach(e => e.SetActive(true));
//        showRestoreButton = false;
//        hideGameObjects.Clear();
//    }
//}