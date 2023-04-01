// Copyright (C) 2022 Meizhouxuanhan.


#include "PALPlayerController.h"
#include "PALPlayerCameraManager.h"
#include "PALPlayerState.h"
#include "PALPlayerStateData.h"
#include "PALSceneCameraActor.h"
#include "PALGameState.h"
#include "PALInGameMenu.h"
#include "PALItemSelectMenu.h"
#include "PALScriptManager.h"
#include "PALSaveSlotMenu.h"
#include "PALSceneGameMode.h"
#include "PALMapManager.h"
#include "PALRoleStatusMenu.h"
#include "PALInGameUseMagicMenu.h"
#include "PALEquipItemMenu.h"
#include "PALItemUseMenu.h"
#include "PALBuyMenu.h"
#include "PAL.h"

APALPlayerController::APALPlayerController() : Super() {
	PlayerCameraManagerClass = APALPlayerCameraManager::StaticClass();
	bAutoManageActiveCameraTarget = false;
}

void APALPlayerController::ReloadRoles()
{
	for (APALRolePawn* RolePawn : RolePawns) {
		RolePawn->SetActorTickEnabled(false);
		RolePawn->Destroy();
	}
	RolePawns.Empty();
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	APALRolePawn* MainRolePawn = nullptr;
	for (UPALRoleData* PartyMember : PlayerStateData->Party)
	{
		// Load the players
		APALRolePawn* RolePawn = GetWorld()->SpawnActor<APALRolePawn>();
		RolePawn->Init(PartyMember, GetPlayerState<APALPlayerState>());
		RolePawns.Add(RolePawn);
		if (!CameraActor)
		{
			CameraActor = GetWorld()->SpawnActor<APALSceneCameraActor>();
			SetViewTarget(CameraActor);
		}
		if (!MainRolePawn)
		{
			MainRolePawn = RolePawn;
			Possess(MainRolePawn);
			CameraActor->SetActorLocation(FVector::Zero());
			CameraActor->AttachToActor(MainRolePawn, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		}
	}

	for (UPALRoleData* Follower : PlayerStateData->Follow)
	{
		// Load the follower
		APALRolePawn* RolePawn = GetWorld()->SpawnActor<APALRolePawn>();
		RolePawn->Init(Follower, GetPlayerState<APALPlayerState>(), true);
		RolePawns.Add(RolePawn);
	}
}

void APALPlayerController::ReloadRoleSprites()
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->UpdateSprite();
	}
}

APALRolePawn* APALPlayerController::GetRolePawn(const SIZE_T RoleId) const
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		if (RolePawn->GetRoleId() == RoleId)
		{
			return RolePawn;
		}
	}
	PAL_DebugMsg(FString(TEXT("Cannot find role pawn for role ID ")).Append(FString::FromInt(RoleId)));
	return nullptr;
}

const FPALPosition3d& APALPlayerController::GetPartyPosition() const
{
	check(!RolePawns.IsEmpty());
	return RolePawns[0]->GetPosition();
}

FPALPosition3d APALPlayerController::GetViewport() const
{
	AActor* ViewTarget = GetViewTarget();
	if (ViewTarget != CameraActor)
	{
		const FVector3d& Location = ViewTarget->GetActorLocation();
		return FPALPosition3d(Location.X / PIXEL_TO_UNIT, Location.Y / PIXEL_TO_UNIT, Location.Z / PIXEL_TO_UNIT);
	}
	
	return GetPartyPosition();
}

void APALPlayerController::ControllByGame()
{
	if (GetPawn())
	{
		GetPawn()->DisableInput(this);
	}
}

void APALPlayerController::ReleaseControllFromGame()
{
	if (GetPawn())
	{
		GetPawn()->EnableInput(this);
	}
}

bool APALPlayerController::IsControlledByGame() const
{
	if (RolePawns.IsEmpty())
	{
		return true;
	}

	return !RolePawns[0]->IsInputEnabled();
}

void APALPlayerController::RemoveAllPartyRoles()
{
	for (TArray<APALRolePawn*>::TIterator It = RolePawns.CreateIterator(); It; ++It)
	{
		if (!(*It)->IsFollower())
		{
			(*It)->DisableInput(this);
			(*It)->Destroy();
			It.RemoveCurrent();
		}
	}
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	PlayerStateData->Party.Empty();
}

