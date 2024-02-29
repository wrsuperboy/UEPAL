// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PALOpeningMenu.h"
#include "PALOpeningPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALOpeningPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UPALOpeningMenu* OpeningMenu;

public:
	void OnEscape();

	void ControllOpeningMenu(UPALOpeningMenu* InOpeningMenu);

public:
	virtual void SetupInputComponent() override;
};
