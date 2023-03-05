// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "PALDialog.generated.h"

enum EPALDialogLocation : uint8
{
	DialogUpper,
	DialogCenter,
	DialogLower,
	DialogCenterWindow
};

enum EDisplayTag : uint8
{
	TagNone,
	TagCyan,
	TagRed,
	TagRedAlt,
	TagYellow
};

/**
 * 
 */
UCLASS()
class PAL_API UPALDialog : public UUserWidget
{
	GENERATED_BODY()

private:
	EPALDialogLocation DialogLocation;

	SIZE_T CharacterFaceNum;

	UPROPERTY()
	UTextBlock* Title;

	UPROPERTY()
	URichTextBlock* Text;

	UPROPERTY()
	UScrollBox* TextScrollBox;

	EDisplayTag Tag;

public:
	void Start(EPALDialogLocation InDialogLocation, SIZE_T FontColorNum, SIZE_T InCharacterFaceNum = 0);

	void ShowText(const FString& TextString);

	bool IsDialog() const;

private:
	FString DisplayText(UPALDialog* Dialog, const FString& TextString);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
	
};
