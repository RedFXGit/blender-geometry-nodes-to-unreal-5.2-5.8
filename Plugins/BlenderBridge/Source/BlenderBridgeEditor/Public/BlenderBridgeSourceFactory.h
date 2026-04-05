#pragma once

#include "Factories/Factory.h"

#include "BlenderBridgeSourceFactory.generated.h"

class FFeedbackContext;
class UClass;
class UObject;

UCLASS(hidecategories = Object)
class UBlenderBridgeSourceFactory : public UFactory
{
    GENERATED_BODY()

public:
    UBlenderBridgeSourceFactory(const FObjectInitializer& ObjectInitializer);

    virtual bool FactoryCanImport(const FString& Filename) override;
    virtual UObject* FactoryCreateFile(
        UClass* InClass,
        UObject* InParent,
        FName InName,
        EObjectFlags Flags,
        const FString& Filename,
        const TCHAR* Parms,
        FFeedbackContext* Warn,
        bool& bOutOperationCanceled) override;
};
