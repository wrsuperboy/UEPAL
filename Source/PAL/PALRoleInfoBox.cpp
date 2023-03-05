// Copyright (C) 2022 Meizhouxuanhan.


#include "PALRoleInfoBox.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateBrush.h"
#include "PALPlayerController.h"
#include "PALGameState.h"
#include "PALGameStateData.h"
#include "PALCommon.h"

const FVector2D StatusPosition[EPALRoleStatus::_RoleStatusCount] =
{
   FVector2D(35, 19),  // confused
   FVector2D(44, 12),  // slow
   FVector2D(54, 1),   // sleep
   FVector2D(55, 20),  // silence
   FVector2D(0, 0),    // puppet
   FVector2D(0, 0),    // bravery
   FVector2D(0, 0),    // protect
   FVector2D(0, 0),    // haste
   FVector2D(0, 0),    // dualattack
};

const SIZE_T StatusWordNum[EPALRoleStatus::_RoleStatusCount] =
{
   0x1D,  // confused
   0x1B,  // slow
   0x1C,  // sleep
   0x1A,  // silence
   0x00,  // puppet
   0x00,  // bravery
   0x00,  // protect
   0x00,  // haste
   0x00,  // dualattack
};

const SIZE_T StatusColor[EPALRoleStatus::_RoleStatusCount] =
{
   0x5F,  // confused
   0xBF,  // slow
   0x0E,  // sleep
   0x3C,  // silence
   0x00,  // puppet
   0x00,  // bravery
   0x00,  // protect
   0x00,  // haste
   0x00,  // dualattack
};

void UPALRoleInfoBox::Init(SIZE_T InRoleId)
{
	RoleId = InRoleId;
	bSelectionEnabled = false;
}

void UPALRoleInfoBox::SetSelectionEnabled(bool bInSelectionEnabled)
{
	if (bSelectionEnabled != bInSelectionEnabled)
	{
		bSelectionEnabled = bInSelectionEnabled;

		UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
		FButtonStyle ButtonStyle;
		FSlateBrush SlatBrushNone;
		SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
		ButtonStyle.SetNormal(SlatBrushNone);
		ButtonStyle.SetHovered(SlatBrushNone);
		ButtonStyle.SetPressed(SlatBrushNone);
		ButtonStyle.SetDisabled(SlatBrushNone);
		ButtonStyle.SetNormalForeground(FLinearColor::White);
		ButtonStyle.SetDisabledForeground(FLinearColor::White);
		ButtonStyle.SetPressedForeground(bSelectionEnabled ? FLinearColor(Common->GetDefaultPalette()[44]) : FLinearColor::White);
		ButtonStyle.SetHoveredForeground(bSelectionEnabled ? FLinearColor(Common->GetDefaultPalette()[249]) : FLinearColor::White);
		ButtonStyle.SetNormalPadding(FMargin(0));
		ButtonStyle.SetPressedPadding(FMargin(0));
		Button->SetStyle(ButtonStyle);
	}
}

void UPALRoleInfoBox::Refresh()
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	UPALPlayerStateData* PlayerStateData = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	HPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.HP[RoleId])));
	MaxHPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MaxHP[RoleId])));
	MPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MP[RoleId])));
	MaxMPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MaxMP[RoleId])));

	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	uint16 PoisonId = PlayerState->GetRoleHighestLevelPoisonId(RoleId);
	if (PoisonId != 0) {
		FColor PoisonColor = Common->GetDefaultPalette()[GameStateData->Objects[PoisonId].Poison.Color];
		FaceImage->SetColorAndOpacity(PoisonColor);
	}

	if (PlayerStateData->PlayerRoles.HP[RoleId] > 0)
	{
		for (EPALRoleStatus RoleStatus = Confused; RoleStatus < EPALRoleStatus::_RoleStatusCount; RoleStatus = static_cast<EPALRoleStatus>(RoleStatus + 1))
		{
			if (PlayerState->IsRoleInStatus(RoleId, RoleStatus) && StatusWordNum[RoleStatus] != 0)
			{
				Status->SetText(FText::FromString(Common->GetWord(StatusWordNum[RoleStatus])));
				Status->SetColorAndOpacity(Common->GetDefaultPalette()[StatusColor[RoleStatus]]);
				Cast<UCanvasPanelSlot>(Status->Slot)->SetPosition(StatusPosition[RoleStatus] * UI_PIXEL_TO_UNIT);
			}
		}
	}
}

void UPALRoleInfoBox::Select()
{
	if (bSelectionEnabled)
	{
		PAL_DebugMsg("Role info box selected");
		OnSelect.ExecuteIfBound(RoleId);
	}
}

