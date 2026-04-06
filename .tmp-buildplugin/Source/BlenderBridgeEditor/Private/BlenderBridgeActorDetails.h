#pragma once

#include "IDetailCustomization.h"

class ABlenderBridgeActor;

class FBlenderBridgeActorDetails final : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance();

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
    FReply HandleRefreshInputSchemaClicked();
    FReply HandleRequestCookClicked();
    FReply HandleBakeToStaticMeshClicked();
    FReply HandleSelectBakedOutputClicked();
    FReply HandleDeleteBakedOutputClicked();

private:
    TArray<TWeakObjectPtr<ABlenderBridgeActor>> SelectedActors;
};