void APALPlayerController::RemoveAllFollowerRoles()
{
	for (TArray<APALRolePawn*>::TIterator It = RolePawns.CreateIterator(); It; ++It)
	{
		if ((*It)->IsFollower())
		{
			(*It)->DisableInput(this);
			(*It)->Destroy();
			It.RemoveCurrent();
		}
	}
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	PlayerStateData->Follow.Empty();
}

void APALPlayerController::AddPartyRole(const SIZE_T RoleId)
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	UPALRoleData* NewPartyMember = NewObject<UPALRoleData>();
	NewPartyMember->RoleId = RoleId;
	PlayerStateData->Party.Add(NewPartyMember);
	APALRolePawn* RolePawn = GetWorld()->SpawnActor<APALRolePawn>();
	RolePawn->Init(NewPartyMember, GetPlayerState<APALPlayerState>());
	RolePawns.Add(RolePawn);
	if (!CameraActor)
	{
		CameraActor = GetWorld()->SpawnActor<APALSceneCameraActor>();
		SetViewTarget(CameraActor);
	}
	if (PlayerStateData->Party.Num() == 1)
	{
		Possess(RolePawn);
		CameraActor->SetActorLocation(FVector::Zero());
		CameraActor->AttachToActor(RolePawn, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	}
}

void APALPlayerController::AddFollowerRole(const SIZE_T RoleId)
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	UPALRoleData* NewPartyMember = NewObject<UPALRoleData>();
	NewPartyMember->RoleId = RoleId;
	PlayerStateData->Follow.Add(NewPartyMember);
	APALRolePawn* RolePawn = GetWorld()->SpawnActor<APALRolePawn>();
	RolePawn->Init(NewPartyMember, GetPlayerState<APALPlayerState>(), true);
	RolePawns.Add(RolePawn);

	// Update the position and gesture for the follower
	SIZE_T TrailIndex = RolePawns.Num();
	if (!PlayerStateData->Trails.IsValidIndex(TrailIndex))
	{
		TrailIndex = PlayerStateData->Trails.Num() - 1;
	}
	NewPartyMember->Position = PlayerStateData->Trails[TrailIndex].Position;
	NewPartyMember->FrameNum = PlayerStateData->Trails[TrailIndex].Direction * 3;
}

void APALPlayerController::PartyMoveTo(const FPALPosition3d& Position)
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (!PlayerStateData->Party.IsEmpty())
	{
		FTrail NewTrail;
		NewTrail.Direction = PlayerStateData->PartyDirection;
		NewTrail.Position = Position;
		PlayerStateData->Trails.Insert(NewTrail, 0);
		PlayerStateData->Trails.SetNum(MAX_PLAYER_ROLES);

		const FPALPosition3d& PartyPosition = GetPartyPosition();
		double XOffset = Position.X - PartyPosition.X;
		double YOffset = Position.Y - PartyPosition.Y;
		double ZOffset = Position.Z - PartyPosition.Z;
		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			PartyMember->Position.X += XOffset;
			PartyMember->Position.Y += YOffset;
			PartyMember->Position.Z += ZOffset;
		}
	}
}

void APALPlayerController::MainRoleMoveTo(const FPALPosition3d& Position)
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	PlayerStateData->Party[0]->Position = Position;
}

void APALPlayerController::PartyWalkTo(const FPALPosition3d& Position)
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (!PlayerStateData->Party.IsEmpty())
	{
		FTrail NewTrail;
		NewTrail.Direction = PlayerStateData->PartyDirection;
		NewTrail.Position = Position;
		PlayerStateData->Trails.Insert(NewTrail, 0);
		PlayerStateData->Trails.SetNum(MAX_PLAYER_ROLES);
		PlayerStateData->Party[0]->Position = Position;
		RolePawns[0]->SetWalking();
		RolePawns[0]->SetAtEase(false);
		OnMainRoleWalk();
	}
}

