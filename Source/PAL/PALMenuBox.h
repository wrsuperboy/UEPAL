// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PALMenuAnchor.h"
#include "PALMenuBox.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALMenuBox : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UWidget*> ItemList;

	UPROPERTY()
	TArray<UButton*> ItemButtonList;

	UPROPERTY()
	UVerticalBox* ItemContainer;

public:
	void AddItemAndBind(const FString& Name, UObject* InObject, const FName& InFunctionName);

	UPALMenuAnchor* AddMenuItemWithBind(const FString& Name, UObject* InObject, const FName& InFunctionName);

	void SetHittable(bool bHittable);
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
