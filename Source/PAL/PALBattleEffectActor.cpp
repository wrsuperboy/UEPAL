// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleEffectActor.h"
#include "PALCommon.h"
#include "PALBattle.h"

// Sets default values
APALBattleEffectActor::APALBattleEffectActor()
{
	FrameAccumulatedTime = 0;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void APALBattleEffectActor::Init(SIZE_T InStartFrameNum, SIZE_T InEndFrameNum)
{
	CurrentFrameNum = InStartFrameNum;
	EndFrameNum = InEndFrameNum;
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetBattleEffectSprite();
	SpriteMeshComponent->SetSprite(Sprite);
}

// Called when the game starts or when spawned
void APALBattleEffectActor::BeginPlay()
{
	Super::BeginPlay();
	SpriteMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	SpriteMeshComponent->SetFrame(CurrentFrameNum);
}

// Called every frame
void APALBattleEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FrameAccumulatedTime += DeltaTime;
	if (FrameAccumulatedTime > BATTLE_FRAME_TIME)
	{
		FrameAccumulatedTime = 0;
		CurrentFrameNum++;
		if (CurrentFrameNum >= EndFrameNum)
		{
			Destroy();
		}
		else
		{
			SpriteMeshComponent->SetFrame(CurrentFrameNum);
		}
	}
}

