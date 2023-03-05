// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALConfirmMenu.h"
#include "PALConfirmScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALConfirmScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	UPALConfirmMenu* ConfirmMenu;

public:
	template<typename FunctorType>
	void Init(FunctorType&& Callback);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};

template<typename FunctorType>
inline void APALConfirmScriptRunner::Init(FunctorType&& Callback)
{
	ConfirmMenu = CreateWidget<UPALConfirmMenu>(PlayerController, UPALConfirmMenu::StaticClass());
	ConfirmMenu->OnConfirm.AddLambda(Callback);
	TWeakObjectPtr<UPALConfirmMenu> ConfirmMenuWP(ConfirmMenu);
	ConfirmMenu->OnConfirm.AddWeakLambda(ConfirmMenu, [ConfirmMenuWP](bool bYes) {
		if (ConfirmMenuWP.IsValid())
		{
			ConfirmMenuWP->RemoveFromParent();
		}
		});
	ConfirmMenu->AddToViewport();
	MarkInitialized();
}
