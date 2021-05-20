using System.IO;
using System.Text;
using UnityEditor;
using UnityEngine;

public class ExportNavMeshToObj
{
    [MenuItem("Custom/[Deprecated] Export NavMesh to mesh")]
    public static void Export()
    {
        UnityEngine.AI.NavMeshTriangulation triangulatedNavMesh = UnityEngine.AI.NavMesh.CalculateTriangulation();

        Mesh mesh = new Mesh
        {
            name = "ExportedNavMesh",
            vertices = triangulatedNavMesh.vertices,
            triangles = triangulatedNavMesh.indices
        };

        string fileName = MagicString.GeneratedPath + UnityEngine.SceneManagement.SceneManager.GetActiveScene().name + mesh.name + ".obj";
        MeshToFile(mesh, fileName);
        Debug.Log("NavMesh exported as '" + fileName + "'");
        AssetDatabase.Refresh();
    }

    static string MeshToString(Mesh mesh)
    {
        StringBuilder sb = new StringBuilder();

        sb.Append("g ").Append(mesh.name).Append("\n");
        foreach (Vector3 v in mesh.vertices)
        {
            sb.Append(string.Format("v {0} {1} {2}\n", v.x, v.y, v.z));
        }

        for (int material = 0; material < mesh.subMeshCount; material++)
        {
            sb.Append("\n");

            int[] triangles = mesh.GetTriangles(material);
            for (int i = 0; i < triangles.Length; i += 3)
            {
                sb.Append(string.Format("f {0}/{0}/{0} {1}/{1}/{1} {2}/{2}/{2}\n", triangles[i] + 1, triangles[i + 1] + 1, triangles[i + 2] + 1));
            }
        }
        return sb.ToString();
    }

    static void MeshToFile(Mesh mesh, string filename)
    {
        using (StreamWriter sw = new StreamWriter(filename))
        {
            sw.Write(MeshToString(mesh));
        }
    }
}