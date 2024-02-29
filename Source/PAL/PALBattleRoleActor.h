// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALSpriteMeshComponent.h"
#include "PALRoleData.h"
#include "PALPlayerState.h"
#include "GameFramework/Actor.h"
#include "PALBattleRoleActor.generated.h"

UCLASS()
class PAL_API APALBattleRoleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APALBattleRoleActor(); 

private:
	SIZE_T RoleId;

	UPROPERTY(VisibleAnywhere)
	UPALSpriteMeshComponent* SpriteMeshComponent;

	UPROPERTY()
	APALPlayerState* PlayerStatePrivate;

	bool bDefending;

	FPALPosition3d OriginalPosition;

	FPALPosition3d Position;

public:
	void Init(UPALRoleData* RoleData, APALPlayerState* PlayerState, const FPALPosition3d& InOriginalPosition);
	
	void SetDefending(bool bInDefending);

	void RestorePosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
