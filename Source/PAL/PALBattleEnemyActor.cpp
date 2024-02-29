// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleEnemyActor.h"
#include "PALCommon.h"
#include "PALGameInstance.h"

// Sets default values
APALBattleEnemyActor::APALBattleEnemyActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SpriteMeshComponent = CreateDefaultSubobject<UPALSpriteMeshComponent>(TEXT("SpriteMeshComponent"));

	SetActorTickEnabled(false);
	CurrentFrameNum = 0;
	AnimationAccumulatedTime = 0;
}

void APALBattleEnemyActor::Init(UPALBattleEnemyData* BattleEnemyData, uint16 InEnemyId, const FPALPosition3d& InOriginalPosition)
{
	BattleEnemyDataPrivate = BattleEnemyData;
	OriginalPosition = InOriginalPosition;
	Position = InOriginalPosition;
	// Load battle sprites for enemy
	EnemyId = InEnemyId;
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetBattleEnemySprite(EnemyId);
	SpriteMeshComponent->SetSprite(Sprite);
	SpriteMeshComponent->SetLocationOffset(FVector3d(0, BattleEnemyData->Enemy.YPosOffset * 2 * PIXEL_TO_UNIT, 0));
	SetActorLocation(Position.toLocation());
	SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void APALBattleEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	SpriteMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	SpriteMeshComponent->SetLayer(0);
}

// Called every frame
void APALBattleEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Position = OriginalPosition;

	if (BattleEnemyDataPrivate->Status[EPALStatus::Sleep] > 0 ||
		BattleEnemyDataPrivate->Status[EPALStatus::Paralyzed] > 0)
	{
		CurrentFrameNum = 0;
		AnimationAccumulatedTime = 0;
	}
	else
	{
		AnimationAccumulatedTime += DeltaTime;
		if (AnimationAccumulatedTime > FRAME_TIME)
		{
			AnimationAccumulatedTime = 0;
			if (--(BattleEnemyDataPrivate->Enemy.IdleAnimSpeed) == 0)
			{
				CurrentFrameNum++;
				BattleEnemyDataPrivate->Enemy.IdleAnimSpeed =
					GetGameInstance<UPALGameInstance>()->GetGameData()->Enemies[EnemyId].IdleAnimSpeed;
			}

			if (CurrentFrameNum >= BattleEnemyDataPrivate->Enemy.IdleFrames)
			{
				CurrentFrameNum = 0;
			}
		}
	}

	FVector3d RealLocation = Position.toLocation();
	if (FVector3d::Dist(RealLocation, GetActorLocation()) > 100 * PIXEL_TO_UNIT)
	{
		SetActorLocation(RealLocation);
	}
	else
	{
		FVector3d Step = RealLocation - GetActorLocation();
		if (!Step.IsNearlyZero())
		{
			Step.Normalize();
			Step *= 24 * PIXEL_TO_UNIT / FRAME_TIME * DeltaTime;
			FVector3d Destination = GetActorLocation() + Step;
			if (FVector3d::DotProduct(Destination - RealLocation, Step) > 0)
			{
				SetActorLocation(RealLocation);
			}
			else
			{
				SetActorLocation(Destination);
			}
		}
	}

	SpriteMeshComponent->SetFrame(CurrentFrameNum);
}
