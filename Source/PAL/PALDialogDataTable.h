// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Components/RichTextBlock.h"
#include "PALCommon.h"
#include "PALDialogDataTable.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALDialogDataTable : public UDataTable
{
	GENERATED_BODY()

public:
	UPALDialogDataTable();

public:
	void Init(UPALCommon* Common);
};
