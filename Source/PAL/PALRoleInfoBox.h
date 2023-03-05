// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PALRoleInfoBox.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleInfoBox : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FRoleInfoSelect, SIZE_T);

	FRoleInfoSelect OnSelect;

private:
	SIZE_T RoleId;

	UPROPERTY()
	UImage* FaceImage;

	UPROPERTY()
	UTextBlock* HPNumber;

	UPROPERTY()
	UTextBlock* MaxHPNumber;

	UPROPERTY()
	UTextBlock* MPNumber;

	UPROPERTY()
	UTextBlock* MaxMPNumber;

	UPROPERTY()
	UTextBlock* Status;

	UPROPERTY()
	UButton* Button;

	bool bSelectionEnabled;

public:
	void Init(SIZE_T InRoleId);

	void SetSelectionEnabled(bool bInSelectionEnabled);

	void Refresh();

	UFUNCTION()
	void Select();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
