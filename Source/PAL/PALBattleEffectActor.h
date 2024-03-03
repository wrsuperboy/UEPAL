// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALSpriteMeshComponent.h"
#include "PALBattleEffectActor.generated.h"

UCLASS()
class PAL_API APALBattleEffectActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APALBattleEffectActor();

private:
	// Excluded end
	SIZE_T EndFrameNum;

	SIZE_T CurrentFrameNum;

	float FrameAccumulatedTime;

	UPROPERTY(VisibleAnywhere)
	UPALSpriteMeshComponent* SpriteMeshComponent;

public:
	void Init(SIZE_T InStartFrameNum, SIZE_T InEndFrameNum);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
