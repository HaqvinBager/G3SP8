using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;

public class Test 
{
    public static string monoTxt =
        "\n\n\n public class AudioClipGenerated : MonoBehaviour { \n";

    [MenuItem("Custom/Test Read")]
    public static void TestRead()
    {
        string path = "../IronWrought/Source/Engine/AudioManager.h";
        string text = File.ReadAllText(path);
        text = text.Substring(text.IndexOf("//START") + 7, text.IndexOf("//STOP") - text.IndexOf("//START"));
        List<string> enums = text.Split('\n').ToList();


        List<string> enumWhole = new List<string>();
        var file = File.CreateText("Assets/Generated/AudioFile.cs");
        file.Write("using UnityEngine;\n");
        foreach(var str in enums)
        {

            string final = str;
            int startIndex = final.IndexOf("class");
            if (startIndex < 0)
                continue;

            final = final.Replace("enum class", "public enum");
            enumWhole.Add(final);

            file.WriteLine(final);
            enumWhole.Add(final);
        }


        string script =  monoTxt;
        foreach (var str in enumWhole)
        {
            string final = str;
            if (final.IndexOf("class") < 0)
                continue;
            //script.Append("public enum");
            script += str + "\n";
        }
        script += "}";


        file.Write(script);


        file.Close();




        AssetDatabase.Refresh();


        foreach (var str in enums)
            if (str.Length == 0)
                enums.Remove(str);

        foreach (var str in enums)
        {
            char[] seperators = new char[4] { '{', ',', '}', '\n' };
            List<string> actualEnumNames = str.Split(seperators, StringSplitOptions.RemoveEmptyEntries).ToList();
            actualEnumNames.TrimExcess();
            foreach (var enumName in actualEnumNames)
            {             
                if(!enumName.Contains("enum") && !enumName.Contains("class") && !enumName.Contains(";") && (enumName.Length > 1))
                    Debug.Log(enumName);
            }
        }
        
        //while(currentIndex != -1)
        //{
        //    currentIndex = endIndex;
        //    endIndex += text.IndexOf("};", currentIndex) + 3;
        //    string enumString = text.Substring(currentIndex, endIndex);
        //    Debug.Log(enumString);
        //}
        //enums.Add(text.Substring(text.IndexOf("enum class", currentIndex, text.IndexOf("};", currentIndex))));

        //foreach (string enumString in enums)
        //    Debug.Log(enumString);



    }

}
