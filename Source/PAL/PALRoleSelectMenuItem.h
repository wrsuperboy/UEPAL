// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALRoleSelectMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleSelectMenuItem : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FMenuRoleSelect, SIZE_T);

	FMenuRoleSelect OnSelect;

	FMenuRoleSelect OnHover;

private:
	SIZE_T RoleId;

	FString RoleName;

	bool bSelectable;

public:
	void Init(SIZE_T InRoleId, const FString& InRoleName, bool bSelectable);

	UFUNCTION()
	void Select();

	UFUNCTION()
	void Hover();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;

};
