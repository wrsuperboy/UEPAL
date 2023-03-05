// Copyright (C) 2022 Meizhouxuanhan.


#include "PALInGameMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "PALShowCash.h"
#include "PALPlayerController.h"
#include "PALSceneGameMode.h"
#include "PALMenuAnchor.h"
#include "PALRoleSelectMenu.h"
#include "PALSwitchMenu.h"
#include "PALConfirmMenu.h"
#include "PALAudioManager.h"
#include "PALCommon.h"

void UPALInGameMenu::EmptyFunction()
{
	PAL_DebugMsg("EmptyFunction");
}

void UPALInGameMenu::CreateRoleStatusMenu()
{
	Cast<APALPlayerController>(GetOwningPlayer())->GameRoleStatus();
}

UUserWidget* UPALInGameMenu::CreateInGameMagicMenu()
{
	UPALRoleSelectMenu* RoleSelectMenu = CreateWidget<UPALRoleSelectMenu>(GetOwningPlayer(), UPALRoleSelectMenu::StaticClass(), TEXT("InGameMagicMenu"));
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, true);
	}
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP = Cast<APALPlayerController>(GetOwningPlayer());
	RoleSelectMenu->OnRoleSelected.BindWeakLambda(this, [PlayerControllerWP](SIZE_T RoleId) {
		if (PlayerControllerWP.IsValid())
		{
			PlayerControllerWP->GameUseMagic(RoleId);
		}
		});
	PAL_DebugMsg("CreateInGameMagicMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(InGameMagicMenuAnchor, UPALMenuAnchor::StaticClass()));
	return RoleSelectMenu;
}

void UPALInGameMenu::CreateEquipItemMenu()
{
	Cast<APALPlayerController>(GetOwningPlayer())->GameEquipItem();
}

void UPALInGameMenu::CreateUseItemMenu()
{
	Cast<APALPlayerController>(GetOwningPlayer())->GameUseItem();
}

UPALMenuBox* UPALInGameMenu::CreateIventoryMenu()
{
	UPALMenuBox* MenuBox = CreateWidget<UPALMenuBox>(GetOwningPlayer(), UPALMenuBox::StaticClass(), TEXT("IventoryMenuBox"));
	FScriptDelegate Delegate;
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(22), this, FName("CreateEquipItemMenu"));
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(23), this, FName("CreateUseItemMenu"));
	PAL_DebugMsg("CreateIventoryMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(InventoryMenuAnchor, UPALMenuAnchor::StaticClass()));
	return MenuBox;
}

