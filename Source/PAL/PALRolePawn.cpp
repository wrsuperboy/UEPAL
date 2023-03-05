// Copyright (C) 2022 Meizhouxuanhan.


#include "PALRolePawn.h"
#include "EngineUtils.h"
#include "Algo/Sort.h"
#include "PALCommon.h"
#include "PALPlayerState.h"
#include "PALPlayerController.h"
#include "PALMapManager.h"

constexpr float DefaultSpeed = 24 * PIXEL_TO_UNIT / FRAME_TIME;

// Sets default values
APALRolePawn::APALRolePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SpriteMeshComponent = CreateDefaultSubobject<UPALSpriteMeshComponent>(TEXT("SpriteMeshComponent"));
	KeyInputDirection = EPALDirection::Unknown;
	PreviousKeyInputDirection = EPALDirection::Unknown;
	JoystickInputDirection = EPALDirection::Unknown;
	bInputDirectionEnabled = true;
	Speed = DefaultSpeed;
	LastInputTime = 0;
	ThisStepFrame = 0;
	ThisStepFrameTime = 0;
	bWalking = false;
	bAtEase = false;
	StandTime = 0;
	SetActorTickEnabled(false);
}

void APALRolePawn::Init(UPALRoleData* RoleData, APALPlayerState* InPlayerState, bool bInFollower)
{
	RoleDataPrivate = RoleData;
	PlayerStatePrivate = InPlayerState;
	bFollower = bInFollower;
	// Load player sprite
	SIZE_T SpriteNum = bInFollower ? RoleDataPrivate->RoleId : PlayerStatePrivate->GetPlayerStateData()->PlayerRoles.SpriteNum[RoleDataPrivate->RoleId];
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetSprite(SpriteNum);
	SpriteMeshComponent->SetSprite(Sprite);
	SpriteMeshComponent->SetLocationOffset(FVector3d(0, 8 * PIXEL_TO_UNIT, 0));
	SetActorLocation(RoleDataPrivate->Position.toLocation());
	SetActorTickEnabled(true);
}

SIZE_T APALRolePawn::GetRoleId() const
{
	return RoleDataPrivate->RoleId;
}

bool APALRolePawn::IsFollower() const
{
	return bFollower;
}

void APALRolePawn::UpdateSprite()
{
	SIZE_T SpriteNum = bFollower ? RoleDataPrivate->RoleId : PlayerStatePrivate->GetPlayerStateData()->PlayerRoles.SpriteNum[RoleDataPrivate->RoleId];
	UPALSprite* Sprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetSprite(SpriteNum);
	SpriteMeshComponent->SetSprite(Sprite);
	SpriteMeshComponent->SetFrame(RoleDataPrivate->FrameNum);
}

const FPALPosition3d& APALRolePawn::GetPosition() const
{
	return RoleDataPrivate->Position;
}

void APALRolePawn::UpKeyDown()
{
	if (KeyInputDirection != EPALDirection::North)
	{
		PreviousKeyInputDirection = KeyInputDirection;
		KeyInputDirection = EPALDirection::North;
	}
}

void APALRolePawn::UpKeyUp()
{
	if (KeyInputDirection == EPALDirection::North)
	{
		KeyInputDirection = PreviousKeyInputDirection;
	}
	PreviousKeyInputDirection = EPALDirection::Unknown;
}

void APALRolePawn::DownKeyDown()
{
	if (KeyInputDirection != EPALDirection::South)
	{
		PreviousKeyInputDirection = KeyInputDirection;
		KeyInputDirection = EPALDirection::South;
	}
}

void APALRolePawn::DownKeyUp()
{
	if (KeyInputDirection == EPALDirection::South)
	{
		KeyInputDirection = PreviousKeyInputDirection;
	}
	PreviousKeyInputDirection = EPALDirection::Unknown;
}

void APALRolePawn::LeftKeyDown()
{
	if (KeyInputDirection != EPALDirection::West)
	{
		PreviousKeyInputDirection = KeyInputDirection;
		KeyInputDirection = EPALDirection::West;
	}
}

void APALRolePawn::LeftKeyUp()
{
	if (KeyInputDirection == EPALDirection::West)
	{
		KeyInputDirection = PreviousKeyInputDirection;
	}
	PreviousKeyInputDirection = EPALDirection::Unknown;
}

void APALRolePawn::RightKeyDown()
{
	if (KeyInputDirection != EPALDirection::East)
	{
		PreviousKeyInputDirection = KeyInputDirection;
		KeyInputDirection = EPALDirection::East;
	}
}

