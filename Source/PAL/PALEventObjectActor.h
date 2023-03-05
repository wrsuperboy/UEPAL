// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALGameData.h"
#include "PALSprite.h"
#include "PALSpriteMeshComponent.h"
#include "PAL.h"
#include "PALPlayerState.h"
#include "PALEventObjectActor.generated.h"

UCLASS()
class PAL_API APALEventObjectActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APALEventObjectActor();

private:
	uint16 EventObjectId;

	FEventObject* EventObjectPrivate;

	UPROPERTY(VisibleAnywhere)
	UPALSprite* Sprite;

	UPROPERTY(VisibleAnywhere)
	UPALSpriteMeshComponent* SpriteMeshComponent;

	float TimeFromLastFrame;

	float Speed;

	float LastInteractionTime;

public:
	void Init(uint16 EventObjectId);

	bool IsNotHidden() const;

	bool IsBlocking() const;

	uint16 GetEventObjectId() const;

	FPALPosition3d GetPosition() const;

	bool CanBeSearchedFrom(const FPALPosition3d& SearchPosition, EPALDirection Direction) const;

	void SearchBy(APALPlayerState* PlayerState);

	void SetSpeed(float Speed);

	void MoveTo(const FPALPosition3d& Position);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
