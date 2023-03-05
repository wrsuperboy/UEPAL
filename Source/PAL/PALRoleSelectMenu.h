// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "PALPlayerState.h"
#include "PALRoleSelectMenuItem.h"
#include "PALRoleSelectMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleSelectMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FRoleSelect, SIZE_T);

	FRoleSelect OnRoleSelected;

	FRoleSelect OnRoleChanged;

	bool bHasBorder = true;

private:
	UPROPERTY()
	UVerticalBox* ItemContainer;

	UPROPERTY()
	TArray<UPALRoleSelectMenuItem*> ItemList;

public:
	void AddPartyRole(SIZE_T RoleId, APALPlayerState* PlayerState, bool bSelectable);

	void ClearRoles();

	void SelectRole(SIZE_T RoleId);

	void ChangeRole(SIZE_T RoleId);
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
