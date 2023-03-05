// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "PALRolePawn.h"
#include "PALMenuBase.h"
#include "PALOpeningMenu.h"
#include "PAL.h"
#include "PALPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APALPlayerController();
	
private:
	UPROPERTY(VisibleAnywhere)
	TArray<APALRolePawn*> RolePawns;

	UPROPERTY(VisibleAnywhere)
	ACameraActor* CameraActor;

	UPROPERTY(VisibleAnywhere)
	UPALMenuBase* CurrentMenu;

public:
	void ReloadRoles();

	void ReloadRoleSprites();

	APALRolePawn* GetRolePawn(const SIZE_T RoleId) const;

	const FPALPosition3d& GetPartyPosition() const;

	FPALPosition3d GetViewport() const;

	void ControllByGame();

	void ReleaseControllFromGame();

	bool IsControlledByGame() const;

	void RemoveAllPartyRoles();

	void RemoveAllFollowerRoles();

	void AddPartyRole(const SIZE_T RoleId);

	void AddFollowerRole(const SIZE_T RoleId);

	void PartyMoveTo(const FPALPosition3d& Position);

	void MainRoleMoveTo(const FPALPosition3d& Position);

	void PartyWalkTo(const FPALPosition3d& Position);

	void OnMainRoleWalk();

	void PartyAtEase(bool bAtEase);

	void RoleAtEase(SIZE_T RoleId, bool bAtEase);

	void RoleStopWalking(SIZE_T RoleId);

	void PartySetWalking();

	void PartyStopWalking();

	void PartyResetSpeed();

	void PartySetSpeed(float Speed);

	void PartyMoveClose();

	void OnEscape();

	void CloseCurrentMenu();

	void ControllOpeningMenu(UPALOpeningMenu* OpeningMenu);

	void GameRoleStatus();

	void GameEquipItem();

	void GameUseItem();

	void GameUseMagic(SIZE_T RoleId);

	void Buy(uint16 StoreNum);

	void Sell();

	void CameraRestoreNormal();

	void CameraMoveTo(const FPALPosition3d& Position);

public:
	virtual void SetupInputComponent() override;
};