void APALRolePawn::RightKeyUp()
{
	if (KeyInputDirection == EPALDirection::East)
	{
		KeyInputDirection = PreviousKeyInputDirection;
	}
	PreviousKeyInputDirection = EPALDirection::Unknown;
}


void APALRolePawn::JoystickAxisInput(float Value)
{
	float X = InputComponent->GetAxisValue("Joystick X-Axis");
	float Y = InputComponent->GetAxisValue("Joystick Y-Axis");
	if (FVector2D(X, Y).Length() < 0.3)
	{
		JoystickInputDirection = EPALDirection::Unknown;
		return;
	}
	if (X > 0)
	{
		if (Y > 0)
		{
			JoystickInputDirection = EPALDirection::North;
		}
		else
		{
			JoystickInputDirection = EPALDirection::East;
		}
	}
	else
	{
		if (Y > 0)
		{
			JoystickInputDirection = EPALDirection::West;
		}
		else
		{
			JoystickInputDirection = EPALDirection::South;
		}
	}
}

void APALRolePawn::SetWalking()
{
	bWalking = true;
}

void APALRolePawn::StopWalking()
{
	if (bWalking)
	{
		ThisStepFrame = (ThisStepFrame + 1) % 4;
		SIZE_T StepFrame = 0;
		if (ThisStepFrame & 1)
		{
			StepFrame = bFollower ? (3 - (ThisStepFrame + 1) / 2) : (ThisStepFrame + 1) / 2;
		}

		UPALPlayerStateData* PlayerStateData = PlayerStatePrivate->GetPlayerStateData();
		if (PlayerStateData->PlayerRoles.WalkFrames[RoleDataPrivate->RoleId] == 4 && !bFollower)
		{
			// TODO
			RoleDataPrivate->FrameNum = PlayerStateData->PartyDirection * 4 + ThisStepFrame;
		}
		else
		{
			RoleDataPrivate->FrameNum = PlayerStateData->PartyDirection * 3 + StepFrame;
		}
		ThisStepFrameTime = 0;
	}
	bWalking = false;
}

void APALRolePawn::SetAtEase(bool bInAtEase)
{
	bAtEase = bInAtEase;
}

void APALRolePawn::SetSpeed(float NewSpeed)
{
	Speed = NewSpeed;
}

void APALRolePawn::ResetSpeed()
{
	Speed = DefaultSpeed;
}

void APALRolePawn::Search()
{
	if (!bInputDirectionEnabled)
	{
		return;
	}
	PAL_DebugMsg(TEXT("Seaching"));

	EPALDirection Direction = PlayerStatePrivate->GetPlayerStateData()->PartyDirection;

	// Get search location
	const FPALPosition3d& Position = GetPosition();

	// Loop through all event objects
	TArray<APALEventObjectActor*> SearchEventObjects;
	for (TActorIterator<APALEventObjectActor> It(GetWorld(), APALEventObjectActor::StaticClass()); It; ++It)
	{
		APALEventObjectActor* EventObjectActor = *It;
		if (EventObjectActor->IsValidLowLevelFast())
		{
			if (EventObjectActor->IsNotHidden() && EventObjectActor->CanBeSearchedFrom(Position, Direction))
			{
				SearchEventObjects.Add(EventObjectActor);
			}
		}
	}

	if (SearchEventObjects.IsEmpty())
	{
		return;
	}

	Algo::Sort(SearchEventObjects, [&Position](APALEventObjectActor*& a, APALEventObjectActor*& b) {
		return FVector::Dist(a->GetPosition().toLocation(), Position.toLocation()) < FVector::Dist(b->GetPosition().toLocation(), Position.toLocation());
		});

	SearchEventObjects[0]->SearchBy(PlayerStatePrivate);
}

void APALRolePawn::MoveByInput()
{
	// Has user pressed one of the arrow keys?
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (bInputDirectionEnabled && LastInputTime + FRAME_TIME < CurrentTime)
	{
		EPALDirection InputDirection = JoystickInputDirection == EPALDirection::Unknown ? KeyInputDirection : JoystickInputDirection;
		if (InputDirection != EPALDirection::Unknown)
		{
			LastInputTime = CurrentTime;

			double XOffset = ((InputDirection == EPALDirection::West || InputDirection == EPALDirection::South) ? -16 : 16);
			double YOffset = ((InputDirection == EPALDirection::West || InputDirection == EPALDirection::North) ? -16 : 16);
			double ZOffset = 0; // TODO

			const FPALPosition3d& SourcePosition = GetPosition();
			FPALPosition3d TargetPosition(SourcePosition.X + XOffset,
				SourcePosition.Y + YOffset,
				SourcePosition.Z + ZOffset);

			UPALPlayerStateData* PlayerStateData = PlayerStatePrivate->GetPlayerStateData();
			PlayerStateData->PartyDirection = InputDirection;

			// Check for obstacles on the destination location
			if (!GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(TargetPosition, true))
			{
				bWalking = true;
				bAtEase = false;
				// Player will actually move. Store trail.
				FTrail NewTrail;
				NewTrail.Direction = InputDirection;
				NewTrail.Position = SourcePosition;
				PlayerStateData->Trails.Insert(NewTrail, 0);
				PlayerStateData->Trails.SetNum(MAX_PLAYER_ROLES);
				RoleDataPrivate->Position = TargetPosition;
				Cast<APALPlayerController>(Controller)->OnMainRoleWalk();
			}
		}
	}
}

