#pragma once

#include "Factories/Factory.h"

#include "GeometryNodesBridgeSourceFactory.generated.h"

class FFeedbackContext;
class UClass;
class UObject;

UCLASS(hidecategories = Object)
class UGeometryNodesBridgeSourceFactory : public UFactory
{
    GENERATED_BODY()

public:
    UGeometryNodesBridgeSourceFactory(const FObjectInitializer& ObjectInitializer);

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
