// Copyright (C) 2022 Meizhouxuanhan.


#include "PALEventObjectActor.h"
#include "PALCommon.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALScenePlayerController.h"
#include "PALScriptManager.h"
#include "PALMapManager.h"
#include "PAL.h"

// Sets default values
APALEventObjectActor::APALEventObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Sprite = nullptr;
	SpriteMeshComponent = nullptr;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	TimeFromLastFrame = 0;
	Speed = 36 * PIXEL_TO_UNIT / FRAME_TIME;
	LastInteractionTime = 0;
}


void APALEventObjectActor::Init(uint16 InEventObjectId)
{
	EventObjectId = InEventObjectId;
	EventObjectPrivate = &(GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->EventObjects[EventObjectId - 1]);
	SetActorLocation(GetPosition().toLocation());
	SIZE_T SpriteNum = static_cast<SIZE_T>(EventObjectPrivate->SpriteNum);
	if (SpriteNum != 0)
	{
		Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetSprite(SpriteNum); 
		SpriteMeshComponent = NewObject<UPALSpriteMeshComponent>(this, UPALSpriteMeshComponent::StaticClass());
		SpriteMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		SpriteMeshComponent->SetLocationOffset(FVector3d(0, 14 * PIXEL_TO_UNIT, 0));
		SpriteMeshComponent->SetSprite(Sprite);
		SpriteMeshComponent->SetLayer(EventObjectPrivate->Layer);
		SpriteMeshComponent->SetFrame(EventObjectPrivate->CurrentFrameNum);
		SpriteMeshComponent->RegisterComponent();
		EventObjectPrivate->SpriteFramesAuto = Sprite->GetFrameCount();
	}
}

bool APALEventObjectActor::IsNotHidden() const
{
	return EventObjectPrivate->State > 0;
}

bool APALEventObjectActor::IsBlocking() const
{
	return EventObjectPrivate->State >= EObjectState::ObjStateBlocker;
}

uint16 APALEventObjectActor::GetEventObjectId() const
{
	return EventObjectId;
}

FPALPosition3d APALEventObjectActor::GetPosition() const
{
	return FPALPosition3d(EventObjectPrivate->X, EventObjectPrivate->Y * 2, 0);
}

bool APALEventObjectActor::CanBeSearchedFrom(const FPALPosition3d& SearchPosition, EPALDirection Direction) const
{
	if (EventObjectPrivate->TriggerMode != ETriggerMode::SearchNormal
		&& EventObjectPrivate->TriggerMode != ETriggerMode::SearchNear
		&& EventObjectPrivate->TriggerMode != ETriggerMode::SearchFar)
	{
		return false;
	}
	
	if (LastInteractionTime + 0.05 > GetWorld()->GetTimeSeconds())
	{
		return false;
	}

	const FPALPosition3d& Position = GetPosition();
	double XOffset = (Direction == EPALDirection::North || Direction == EPALDirection::East) ? 
		(Position.X - SearchPosition.X) : -(Position.X - SearchPosition.X);
	double YOffset = (Direction == EPALDirection::East || Direction == EPALDirection::South) ?
		(Position.Y - SearchPosition.Y) : -(Position.Y - SearchPosition.Y);

	if (XOffset < 0 && !FMath::IsNearlyZero(XOffset)
		|| YOffset < 0 && !FMath::IsNearlyZero(YOffset)
		|| XOffset > 56 && !FMath::IsNearlyEqual(XOffset, 56) 
		|| YOffset > 56 && !FMath::IsNearlyEqual(YOffset, 56)
		|| (YOffset - XOffset) > 32 && !FMath::IsNearlyEqual(YOffset - XOffset, 32) 
		|| (YOffset - XOffset) < -32 && !FMath::IsNearlyEqual(YOffset - XOffset, -32))
	{
		return false;
	}

	if (XOffset + YOffset < 48 || FMath::IsNearlyEqual(XOffset + YOffset, 48))
	{
		return true;
	} 
	else if (EventObjectPrivate->TriggerMode == ETriggerMode::SearchNear)
	{
		return false;
	}

	if (XOffset + YOffset < 80 || FMath::IsNearlyEqual(XOffset + YOffset, 80))
	{
		return true;
	}
	else if (EventObjectPrivate->TriggerMode == ETriggerMode::SearchNormal)
	{
		return false;
	}

	return true;
}

void APALEventObjectActor::SearchBy(APALPlayerState* PlayerState)
{
	if (EventObjectPrivate->TriggerMode != ETriggerMode::SearchNormal
		&& EventObjectPrivate->TriggerMode != ETriggerMode::SearchNear
		&& EventObjectPrivate->TriggerMode != ETriggerMode::SearchFar)
	{
		return;
	}

	// Adjust direction/gesture for party members and the event object
	if (EventObjectPrivate->SpriteFrames * 4 > EventObjectPrivate->CurrentFrameNum)
	{
		EventObjectPrivate->CurrentFrameNum = 0; // use standing gesture
		EventObjectPrivate->Direction = (PlayerState->GetPlayerStateData()->PartyDirection + 2) % 4; // face the party

		// All party members should face the event object
		PlayerState->AlignPartyFacing();
	}

	LastInteractionTime = GetWorld()->GetTimeSeconds();

	PAL_DebugMsg(FString::FromInt(EventObjectId).Append(TEXT(" is searched")));
	// Execute the script
	GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(EventObjectPrivate->TriggerScript, EventObjectId, true);
}

