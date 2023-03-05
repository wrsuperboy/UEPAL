// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALMenuBase.h"
#include "PALMenuBox.h"
#include "Containers/Queue.h"
#include "PALInGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALInGameMenu : public UPALMenuBase
{
	GENERATED_BODY()

private:
	TArray<TTuple<UWidget*, TSubclassOf<UWidget>>> MenuStack;

	UPROPERTY()
	UWidget* InGameMagicMenuAnchor;

	UPROPERTY()
	UWidget* InventoryMenuAnchor;

	UPROPERTY()
	UWidget* SystemMenuAnchor;

public:
	UFUNCTION()
	void EmptyFunction();

	UFUNCTION()
	void CreateRoleStatusMenu();

	UFUNCTION()
	UUserWidget* CreateInGameMagicMenu();

	UFUNCTION()
	void CreateEquipItemMenu();

	UFUNCTION()
	void CreateUseItemMenu();

	UFUNCTION()
	UPALMenuBox* CreateIventoryMenu();

	UFUNCTION()
	void CreateSaveGameMenu();

	UFUNCTION()
	void CreateLoadGameMenu();

	UFUNCTION()
	void CreateEnableMusicMenu();

	UFUNCTION()
	void CreateEnableSoundMenu();

	UFUNCTION()
	void CreateQuitGameConfirmMenu();

	UFUNCTION()
	UUserWidget* CreateSystemMenu();

private:
	void PushStack(const TTuple<UWidget*, TSubclassOf<UWidget>>& WidgetAndClass);

public:
	virtual bool GoBack() override;

protected:
	virtual void NativeConstruct() override;
};
