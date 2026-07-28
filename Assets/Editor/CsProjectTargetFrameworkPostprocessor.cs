using UnityEditor;

internal sealed class CsProjectTargetFrameworkPostprocessor : AssetPostprocessor
{
    private const string UnityGeneratedTargetFramework =
        "<TargetFrameworkVersion>v4.7.1</TargetFrameworkVersion>";

    private const string NUnitCompatibleTargetFramework =
        "<TargetFrameworkVersion>v4.7.2</TargetFrameworkVersion>";

    public static string OnGeneratedCSProject(string path, string content)
    {
        return content.Replace(
            UnityGeneratedTargetFramework,
            NUnitCompatibleTargetFramework);
    }
}
