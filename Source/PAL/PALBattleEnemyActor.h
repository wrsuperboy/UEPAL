// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALBattleEnemyData.h"
#include "PALSpriteMeshComponent.h"
#include "PALPlayerState.h"
#include "PALBattleEffectActor.h"
#include "PALBattleEnemyActor.generated.h"

UCLASS()
class PAL_API APALBattleEnemyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APALBattleEnemyActor();

private:
	UPROPERTY(VisibleAnywhere)
	UPALSpriteMeshComponent* SpriteMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UPALBattleEnemyData* BattleEnemyDataPrivate;

	FPALPosition3d OriginalPosition;

	FPALPosition3d Position;

	double InitialHeight;

	SIZE_T CurrentFrameNum;

	uint16 EnemyId;

	float GestureAccumulatedTime;

	bool bStopGuesture;

	uint16 PreviousHP;

	bool bHasText;

	UPROPERTY()
	APALBattleEnemyActor* FriendToAttack;

	enum EPALBattleActionPhase : uint8 {
		Approaching,
		PreMagic,
		TakingEffect,
		PostMagic,
	};
	EPALBattleActionPhase AttackFriendPhase;

	float AttackFriendApproachingTime;

	UPROPERTY()
	APALBattleEffectActor* AttackFriendEffect;

public:
	void Init(UPALBattleEnemyData* BattleEnemyData, uint16 InEnemyId, const FPALPosition3d& InOriginalPosition);

	void StopGuesture();

	void ResumeGuesture();

	void ConfusedToAttack(APALBattleEnemyActor* Target);

	const FPALPosition3d& GetPosition() const;

	double GetInitialHeight() const;

	const UPALBattleEnemyData* GetEnemyData() const;

	void TakeDamageAndAnimate(int16 Damage);

private:
	void HandleDamageDisplay(float DeltaTime);

	void HandleConfusedAttacking(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
