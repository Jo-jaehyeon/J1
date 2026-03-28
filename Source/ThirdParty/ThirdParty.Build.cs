using System.IO;
using UnrealBuildTool;

public class ThirdParty : ModuleRules
{
    public ThirdParty(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Libs", "Win64", "Protobuf", "libprotobuf.lib"));

            PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
        }
    }
}