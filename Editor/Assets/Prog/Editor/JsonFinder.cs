using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class JsonFinder
{
    //private static string myTargetPath = "\\IronWrought\\Bin\\json\\";



    public static List<string> GetJsonPaths(string aPrefix)
    {
        string[] paths = Directory.GetFiles(MagicString.VFXJsonPath, "*.json");
        List<string> particleJsonPaths = new List<string>();

        foreach (var path in paths)
        {
            int startIndex = path.IndexOf(aPrefix);
            if (startIndex > 0)
            {
                string particleJsonName = path.Substring(startIndex, (path.Length - startIndex) - 5);
                if (!particleJsonPaths.Contains(particleJsonName))
                {
                    particleJsonPaths.Add(particleJsonName);
                }
            }
        }

        return particleJsonPaths;
    }


}
