// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "PALShowCash.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALShowCash : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	UTextBlock* Value;

public:
	void Refresh();

private:
	UHorizontalBox* BuildLabel();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;

};
