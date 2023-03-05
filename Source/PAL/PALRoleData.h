// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PAL.h"
#include "PALRoleData.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleData : public UObject
{
	GENERATED_BODY()

public:
	SIZE_T RoleId; // player role

	FPALPosition3d Position; // position
	
	SIZE_T FrameNum; // current frame number
};