TSharedRef<SWidget> UPALRoleInfoBox::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALRoleInfoBox::NativeConstruct()
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();
	USizeBox* RootWidget = Cast<USizeBox>(GetRootWidget());
	UTexture2D* BackgroundTexture = UISprite->GetFrame(18);
	RootWidget->SetWidthOverride(BackgroundTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
	RootWidget->SetHeightOverride(BackgroundTexture->GetSizeY() * UI_PIXEL_TO_UNIT);

	Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	Button->SetBackgroundColor(FLinearColor::Transparent);
	FButtonStyle ButtonStyle;
	FSlateBrush SlatBrushNone;
	SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
	ButtonStyle.SetNormal(SlatBrushNone);
	ButtonStyle.SetHovered(SlatBrushNone);
	ButtonStyle.SetPressed(SlatBrushNone);
	ButtonStyle.SetDisabled(SlatBrushNone);
	ButtonStyle.SetNormalForeground(FLinearColor::White);
	ButtonStyle.SetDisabledForeground(FLinearColor::White);
	ButtonStyle.SetPressedForeground(bSelectionEnabled ? FLinearColor(Common->GetDefaultPalette()[44]) : FLinearColor::White);
	ButtonStyle.SetHoveredForeground(bSelectionEnabled ? FLinearColor(Common->GetDefaultPalette()[249]) : FLinearColor::White);
	ButtonStyle.SetNormalPadding(FMargin(0));
	ButtonStyle.SetPressedPadding(FMargin(0));
	Button->SetStyle(ButtonStyle);
	TScriptDelegate<> OnClicked;
	OnClicked.BindUFunction(this, "Select");
	Button->OnClicked.Add(OnClicked);
	RootWidget->AddChild(Button);
	
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	USlateBrushAsset* BorderBrushAsset = NewObject<USlateBrushAsset>();
	BorderBrushAsset->Brush.SetResourceObject(BackgroundTexture);
	BorderBrushAsset->Brush.TintColor = FSlateColor::UseForeground();
	Border->SetBrushFromAsset(BorderBrushAsset);
	Button->AddChild(Border);

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	Border->SetContent(Canvas);

	FaceImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* FaceTexture = UISprite->GetFrame(48 + RoleId);
	USlateBrushAsset* FaceImageBrushAsset = NewObject<USlateBrushAsset>();
	FaceImageBrushAsset->Brush.SetResourceObject(FaceTexture);
	FaceImageBrushAsset->Brush.TintColor = FSlateColor::UseForeground();
	FaceImage->SetBrushFromAsset(FaceImageBrushAsset);
	UCanvasPanelSlot* FaceImageSlot = Canvas->AddChildToCanvas(FaceImage);
	FaceImageSlot->SetSize(FVector2D(FaceTexture->GetSizeX(), FaceTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	FaceImageSlot->SetPosition(FVector2D(-3, -4) * UI_PIXEL_TO_UNIT);

	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	UPALPlayerStateData* PlayerStateData = PlayerState->GetPlayerStateData();

	HPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	HPNumber->SetColorAndOpacity(FColor(212, 200, 168));
	HPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	HPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* HPNumberSlot = Canvas->AddChildToCanvas(HPNumber);
	HPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	HPNumberSlot->SetPosition(FVector2D(50, 9) * UI_PIXEL_TO_UNIT);
	HPNumberSlot->SetAutoSize(true);

	MaxHPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MaxHPNumber->SetColorAndOpacity(FColor(212, 200, 168));
	MaxHPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	MaxHPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* MaxHPNumberSlot = Canvas->AddChildToCanvas(MaxHPNumber);
	MaxHPNumberSlot->SetAlignment(FVector2D(0, 0.5));
	MaxHPNumberSlot->SetPosition(FVector2D(53, 12) * UI_PIXEL_TO_UNIT);
	MaxHPNumberSlot->SetAutoSize(true);

	MPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MPNumber->SetColorAndOpacity(FColor(80, 184, 148));
	MPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	MPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* MPNumberSlot = Canvas->AddChildToCanvas(MPNumber);
	MPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MPNumberSlot->SetPosition(FVector2D(50, 25) * UI_PIXEL_TO_UNIT);
	MPNumberSlot->SetAutoSize(true);

	MaxMPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MaxMPNumber->SetColorAndOpacity(FColor(80, 184, 148));
	MaxMPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	MaxMPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* MaxMPNumberSlot = Canvas->AddChildToCanvas(MaxMPNumber);
	MaxMPNumberSlot->SetAlignment(FVector2D(0, 0.5));
	MaxMPNumberSlot->SetPosition(FVector2D(53, 28) * UI_PIXEL_TO_UNIT);
	MaxMPNumberSlot->SetAutoSize(true);

	UTexture2D* SlashTexture = UISprite->GetFrame(39);
	UImage* HPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	HPSlash->SetBrushFromTexture(SlashTexture);
	UCanvasPanelSlot* HPSlashSlot = Canvas->AddChildToCanvas(HPSlash);
	HPSlashSlot->SetPosition(FVector2D(49, 6) * UI_PIXEL_TO_UNIT);
	HPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UImage* MPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	MPSlash->SetBrushFromTexture(SlashTexture);
	MPSlash->SetDesiredSizeOverride(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UCanvasPanelSlot* MPSlashSlot = Canvas->AddChildToCanvas(MPSlash);
	MPSlashSlot->SetPosition(FVector2D(49, 22) * UI_PIXEL_TO_UNIT);
	MPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);

	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	Status = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Status->SetFont(FontInfo);
	Status->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	Status->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	Canvas->AddChildToCanvas(Status);

	Refresh();
}
