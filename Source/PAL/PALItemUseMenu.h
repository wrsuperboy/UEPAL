// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PALPlayerState.h"
#include "PALRoleSelectMenu.h"
#include "PALItemUseMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALItemUseMenu : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APALPlayerState* PlayerStatePrivate;

	int16 Item;

	UPROPERTY()
	UPALRoleSelectMenu* RoleSelectMenu;

	UPROPERTY()
	UImage* ItemImage;

	UPROPERTY()
	UTextBlock* ItemCountText;

	UPROPERTY()
	UTextBlock* LevelNumber;
	UPROPERTY()
	UTextBlock* MaxHPNumber;
	UPROPERTY()
	UTextBlock* HPNumber;
	UPROPERTY()
	UTextBlock* MaxMPNumber;
	UPROPERTY()
	UTextBlock* MPNumber;
	UPROPERTY()
	UTextBlock* AttackStrengthNumber;
	UPROPERTY()
	UTextBlock* MagicStrengthNumber;
	UPROPERTY()
	UTextBlock* DefenseNumber;
	UPROPERTY()
	UTextBlock* DexterityNumber;
	UPROPERTY()
	UTextBlock* FleeRateNumber;

public:
	void Init(APALPlayerState* PlayerState, int16 InItem);

	void ChangeRole(SIZE_T RoleId);

	void SelectRole(SIZE_T RoleId);

private:
	UWidget* BuildContent();

	UWidget* BuildLabelText(const FString& Name);

	UTextBlock* BuildLabelNumber();

	UTextBlock* BuildLabelMaxNumber();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
	
};
