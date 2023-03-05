// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALSpriteMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "PALPlayerState.h"
#include "PAL.h"
#include "PALRolePawn.generated.h"

UCLASS()
class PAL_API APALRolePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APALRolePawn();

private:
	UPROPERTY(VisibleAnywhere)
	UPALSpriteMeshComponent* SpriteMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UPALRoleData* RoleDataPrivate;

	UPROPERTY(VisibleAnywhere)
	APALPlayerState* PlayerStatePrivate;

	// Should be in input component
	bool bInputDirectionEnabled;
	float LastInputTime;
	EPALDirection KeyInputDirection;
	EPALDirection PreviousKeyInputDirection;
	EPALDirection JoystickInputDirection;

	bool bFollower;

	float Speed;

	bool bWalking;

	bool bAtEase;

	float StandTime;

	SIZE_T ThisStepFrame;

	float ThisStepFrameTime;

public:
	void Init(UPALRoleData* RoleData, APALPlayerState* PlayerState, bool bFollower = false);

	SIZE_T GetRoleId() const;

	bool IsFollower() const;

	void UpdateSprite();

	const FPALPosition3d& GetPosition() const;

	void UpKeyUp();

	void UpKeyDown();

	void DownKeyUp();

	void DownKeyDown();

	void LeftKeyUp();

	void LeftKeyDown();

	void RightKeyUp();

	void RightKeyDown();

	void JoystickAxisInput(float Value);

	void SetWalking();

	void StopWalking();

	void SetAtEase(bool bInAtEase);

	void SetSpeed(float NewSpeed);

	// Reset speed immediately
	void ResetSpeed();

	void Search();

	bool IsInputEnabled() const { return bInputDirectionEnabled; }

private:
	void MoveByInput();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void DisableInput(APlayerController* PlayerController) override;

	virtual void EnableInput(APlayerController* PlayerController) override;
};