void APALPlayerController::OnMainRoleWalk()
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	SIZE_T PartyMemberCount = PlayerStateData->Party.Num();
	for (SIZE_T RoleIndex = 1; RoleIndex < PartyMemberCount; RoleIndex++)
	{
		PlayerStateData->Party[RoleIndex]->Position = PlayerStateData->Trails[1].Position;

		if (RoleIndex == 2)
		{
			PlayerStateData->Party[RoleIndex]->Position.X +=
				(PlayerStateData->Trails[1].Direction == EPALDirection::East || PlayerStateData->Trails[1].Direction == EPALDirection::West) ? -16 : 16;
			PlayerStateData->Party[RoleIndex]->Position.Y += 16;
		}
		else
		{
			PlayerStateData->Party[RoleIndex]->Position.X +=
				(PlayerStateData->Trails[1].Direction == EPALDirection::West || PlayerStateData->Trails[1].Direction == EPALDirection::South) ? 16 : -16;
			PlayerStateData->Party[RoleIndex]->Position.Y +=
				(PlayerStateData->Trails[1].Direction == EPALDirection::West || PlayerStateData->Trails[1].Direction == EPALDirection::North) ? 16 : -16;
		}

		// Adjust the position if there is obstacle
		if (GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(PlayerStateData->Party[RoleIndex]->Position, true))
		{
			PlayerStateData->Party[RoleIndex]->Position = PlayerStateData->Trails[1].Position;
		}

		RolePawns[RoleIndex]->SetWalking();
		RolePawns[RoleIndex]->SetAtEase(false);
	}
	SIZE_T FollowerCount = PlayerStateData->Follow.Num();
	for (SIZE_T i = 0; i < FollowerCount; i++)
	{
		// Update the position and gesture for the follower
		PlayerStateData->Follow[i]->Position = PlayerStateData->Trails[3 + i].Position;
		RolePawns[PartyMemberCount + i]->SetWalking();
		RolePawns[PartyMemberCount + i]->SetAtEase(false);
	}
	CameraRestoreNormal();
}

void APALPlayerController::PartyAtEase(bool bAtEase)
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->SetAtEase(bAtEase);
	}
}

void APALPlayerController::RoleAtEase(SIZE_T RoleId, bool bAtEase)
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		if (RolePawn->GetRoleId() == RoleId)
		{
			RolePawn->SetAtEase(bAtEase);
			break;
		}
	}
}

void APALPlayerController::RoleStopWalking(SIZE_T RoleId)
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		if (RolePawn->GetRoleId() == RoleId)
		{
			RolePawn->StopWalking();
			break;
		}
	}
}

void APALPlayerController::PartySetWalking()
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->SetWalking();
	}
}

void APALPlayerController::PartyStopWalking()
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->StopWalking();
	}
}

void APALPlayerController::PartyResetSpeed()
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->ResetSpeed();
	}
}

void APALPlayerController::PartySetSpeed(float Speed)
{
	for (APALRolePawn* RolePawn : RolePawns)
	{
		RolePawn->SetSpeed(Speed);
	}
}

void APALPlayerController::PartyMoveClose()
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (PlayerStateData->Party.IsEmpty())
	{
		return;
	}

	for (FTrail& Trail : PlayerStateData->Trails)
	{
		Trail.Direction = PlayerStateData->PartyDirection;
		Trail.Position = PlayerStateData->Party[0]->Position;
	}
	for (SIZE_T i = 1; i < PlayerStateData->Party.Num(); i++)
	{
		PlayerStateData->Party[i]->Position = PlayerStateData->Party[0]->Position;
		PlayerStateData->Party[i]->Position.Y--;
	}
	PartyStopWalking();
}

void APALPlayerController::OnEscape()
{
	if (!GetWorld()->GetGameState<APALGameState>()->IsInMainGame() && CurrentMenu)
	{
		CurrentMenu->GoBack();
		return;
	}

	if (!GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->bInBattle)
	{
		if (!CurrentMenu)
		{
			if (GetWorld()->GetGameState<APALGameState>()->IsInMainGame())
			{
				CurrentMenu = CreateWidget<UPALInGameMenu>(this, UPALInGameMenu::StaticClass());
				CurrentMenu->AddToViewport(0);
				SetShowMouseCursor(true);
				CurrentMenu->SetFocus();
				PAL_DebugMsg("Open in game menu");
			}
		}
		else
		{
			bool bSuccess = CurrentMenu->GoBack();
			if (!bSuccess)
			{
				CurrentMenu->RemoveFromParent();
				SetShowMouseCursor(false);
				CurrentMenu = nullptr;
			}
		}
	}
}

