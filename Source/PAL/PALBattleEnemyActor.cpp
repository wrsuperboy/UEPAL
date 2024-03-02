// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleEnemyActor.h"
#include "Components/TextRenderComponent.h"
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
	bStopGuesture = false;
	bHasText = false;
}

void APALBattleEnemyActor::Init(UPALBattleEnemyData* BattleEnemyData, uint16 InEnemyId, const FPALPosition3d& InOriginalPosition)
{
	BattleEnemyDataPrivate = BattleEnemyData;
	OriginalPosition = InOriginalPosition;
	Position = InOriginalPosition;
	// Load battle sprites for enemy
	EnemyId = InEnemyId;
	PreviousHP = BattleEnemyData->Enemy.Health;
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetBattleEnemySprite(EnemyId);
	SpriteMeshComponent->SetSprite(Sprite);
	SpriteMeshComponent->SetLocationOffset(FVector3d(0, BattleEnemyData->Enemy.YPosOffset * 2 * PIXEL_TO_UNIT, 0));
	SetActorLocation(Position.toLocation());
	SetActorTickEnabled(true);
}

void APALBattleEnemyActor::StopGuesture()
{
	bStopGuesture = true;
}

void APALBattleEnemyActor::ResumeGuesture()
{
	bStopGuesture = false;
}

void APALBattleEnemyActor::HandleDamageDisplay(float DeltaTime)
{
	if (bHasText)
	{
		TArray<UTextRenderComponent*> TextRenderComponents;
		GetComponents(TextRenderComponents);
		for (UTextRenderComponent* TextRenderComponent : TextRenderComponents) {
			if (TextRenderComponent->GetRelativeLocation().Z > (115 + 16) * SQRT_3 / 2 * PIXEL_TO_UNIT)
			{
				TextRenderComponent->DestroyComponent();
				if (TextRenderComponents.Num() == 1)
				{
					bHasText = false;
				}
				continue;
			}

			TextRenderComponent->SetRelativeLocation(TextRenderComponent->GetRelativeLocation() + FVector3d(0, 0, 1 * SQRT_3 / 2 * PIXEL_TO_UNIT / 0.04f * DeltaTime));
			FColor& CurrentColor = TextRenderComponent->TextRenderColor;
			const uint8 AlphaToDeduct = 16 * DeltaTime / 0.04f;
			if (CurrentColor.A < AlphaToDeduct)
			{
				CurrentColor.A = 0;
			}
			else
			{
				CurrentColor.A -= AlphaToDeduct;
			}
		}
	}

	int16 Damage = static_cast<int16>(BattleEnemyDataPrivate->Enemy.Health) - PreviousHP;
	if (Damage != 0)
	{
		PreviousHP = BattleEnemyDataPrivate->Enemy.Health;
		// Show the number of damage
		double ZOffset = 115 * SQRT_3 / 2 * PIXEL_TO_UNIT;

		UTextRenderComponent* TextComponent = NewObject<UTextRenderComponent>(this, UStaticMeshComponent::StaticClass());
		TextComponent->SetupAttachment(RootComponent);
		TextComponent->SetRelativeLocation(FVector3d(0, 0, ZOffset));
		if (Damage < 0)
		{
			TextComponent->SetText(FText::FromString(FString::FromInt(-Damage)));
			TextComponent->SetTextRenderColor(FColor(80, 184, 148, 255));
		}
		else
		{
			TextComponent->SetText(FText::FromString(FString::FromInt(Damage)));
			TextComponent->SetTextRenderColor(FColor(212, 200, 168, 255));
		}
		TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
		TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextTop);
		TextComponent->RegisterComponent();
		bHasText = true;
	}
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

	if (!bStopGuesture)
	{
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

	HandleDamageDisplay(DeltaTime);
}