void APALEventObjectActor::SetSpeed(float InSpeed)
{
	Speed = InSpeed;
}

void APALEventObjectActor::MoveTo(const FPALPosition3d& Position)
{
	const FPALPosition2d& Position2d = Position.to2d();
	EventObjectPrivate->X = Position2d.X;
	EventObjectPrivate->Y = Position2d.Y;
}

// Called when the game starts or when spawned
void APALEventObjectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APALEventObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorHiddenInGame(EventObjectPrivate->State == EObjectState::ObjStateHidden || EventObjectPrivate->VanishTime > 0 
		|| EventObjectPrivate->State < 0);
	
	FVector3d RealLocation = GetPosition().toLocation();
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
			Step *= Speed * DeltaTime;
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
	
	
	TimeFromLastFrame += DeltaTime;
	if (TimeFromLastFrame > FRAME_TIME)
	{
		TimeFromLastFrame = 0;
		if (EventObjectPrivate->VanishTime != 0)
		{
			EventObjectPrivate->VanishTime += ((EventObjectPrivate->VanishTime < 0) ? 1 : -1);
		}

		if (EventObjectPrivate->VanishTime == 0)
		{
			if (EventObjectPrivate->State > 0 && EventObjectPrivate->TriggerMode >= ETriggerMode::TouchNear)
			{
				// This event object can be triggered without manually exploring
				APALScenePlayerController* PlayerController = Cast<APALScenePlayerController>(GetGameInstance<UPALGameInstance>()
					->GetMainPlayer()->GetPlayerController(nullptr));
				FPALPosition2d Position2d = PlayerController->GetPartyPosition().to2d();
				if (FMath::Abs(Position2d.X - EventObjectPrivate->X) + FMath::Abs(Position2d.Y - EventObjectPrivate->Y) * 2
					< (EventObjectPrivate->TriggerMode - ETriggerMode::TouchNear) * 32 + 16)
				{
					// Player is in the trigger zone.

					if (EventObjectPrivate->SpriteFrames)
					{
						// The sprite has multiple frames. Try to adjust the direction.
						EventObjectPrivate->CurrentFrameNum = 0;

						if (Position2d.X - EventObjectPrivate->X > 0)
						{
							EventObjectPrivate->Direction = ((Position2d.Y - EventObjectPrivate->Y * 2 > 0) ? EPALDirection::East : EPALDirection::North);
						}
						else
						{
							EventObjectPrivate->Direction = ((Position2d.Y - EventObjectPrivate->Y * 2 > 0) ? EPALDirection::South : EPALDirection::West);
						}
					}

					// Execute the script.
					GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(EventObjectPrivate->TriggerScript, EventObjectId, true);
				}
			}
		}

		if (EventObjectPrivate->State > 0 && EventObjectPrivate->VanishTime == 0)
		{
			uint16 ScriptEntry = EventObjectPrivate->AutoScript;
			if (ScriptEntry != 0 && LastInteractionTime + 1. < GetWorld()->GetTimeSeconds())
			{
				GetWorld()->GetSubsystem<UPALScriptManager>()->RunAutoScript(ScriptEntry, EventObjectId, true);
				EventObjectPrivate->AutoScript = ScriptEntry;
			}
		}

		// Check if the player is in the way
		if (EventObjectPrivate->State >= EObjectState::ObjStateBlocker && EventObjectPrivate->SpriteNum != 0)
		{
			APALScenePlayerController* PlayerController = Cast<APALScenePlayerController>(GetGameInstance<UPALGameInstance>()
				->GetMainPlayer()->GetPlayerController(nullptr));
			const FPALPosition3d& PartyPosition = PlayerController->GetPartyPosition();
			if (FMath::Abs(EventObjectPrivate->X - PartyPosition.X) + FMath::Abs(EventObjectPrivate->Y * 2 - PartyPosition.Y) <= 12) {
				// Player is in the way, try to move a step
				uint16 Dir = (EventObjectPrivate->Direction + 1) % 4;
				for (SIZE_T i = 0; i < 4; i++)
				{
					double X = PartyPosition.X;
					double Y = PartyPosition.Y;

					X += ((Dir == EPALDirection::West || Dir == EPALDirection::South) ? -16 : 16);
					Y += ((Dir == EPALDirection::West || Dir == EPALDirection::North) ? -16 : 16);

					FPALPosition3d NewPosition(X, Y, PartyPosition.Z);
					if (!GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(NewPosition, true))
					{
						if (!PlayerController->IsControlledByGame())
						{
							PlayerController->MainRoleMoveTo(NewPosition);
						}
						break;
					}

					Dir = (Dir + 1) % 4;
				}
			}
		}
		
		// visual
		uint16 Frame = EventObjectPrivate->CurrentFrameNum;
		if (EventObjectPrivate->SpriteFrames == 3)
		{
			// walking character
			if (Frame == 2)
			{
				Frame = 0;
			}

			if (Frame == 3)
			{
				Frame = 2;
			}
		}

		if (SpriteMeshComponent)
		{
			SpriteMeshComponent->SetLayer(EventObjectPrivate->Layer);
			SpriteMeshComponent->SetFrame(EventObjectPrivate->Direction * EventObjectPrivate->SpriteFrames + Frame);
		}
	}
}

