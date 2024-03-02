// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleRoleActor.h"
#include "Components/TextRenderComponent.h"
#include "PALBattleGameMode.h"
#include "PALCommon.h"

// Sets default values
APALBattleRoleActor::APALBattleRoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SpriteMeshComponent = CreateDefaultSubobject<UPALSpriteMeshComponent>(TEXT("SpriteMeshComponent"));
	bDefending = false;
	SetActorTickEnabled(false);
	bHasText = false;
}

void APALBattleRoleActor::Init(UPALRoleData* RoleData, APALPlayerState* PlayerState, const FPALPosition3d& InOriginalPosition)
{
	RoleId = RoleData->RoleId;
    PlayerStatePrivate = PlayerState;
	OriginalPosition = InOriginalPosition;
	Position = InOriginalPosition;
	PreviousMP = PlayerStatePrivate->GetPlayerStateData()->PlayerRoles.MP[RoleId];
	// Load battle sprites for role
	SIZE_T BattleSpriteNum = PlayerState->GetRoleBattleSpriteNum(RoleId);
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetBattleRoleSprite(BattleSpriteNum);
	SpriteMeshComponent->SetSprite(Sprite);
	SetActorLocation(Position.toLocation());
	SetActorTickEnabled(true);
}

void APALBattleRoleActor::SetDefending(bool bInDefending)
{
	bDefending = bInDefending;
}

void APALBattleRoleActor::RestorePosition()
{
	Position = OriginalPosition;
}

void APALBattleRoleActor::HandleStatChangeDisplay(float DeltaTime)
{
	int16 IncreasedMP = static_cast<int16>(PlayerStatePrivate->GetPlayerStateData()->PlayerRoles.MP[RoleId]) - PreviousMP;
	PreviousMP = PlayerStatePrivate->GetPlayerStateData()->PlayerRoles.MP[RoleId];
	if (IncreasedMP > 0)
	{
		// Only show MP increasing
		double ZOffset = 67 * SQRT_3 / 2 * PIXEL_TO_UNIT;

		UTextRenderComponent* TextComponent = NewObject<UTextRenderComponent>(this, UStaticMeshComponent::StaticClass());
		TextComponent->SetupAttachment(RootComponent);
		TextComponent->SetRelativeLocation(FVector3d(0, 0, ZOffset));
		TextComponent->SetText(FText::FromString(FString::FromInt(IncreasedMP)));
		TextComponent->SetTextRenderColor(FColor(80, 184, 148, 255));
		TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
		TextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextTop);
		TextComponent->RegisterComponent();
		bHasText = true;
	}

	if (bHasText)
	{
		TArray<UTextRenderComponent*> TextRenderComponents;
		GetComponents(TextRenderComponents);
		for (UTextRenderComponent* TextRenderComponent : TextRenderComponents) {
			if (TextRenderComponent->GetRelativeLocation().Z > (67 + 16) * SQRT_3 / 2 * PIXEL_TO_UNIT)
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
}

// Called when the game starts or when spawned
void APALBattleRoleActor::BeginPlay()
{
	Super::BeginPlay();
	SpriteMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	SpriteMeshComponent->SetLayer(0);
}

// Called every frame
void APALBattleRoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bDefending) {
        Position = OriginalPosition;
    }
    //g_Battle.rgPlayer[i].iColorShift = 0;

    SIZE_T FrameNum;
    UPALPlayerStateData* PlayerStateData = PlayerStatePrivate->GetPlayerStateData();
    if (PlayerStateData->PlayerRoles.HP[RoleId] == 0)
    {
        if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Puppet] == 0)
        {
            FrameNum = 2; // dead
        }
        else
        {
            FrameNum = 0; // puppet
        }
    }
    else
    {
        if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] != 0 ||
            PlayerStatePrivate->IsRoleDying(RoleId))
        {
            FrameNum = 1;
        }
        else if (bDefending && !GetWorld()->GetAuthGameMode<APALBattleGameMode>()->IsEnemyCleared())
        {
            FrameNum = 3;
        }
        else
        {
            FrameNum = 0;
        }
    }

	FPALPosition3d DrawPosition = Position;
	if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Confused] != 0 &&
		PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] == 0 &&
		PlayerStateData->RoleStatus[RoleId][EPALStatus::Paralyzed] == 0 &&
		PlayerStateData->PlayerRoles.HP[RoleId] > 0)
	{
		// Player is confused
		DrawPosition.Z += FMath::RandRange(-SQRT_3, SQRT_3);
	}

	FVector3d RealLocation = DrawPosition.toLocation();
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

	SpriteMeshComponent->SetFrame(FrameNum);

	HandleStatChangeDisplay(DeltaTime);
}