void UPALInGameMenu::CreateSaveGameMenu()
{
	UPALSaveSlotMenu* SaveSlotMenu = WidgetTree->ConstructWidget<UPALSaveSlotMenu>(UPALSaveSlotMenu::StaticClass());
	UCanvasPanelSlot* SaveSlotMenuSlot = Canvas->AddChildToCanvas(SaveSlotMenu);
	SaveSlotMenuSlot->SetAnchors(FAnchors(1, 0, 1, 1));
	SaveSlotMenuSlot->SetAlignment(FVector2D(1, 0));
	SaveSlotMenuSlot->SetZOrder(1);
	SaveSlotMenuSlot->SetAutoSize(true);
	SaveSlotMenuSlot->SetPosition(FVector2D(-13, 0) * UI_PIXEL_TO_UNIT);
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(Cast<APALPlayerController>(GetOwningPlayer()));
	TWeakObjectPtr<APALSceneGameMode> SceneGameModeWP(Cast<APALSceneGameMode>(GetWorld()->GetAuthGameMode()));
	SaveSlotMenu->OnSaveSlotSelected.BindWeakLambda(this, [PlayerControllerWP, SceneGameModeWP](SIZE_T SaveSlot) {
		if (SceneGameModeWP.IsValid())
		{
			SceneGameModeWP->SaveGame(SaveSlot);
		}
		if (PlayerControllerWP.IsValid())
		{
			PlayerControllerWP->CloseCurrentMenu();
		}
		});
	PAL_DebugMsg("CreateSaveGameMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(SaveSlotMenu, UPALSaveSlotMenu::StaticClass()));
}

void UPALInGameMenu::CreateLoadGameMenu()
{
	UPALSaveSlotMenu* SaveSlotMenu = WidgetTree->ConstructWidget<UPALSaveSlotMenu>(UPALSaveSlotMenu::StaticClass());
	UCanvasPanelSlot* SaveSlotMenuSlot = Canvas->AddChildToCanvas(SaveSlotMenu);
	SaveSlotMenuSlot->SetAnchors(FAnchors(1, 0, 1, 1));
	SaveSlotMenuSlot->SetAlignment(FVector2D(1, 0));
	SaveSlotMenuSlot->SetZOrder(1);
	SaveSlotMenuSlot->SetAutoSize(true);
	SaveSlotMenuSlot->SetPosition(FVector2D(-13, 0) * UI_PIXEL_TO_UNIT);
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(Cast<APALPlayerController>(GetOwningPlayer()));
	TWeakObjectPtr<APALSceneGameMode> SceneGameModeWP(Cast<APALSceneGameMode>(GetWorld()->GetAuthGameMode()));
	SaveSlotMenu->OnSaveSlotSelected.BindWeakLambda(this, [PlayerControllerWP, SceneGameModeWP](SIZE_T SaveSlot) {
		if (SceneGameModeWP.IsValid())
		{
			SceneGameModeWP->ReloadInNextTick(SaveSlot);
		}
		if (PlayerControllerWP.IsValid())
		{
			PlayerControllerWP->CloseCurrentMenu();
		}
		});
	PAL_DebugMsg("CreateLoadGameMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(SaveSlotMenu, UPALSaveSlotMenu::StaticClass()));
}

void UPALInGameMenu::CreateEnableMusicMenu()
{
	UPALSwitchMenu* SwitchMenu = CreateWidget<UPALSwitchMenu>(GetOwningPlayer(), UPALSwitchMenu::StaticClass());
	TWeakObjectPtr<UPALInGameMenu> InGameMenuWP(this);
	SwitchMenu->OnToggled.AddWeakLambda(this, [InGameMenuWP](bool bEnabled) {
		if (InGameMenuWP.IsValid())
		{
			InGameMenuWP->GetWorld()->GetSubsystem<UPALAudioManager>()->EnableMusic(bEnabled);
			InGameMenuWP->GoBack();
		}
		});
	SwitchMenu->AddToViewport();
	PAL_DebugMsg("CreateEnableMusicMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(SwitchMenu, UPALSwitchMenu::StaticClass()));
}

void UPALInGameMenu::CreateEnableSoundMenu()
{
	UPALSwitchMenu* SwitchMenu = CreateWidget<UPALSwitchMenu>(GetOwningPlayer(), UPALSwitchMenu::StaticClass());
	TWeakObjectPtr<UPALInGameMenu> InGameMenuWP(this);
	SwitchMenu->OnToggled.AddWeakLambda(this, [InGameMenuWP](bool bEnabled) {
		if (InGameMenuWP.IsValid())
		{
			InGameMenuWP->GetWorld()->GetSubsystem<UPALAudioManager>()->EnableSound(bEnabled);
			InGameMenuWP->GoBack();
		}
		});
	SwitchMenu->AddToViewport();
	PAL_DebugMsg("CreateEnableSoundMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(SwitchMenu, UPALSwitchMenu::StaticClass()));
}

void UPALInGameMenu::CreateQuitGameConfirmMenu()
{
	UPALConfirmMenu* ConfirmMenu = CreateWidget<UPALConfirmMenu>(GetOwningPlayer(), UPALConfirmMenu::StaticClass());
	TWeakObjectPtr<APALPlayerController> PlayerControllerWP(Cast<APALPlayerController>(GetOwningPlayer()));
	TWeakObjectPtr<UPALInGameMenu> InGameMenuWP(this);
	ConfirmMenu->OnConfirm.AddWeakLambda(this, [PlayerControllerWP, InGameMenuWP](bool bYes) {
		if (InGameMenuWP.IsValid())
		{
			InGameMenuWP->GoBack();
			if (bYes && PlayerControllerWP.IsValid())
			{
				PlayerControllerWP->ConsoleCommand("quit");
			}
		}
		});
	ConfirmMenu->AddToViewport();
	PAL_DebugMsg("CreateEnableSoundMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(ConfirmMenu, UPALConfirmMenu::StaticClass()));
}

UUserWidget* UPALInGameMenu::CreateSystemMenu()
{
	UPALMenuBox* MenuBox = CreateWidget<UPALMenuBox>(GetOwningPlayer(), UPALMenuBox::StaticClass(), TEXT("SystemMenuBox"));
	FScriptDelegate Delegate;
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(11), this, FName("CreateSaveGameMenu"));
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(12), this, FName("CreateLoadGameMenu"));
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(13), this, FName("CreateEnableMusicMenu"));
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(14), this, FName("CreateEnableSoundMenu"));
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(15), this, FName("CreateQuitGameConfirmMenu"));
	PAL_DebugMsg("CreateSystemMenu");
	PushStack(TTuple<UWidget*, TSubclassOf<UWidget>>(SystemMenuAnchor, UPALMenuAnchor::StaticClass()));
	return MenuBox;
}

