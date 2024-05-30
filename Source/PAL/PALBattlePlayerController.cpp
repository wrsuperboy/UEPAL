// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattlePlayerController.h"
#include "PALPlayerState.h"

void APALBattlePlayerController::Init()
{
	UIStatus = EPALBattleUIStatus::BattleUIWait;
	bAutoAttack = false;
	bRepeat = false;
	bForce = false;
	bFlee = false;
	SelectedIndex = 0;
	PreviousEnemyTarget = -1;
	BattleMenu = CreateWidget<UPALBattleMenu>(this, UPALBattleMenu::StaticClass());
	BattleMenu->AddToViewport(0);
	SetShowMouseCursor(true);
	AutoAttackMessage = CreateWidget<UPALBattleAutoAttackMessage>(this, UPALBattleAutoAttackMessage::StaticClass());

	bInit = true;
}

EPALBattleUIStatus APALBattlePlayerController::GetUIStatus() const
{
	return UIStatus;
}

void APALBattlePlayerController::BattleUIRoleReady(SIZE_T RoleId)
{
}

void APALBattlePlayerController::BattleUIWait()
{
	UIStatus = EPALBattleUIStatus::BattleUIWait;
}

bool APALBattlePlayerController::IsAutoAttack() const
{
	return bAutoAttack;
}

bool APALBattlePlayerController::IsRepeat() const
{
	return bRepeat;
}

bool APALBattlePlayerController::IsForce() const
{
	return bForce;
}

bool APALBattlePlayerController::IsFlee() const
{
	return bFlee;
}

void APALBattlePlayerController::SetAutoAttack(bool bInAutoAttack)
{
	if (bAutoAttack != bInAutoAttack)
	{
		if (bInAutoAttack)
		{
			if (!AutoAttackMessage->IsInViewport()) {
				AutoAttackMessage->AddToViewport(0);
			}
		}
		else
		{
			if (AutoAttackMessage->IsInViewport()) {
				AutoAttackMessage->RemoveFromParent();
			}
		}
		bAutoAttack = bAutoAttack;
	}
}

void APALBattlePlayerController::ClearTeamAction()
{
	bRepeat = false;
	bForce = false;
	bFlee = false;
}

void APALBattlePlayerController::SetUISourceTargetAction(SIZE_T NewRoleId, int16 ToSelectIndex, EBattleActionType ActionType)
{
	CurrentRoleId = NewRoleId;
	SelectedIndex = ToSelectIndex;
}

void APALBattlePlayerController::OnEscape()
{
	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (bAutoAttack && !PlayerStateData->bAutoBattle) {
		SetAutoAttack(false);
	}
}

void APALBattlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &APALBattlePlayerController::OnEscape);
}

void APALBattlePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bInit)
	{
		return;
	}

	UPALPlayerStateData* PlayerStateData = GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (PlayerStateData->bAutoBattle)
	{

	}
}
