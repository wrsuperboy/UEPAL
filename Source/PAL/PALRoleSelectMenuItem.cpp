// Copyright (C) 2022 Meizhouxuanhan.


#include "PALRoleSelectMenuItem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALRoleSelectMenuItem::Init(SIZE_T InRoleId, const FString& InRoleName, bool bInSelectable)
{
	RoleId = InRoleId;
	RoleName = InRoleName;
	bSelectable = bInSelectable;
}

void UPALRoleSelectMenuItem::Select()
{
	PAL_DebugMsg("Role select menu item selected");
	OnSelected.ExecuteIfBound(RoleId);
}

void UPALRoleSelectMenuItem::Hover()
{
	OnHovered.ExecuteIfBound(RoleId);
}

TSharedRef<SWidget> UPALRoleSelectMenuItem::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());;
	}

	return Super::RebuildWidget();
}

void UPALRoleSelectMenuItem::NativeConstruct()
{
	Super::NativeConstruct();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UButton* RootWidget = Cast<UButton>(GetRootWidget());
	FButtonStyle ButtonStyle;
	FSlateBrush SlatBrushNone;
	SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
	ButtonStyle.SetNormal(SlatBrushNone);
	ButtonStyle.SetHovered(SlatBrushNone);
	ButtonStyle.SetPressed(SlatBrushNone);
	ButtonStyle.SetDisabled(SlatBrushNone);
	ButtonStyle.SetNormalForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable ? 79 : 28]));
	ButtonStyle.SetDisabledForeground(FLinearColor(Common->GetDefaultPalette()[28])); // Not used
	ButtonStyle.SetPressedForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable ? 44 : 31]));
	ButtonStyle.SetHoveredForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable ? 249 : 31]));
	RootWidget->SetStyle(ButtonStyle);
	if (bSelectable)
	{
		TScriptDelegate<> OnClickedDelegate;
		OnClickedDelegate.BindUFunction(this, "Select");
		RootWidget->OnClicked.Add(OnClickedDelegate);
	}
	TScriptDelegate<> OnHoveredDelegate;
	OnHoveredDelegate.BindUFunction(this, "Hover");
	RootWidget->OnHovered.Add(OnHoveredDelegate);

	UTextBlock* RoleButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	RoleButtonText->SetText(FText::FromString(RoleName));
	RoleButtonText->SetFont(FontInfo);
	RoleButtonText->SetColorAndOpacity(FSlateColor::UseForeground());
	RoleButtonText->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	RoleButtonText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UButtonSlot* RoleButtonTextSlot = Cast<UButtonSlot>(RootWidget->AddChild(RoleButtonText));
	RoleButtonTextSlot->SetPadding(FMargin(0, 1., 0, 1.) * UI_PIXEL_TO_UNIT);
	RoleButtonTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	RoleButtonTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
}