void APALPlayerController::CloseCurrentMenu()
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
		CurrentMenu = nullptr;
	}
}

void APALPlayerController::ControllOpeningMenu(UPALOpeningMenu* OpeningMenu)
{
	check(!GetWorld()->GetGameState<APALGameState>()->IsInMainGame());
	CurrentMenu = OpeningMenu;
	OpeningMenu->SetFocus();
	SetShowMouseCursor(true);
}

void APALPlayerController::GameRoleStatus()
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
	}

	UPALRoleStatusMenu* RoleStatusMenu = CreateWidget<UPALRoleStatusMenu>(this, UPALRoleStatusMenu::StaticClass());
	CurrentMenu = RoleStatusMenu;
	CurrentMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Open role status menu");
}

void APALPlayerController::GameUseItem()
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);	
	}

	UPALItemSelectMenu* UseItemMenu = CreateWidget<UPALItemSelectMenu>(this, UPALItemSelectMenu::StaticClass());
	UseItemMenu->Init(GetPlayerState<APALPlayerState>(), EPALItemFlag::ItemFlagUsable);
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(this);
	TWeakObjectPtr<UPALItemSelectMenu> UseItemMenuWP(UseItemMenu);
	UseItemMenu->OnMenuItemSelected.AddWeakLambda(this, [PlayerControllerWP, UseItemMenuWP](int16 Item) {
		if (PlayerControllerWP.IsValid())
		{
			APALPlayerController* PlayerController = PlayerControllerWP.Get();
			APALPlayerState* PlayerState = PlayerController->GetPlayerState<APALPlayerState>();
			UPALGameStateData* GameStateData = PlayerController->GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
			if (!(GameStateData->Objects[Item].Item.Flags & EPALItemFlag::ItemFlagApplyToAll))
			{
				//// Select the player to use the item on
				UPALItemUseMenu* ItemUseMenu = CreateWidget<UPALItemUseMenu>(PlayerController, UPALItemUseMenu::StaticClass());
				ItemUseMenu->Init(PlayerState, Item);
				ItemUseMenu->AddToViewport(1);
				UseItemMenuWP->Takeover(ItemUseMenu);
			}
			else
			{
				PlayerState->UseItemToAll(Item);
				if (PlayerController->CurrentMenu && PlayerController->CurrentMenu->GetClass() == UPALItemSelectMenu::StaticClass())
				{
					PlayerController->CurrentMenu->RemoveFromParent();
					PlayerController->SetShowMouseCursor(false);
				}
			}
		}
		});
	CurrentMenu = UseItemMenu;
	CurrentMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Open use item menu");
}

void APALPlayerController::GameUseMagic(SIZE_T RoleId)
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
	}

	UPALInGameUseMagicMenu* UseMagicMenu = CreateWidget<UPALInGameUseMagicMenu>(this, UPALInGameUseMagicMenu::StaticClass());
	CurrentMenu = UseMagicMenu;
	UseMagicMenu->SelectRole(RoleId);
	CurrentMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Open in game use magic menu");
}

void APALPlayerController::GameEquipItem()
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
	}

	UPALItemSelectMenu* EquipItemMenu = CreateWidget<UPALItemSelectMenu>(this, UPALItemSelectMenu::StaticClass());
	EquipItemMenu->Init(GetPlayerState<APALPlayerState>(), EPALItemFlag::ItemFlagEquipable);
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(this);
	TWeakObjectPtr<UPALItemSelectMenu> EquipItemMenuWP(EquipItemMenu);
	EquipItemMenu->OnMenuItemSelected.AddWeakLambda(this, [PlayerControllerWP, EquipItemMenuWP](int16 Item) {
		if (PlayerControllerWP.IsValid() && EquipItemMenuWP.IsValid())
		{
			UPALEquipItemMenu* EquipItemMenu = CreateWidget<UPALEquipItemMenu>(PlayerControllerWP.Get(), UPALEquipItemMenu::StaticClass());
			EquipItemMenu->Init(Item);
			EquipItemMenu->AddToViewport(1);
			EquipItemMenuWP->Takeover(EquipItemMenu);
		}
		});
	CurrentMenu = EquipItemMenu;
	CurrentMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Open equip item menu");
}

