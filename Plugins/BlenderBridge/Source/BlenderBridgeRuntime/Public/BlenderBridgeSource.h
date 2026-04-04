#pragma once

#include "Engine/DataAsset.h"
#include "UObject/NoExportTypes.h"

#include "BlenderBridgeSource.generated.h"

UCLASS(BlueprintType)
class BLENDERBRIDGERUNTIME_API UBlenderBridgeSource : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender")
    FFilePath BlendFile;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender")
    FString ObjectName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender")
    FString ModifierName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blender")
    bool bHelloWorldMode = false;
};