void UPALInGameMenu::PushStack(const TTuple<UWidget*, TSubclassOf<UWidget>>& WidgetAndClass)
{
	TTuple<UWidget*, TSubclassOf<UWidget>> Entry = MenuStack.Last();
	if (Entry.Value == UPALMenuAnchor::StaticClass())
	{
		Cast<UPALMenuBox>(Cast<UPALMenuAnchor>(Entry.Key)->GetMenuWidget())->SetHittable(false);
	}
	else if (Entry.Value == UPALMenuBox::StaticClass())
	{
		Cast<UPALMenuBox>(Entry.Key)->SetHittable(false);
	}
	else
	{
		checkNoEntry();
	}
	MenuStack.Push(WidgetAndClass);
}

void UPALInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UVerticalBox* LeftArea = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LeftArea"));
	UCanvasPanelSlot* LeftAreaSlot = Canvas->AddChildToCanvas(LeftArea);
	LeftAreaSlot->SetAnchors(FAnchors(0, 0, 0, 1));
	LeftAreaSlot->SetPosition(FVector2D(0, LeftAreaSlot->GetPosition().Y));
	LeftAreaSlot->SetOffsets(FMargin(LeftAreaSlot->GetOffsets().Left, 0, LeftAreaSlot->GetOffsets().Right, 0));
	LeftAreaSlot->SetSize(FVector2D(500, LeftAreaSlot->GetSize().Y));
	LeftAreaSlot->SetAlignment(FVector2D(0, 0));
	LeftAreaSlot->SetAutoSize(true);

	UCanvasPanel* ShowCashCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("ShowCashCanvas"));
	UVerticalBoxSlot* ShowCashCanvasSlot = LeftArea->AddChildToVerticalBox(ShowCashCanvas);
	ShowCashCanvasSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	ShowCashCanvasSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	ShowCashCanvasSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	UPALShowCash* ShowCash = CreateWidget<UPALShowCash>(GetOwningPlayer(), UPALShowCash::StaticClass(), TEXT("ShowCash"));
	UCanvasPanelSlot* ShowCashSlot = ShowCashCanvas->AddChildToCanvas(ShowCash);
	ShowCashSlot->SetAnchors(FAnchors(0));
	ShowCashSlot->SetSize(FVector2D(96 * UI_PIXEL_TO_UNIT, 34 * UI_PIXEL_TO_UNIT));

	UPALMenuBox* MenuBox = CreateWidget<UPALMenuBox>(GetOwningPlayer(), UPALMenuBox::StaticClass(), TEXT("InGameMenuBox"));
	FScriptDelegate Delegate;
	MenuBox->AddItemAndBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(3), this, FName("CreateRoleStatusMenu"));
	InGameMagicMenuAnchor = MenuBox->AddMenuItemWithBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(4), this, FName("CreateInGameMagicMenu"));
	InventoryMenuAnchor = MenuBox->AddMenuItemWithBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(5), this, FName("CreateIventoryMenu"));
	SystemMenuAnchor = MenuBox->AddMenuItemWithBind(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(6), this, FName("CreateSystemMenu"));
	UVerticalBoxSlot* MenuBoxSlot = LeftArea->AddChildToVerticalBox(MenuBox);
	MenuBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	MenuBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	MenuBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	MenuStack.Add(TTuple<UWidget*, TSubclassOf<UWidget>>(MenuBox, UPALMenuBox::StaticClass()));
}

bool UPALInGameMenu::GoBack()
{
	if (MenuStack.Num() <= 1)
	{
		return false;
	}

	TTuple<UWidget*, TSubclassOf<UWidget>> Entry = MenuStack.Pop();
	if (Entry.Value == UPALMenuAnchor::StaticClass())
	{
		Cast<UPALMenuAnchor>(Entry.Key)->Close();
	}
	else if (Entry.Value == UPALMenuBox::StaticClass())
	{
		Cast<UPALMenuBox>(Entry.Key)->RemoveFromParent();
	}
	else if (Entry.Value == UPALSaveSlotMenu::StaticClass())
	{
		Cast<UPALSaveSlotMenu>(Entry.Key)->RemoveFromParent();
	}
	else if (Entry.Value == UPALSwitchMenu::StaticClass())
	{
		Cast<UPALSwitchMenu>(Entry.Key)->RemoveFromParent();
	}
	else if (Entry.Value == UPALConfirmMenu::StaticClass())
	{
		Cast<UPALConfirmMenu>(Entry.Key)->RemoveFromParent();
	}
	else
	{
		checkNoEntry();
	}
	Entry = MenuStack.Last();
	if (Entry.Value == UPALMenuAnchor::StaticClass())
	{
		Cast<UPALMenuBox>(Cast<UPALMenuAnchor>(Entry.Key)->GetMenuWidget())->SetHittable(true);
	}
	else if (Entry.Value == UPALMenuBox::StaticClass())
	{
		Cast<UPALMenuBox>(Entry.Key)->SetHittable(true);
	}
	else
	{
		checkNoEntry();
	}
	PAL_DebugMsg("Should remove the top menu");
	return true;
}