void APALPlayerController::Buy(uint16 StoreNum)
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
	}

	UPALBuyMenu* BuyMenu = CreateWidget<UPALBuyMenu>(this, UPALBuyMenu::StaticClass());
	BuyMenu->Init(StoreNum); 
	CurrentMenu = BuyMenu;
	CurrentMenu->AddToViewport(0);
	ControllByGame();
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Buy in store");
}

void APALPlayerController::Sell()
{
	if (CurrentMenu)
	{
		CurrentMenu->RemoveFromParent();
		SetShowMouseCursor(false);
	}

	UPALItemSelectMenu* SellItemMenu = CreateWidget<UPALItemSelectMenu>(this, UPALItemSelectMenu::StaticClass());
	SellItemMenu->Init(GetPlayerState<APALPlayerState>(), EPALItemFlag::ItemFlagSellable);
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(this);
	TWeakObjectPtr<UPALItemSelectMenu> SellItemMenuWP(SellItemMenu);
	SellItemMenu->OnMenuItemSelected.AddWeakLambda(this, [PlayerControllerWP, SellItemMenuWP](int16 Item) {
		if (PlayerControllerWP.IsValid() && SellItemMenuWP.IsValid())
		{
			
			UPALConfirmMenu* ConfirmMenu = CreateWidget<UPALConfirmMenu>(PlayerControllerWP.Get(), UPALConfirmMenu::StaticClass());
			ConfirmMenu->AddToViewport(1);
			TWeakObjectPtr<UPALConfirmMenu> ConfirmMenuWP(ConfirmMenu);
			ConfirmMenu->OnConfirm.AddWeakLambda(PlayerControllerWP.Get(), [PlayerControllerWP, SellItemMenuWP, ConfirmMenuWP, Item](bool bYesOrNo) {
				if (bYesOrNo)
				{
					if (PlayerControllerWP.IsValid())
					{
						UPALGameStateData* GameStateData = PlayerControllerWP->GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
						uint16 Price = GameStateData->Objects[Item].Item.Price;
						APALPlayerController* PlayerController = PlayerControllerWP.Get();
						APALPlayerState* PlayerState = PlayerController->GetPlayerState<APALPlayerState>();
						if (PlayerState->AddItemToInventory(Item, -1))
						{
							PlayerState->GetPlayerStateData()->Cash += Price / 2;
						}
					}
				}
				ConfirmMenuWP->RemoveFromParent();
				if (SellItemMenuWP.IsValid())
				{
					SellItemMenuWP->Refresh();
				}
				});
			SellItemMenuWP->Takeover(ConfirmMenu);
		}
		});
	CurrentMenu = SellItemMenu;
	CurrentMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	CurrentMenu->SetFocus();
	PAL_DebugMsg("Open sell item menu");
}

void APALPlayerController::CameraRestoreNormal()
{
	AActor* ViewTarget = GetViewTarget();
	if (ViewTarget != CameraActor)
	{
		SetViewTarget(CameraActor);
		ViewTarget->Destroy();
	}
}

void APALPlayerController::CameraMoveTo(const FPALPosition3d& Position)
{
	APALSceneCameraActor* TempCameraActor = GetWorld()->SpawnActor<APALSceneCameraActor>();
	TempCameraActor->SetActorLocation(Position.toLocation());
	AActor* ViewTarget = GetViewTarget();
	SetViewTarget(TempCameraActor);
	if (ViewTarget != CameraActor)
	{
		ViewTarget->Destroy();
	}
}

void APALPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &APALPlayerController::OnEscape);
	InputComponent->BindAction("UseItem", IE_Pressed, this, &APALPlayerController::GameUseItem);
}