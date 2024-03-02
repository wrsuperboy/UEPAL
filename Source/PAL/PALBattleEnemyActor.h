// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALBattleEnemyData.h"
#include "PALSpriteMeshComponent.h"
#include "PALPlayerState.h"
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

	SIZE_T CurrentFrameNum;

	uint16 EnemyId;

	float AnimationAccumulatedTime;

	bool bStopGuesture;

	uint16 PreviousHP;

	bool bHasText;

public:
	void Init(UPALBattleEnemyData* BattleEnemyData, uint16 InEnemyId, const FPALPosition3d& InOriginalPosition);

	void StopGuesture();

	void ResumeGuesture();

private:
	void HandleDamageDisplay(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