// Called when the game starts or when spawned
void APALRolePawn::BeginPlay()
{
	Super::BeginPlay();
	SpriteMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	SpriteMeshComponent->SetLayer(0);
}

// Called every frame
void APALRolePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move
	if (IsPawnControlled())
	{
		MoveByInput();
	}

	FVector3d RealLocation = RoleDataPrivate->Position.toLocation();
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
				StandTime = 0;
				SetActorLocation(Destination);
			}
		}
		else
		{
			// The role has stopped. Update walking status after a while.
			if (bWalking)
			{
				StandTime += DeltaTime;
				if (StandTime > FRAME_TIME)
				{
					bWalking = false;
					Speed = DefaultSpeed;
					StandTime = 0;
				}
			}
		}
	}

	// Walk gesture
	ThisStepFrameTime += DeltaTime;
	if (ThisStepFrameTime > FRAME_TIME)
	{
		UPALPlayerStateData* PlayerStateData = PlayerStatePrivate->GetPlayerStateData();
		EPALDirection Direction;
		if (IsPawnControlled())
		{
			Direction = PlayerStateData->PartyDirection;
		}
		else
		{
			Direction = PlayerStateData->Trails[1].Direction;
		}

		if (bWalking)
		{
			ThisStepFrame = (ThisStepFrame + 1) % 4;
			SIZE_T StepFrame = 0;
			if (ThisStepFrame & 1)
			{
				StepFrame = bFollower ? (3 - (ThisStepFrame + 1) / 2) : (ThisStepFrame + 1) / 2;
			}

			if (PlayerStateData->PlayerRoles.WalkFrames[RoleDataPrivate->RoleId] == 4 && !bFollower)
			{
				// TODO
				RoleDataPrivate->FrameNum = Direction * 4 + ThisStepFrame;
			}
			else
			{
				RoleDataPrivate->FrameNum = Direction * 3 + StepFrame;
			}
			
			ThisStepFrameTime = 0;
		}
		else if (!bAtEase)
		{
			SIZE_T i = PlayerStateData->PlayerRoles.WalkFrames[RoleDataPrivate->RoleId];
			if (i == 0)
			{
				i = 3;
			}
			RoleDataPrivate->FrameNum = Direction * i;
			ThisStepFrame &= 2;
			ThisStepFrame ^= 2;
		}
	}

	SpriteMeshComponent->SetFrame(RoleDataPrivate->FrameNum);
}

// Called to bind functionality to input
void APALRolePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("Up", IE_Pressed, this, &APALRolePawn::UpKeyDown);
	InputComponent->BindAction("Up", IE_Released, this, &APALRolePawn::UpKeyUp);

	InputComponent->BindAction("Down", IE_Pressed, this, &APALRolePawn::DownKeyDown);
	InputComponent->BindAction("Down", IE_Released, this, &APALRolePawn::DownKeyUp);

	InputComponent->BindAction("Left", IE_Pressed, this, &APALRolePawn::LeftKeyDown);
	InputComponent->BindAction("Left", IE_Released, this, &APALRolePawn::LeftKeyUp);

	InputComponent->BindAction("Right", IE_Pressed, this, &APALRolePawn::RightKeyDown);
	InputComponent->BindAction("Right", IE_Released, this, &APALRolePawn::RightKeyUp);

	InputComponent->BindAxis("Joystick X-Axis", this, &APALRolePawn::JoystickAxisInput);
	InputComponent->BindAxis("Joystick Y-Axis");

	InputComponent->BindAction("Search", IE_Pressed, this, &APALRolePawn::Search);
}

void APALRolePawn::DisableInput(APlayerController* PlayerController)
{
	bInputDirectionEnabled = false;
}

void APALRolePawn::EnableInput(APlayerController* PlayerController)
{
	bInputDirectionEnabled = true;
}
