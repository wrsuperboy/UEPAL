// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "PALCommon.h"

UTexture2D* NewGrayscaleTexture(UTexture2D* Texture)
{
    // Get the texture's width and height  
    int32 Width = Texture->GetSizeX();
    int32 Height = Texture->GetSizeY();

    // Create a new texture with the same properties as the original  
    UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, Texture->GetPixelFormat());

    // Lock the original texture for reading  
    FTexture2DMipMap& SourceMip = Texture->GetPlatformData()->Mips[0];
    void* SourceData = SourceMip.BulkData.Lock(LOCK_READ_ONLY);

    // Lock the new texture for writing  
    FTexture2DMipMap& DestinationMip = NewTexture->GetPlatformData()->Mips[0];
    void* DestinationData = DestinationMip.BulkData.Lock(LOCK_READ_WRITE);

    // Convert each pixel to grayscale and copy the pixel data from the original texture to the new texture  
    for (int32 Y = 0; Y < Height; Y++)
    {
        for (int32 X = 0; X < Width; X++)
        {
            // Get the pixel color from the original texture  
            FColor* SourceColor = static_cast<FColor*>(SourceData) + (Y * Width) + X;

            // Calculate the grayscale value  
            uint8 Gray = (SourceColor->R + SourceColor->G + SourceColor->B) / 3;

            // Set the pixel color to grayscale in the new texture  
            FColor* DestinationColor = static_cast<FColor*>(DestinationData) + (Y * Width) + X;
            DestinationColor->R = Gray;
            DestinationColor->G = Gray;
            DestinationColor->B = Gray;
			DestinationColor->A = SourceColor->A;
        }
    }

    // Unlock both textures  
    SourceMip.BulkData.Unlock();
    DestinationMip.BulkData.Unlock();

    // Update the new texture's resource  
    NewTexture->UpdateResource();

    return NewTexture;
}

void UPALBattleMenu::SetMenuState(EPALBattleMenuState NewState)
{
	MenuState = NewState;
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UPALBattleMenu::NativeConstruct()
{
	Super::NativeConstruct();

	RoleInfoPanel = WidgetTree->ConstructWidget<UPALRoleInfoPanel>(UPALRoleInfoPanel::StaticClass());
	UCanvasPanelSlot* RoleInfoPanelSlot = Canvas->AddChildToCanvas(RoleInfoPanel);
	RoleInfoPanelSlot->SetAlignment(FVector2D(1, 1));
	RoleInfoPanelSlot->SetAnchors(FAnchors(1, 1, 1, 1));
	RoleInfoPanelSlot->SetAutoSize(true);

	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UPALSprite* UISprite = Common->GetUISprite();

	UButton* AttackButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle AttackButtonStyle;
	UTexture2D* AttackTexture = UISprite->GetFrame(40);
	FSlateBrush AttackBrushNormal;
	AttackBrushNormal.DrawAs = ESlateBrushDrawType::Image;
    UTexture2D* AttackTextureGrayscale = NewGrayscaleTexture(AttackTexture);
	AttackBrushNormal.SetResourceObject(AttackTextureGrayscale);
	AttackButtonStyle.SetNormal(AttackBrushNormal);
	FSlateBrush AttackBrushHovered;
	AttackBrushHovered.DrawAs = ESlateBrushDrawType::Image;
	AttackBrushHovered.SetResourceObject(AttackTexture);
	AttackButtonStyle.SetHovered(AttackBrushHovered);
    FSlateBrush AttackBrushPressed;
    AttackBrushPressed.DrawAs = ESlateBrushDrawType::Image;
    AttackBrushPressed.SetResourceObject(AttackTexture);
    AttackBrushPressed.TintColor = FSlateColor(FLinearColor::Gray);
	AttackButtonStyle.SetPressed(AttackBrushPressed);
	FSlateBrush AttackBrushDisabled;
	AttackBrushDisabled.DrawAs = ESlateBrushDrawType::Image;
	AttackBrushDisabled.SetResourceObject(AttackTexture);
	AttackBrushDisabled.TintColor = FSlateColor(FLinearColor::Red);
	AttackButtonStyle.SetDisabled(AttackBrushDisabled);
	AttackButton->SetStyle(AttackButtonStyle);
	UCanvasPanelSlot* AttackButtonSlot = Canvas->AddChildToCanvas(AttackButton);
	AttackButtonSlot->SetSize(FVector2D(AttackTexture->GetSizeX(), AttackTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	AttackButtonSlot->SetAlignment(FVector2D(0, 0));
	AttackButtonSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	AttackButtonSlot->SetPosition(FVector2D(27, -60) * UI_PIXEL_TO_UNIT);

	UButton* MagicButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle MagicButtonStyle;
	UTexture2D* MagicTexture = UISprite->GetFrame(41);
	FSlateBrush MagicBrushNormal;
	MagicBrushNormal.DrawAs = ESlateBrushDrawType::Image;
	UTexture2D* MagicTextureGrayscale = NewGrayscaleTexture(MagicTexture);
	MagicBrushNormal.SetResourceObject(MagicTextureGrayscale);
	MagicButtonStyle.SetNormal(MagicBrushNormal);
	FSlateBrush MagicBrushHovered;
	MagicBrushHovered.DrawAs = ESlateBrushDrawType::Image;
	MagicBrushHovered.SetResourceObject(MagicTexture);
	MagicButtonStyle.SetHovered(MagicBrushHovered);
	FSlateBrush MagicBrushPressed;
	MagicBrushPressed.DrawAs = ESlateBrushDrawType::Image;
	MagicBrushPressed.SetResourceObject(MagicTexture);
	MagicBrushPressed.TintColor = FSlateColor(FLinearColor::Gray);
	MagicButtonStyle.SetPressed(MagicBrushPressed);
	FSlateBrush MagicBrushDisabled;
	MagicBrushDisabled.DrawAs = ESlateBrushDrawType::Image;
	MagicBrushDisabled.SetResourceObject(MagicTexture);
	MagicBrushDisabled.TintColor = FSlateColor(FLinearColor::Red);
	MagicButtonStyle.SetDisabled(MagicBrushDisabled);
	MagicButton->SetStyle(MagicButtonStyle);
	UCanvasPanelSlot* MagicButtonSlot = Canvas->AddChildToCanvas(MagicButton);
	MagicButtonSlot->SetSize(FVector2D(MagicTexture->GetSizeX(), MagicTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MagicButtonSlot->SetAlignment(FVector2D(0, 0));
	MagicButtonSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	MagicButtonSlot->SetPosition(FVector2D(0, -45) * UI_PIXEL_TO_UNIT);

	UButton* CoopMagicButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle CoopMagicButtonStyle;
	UTexture2D* CoopMagicTexture = UISprite->GetFrame(42);
	FSlateBrush CoopMagicBrushNormal;
	CoopMagicBrushNormal.DrawAs = ESlateBrushDrawType::Image;
	UTexture2D* CoopMagicTextureGrayscale = NewGrayscaleTexture(CoopMagicTexture);
	CoopMagicBrushNormal.SetResourceObject(CoopMagicTextureGrayscale);
	CoopMagicButtonStyle.SetNormal(CoopMagicBrushNormal);
	FSlateBrush CoopMagicBrushHovered;
	CoopMagicBrushHovered.DrawAs = ESlateBrushDrawType::Image;
	CoopMagicBrushHovered.SetResourceObject(CoopMagicTexture);
	CoopMagicButtonStyle.SetHovered(CoopMagicBrushHovered);
	FSlateBrush CoopMagicBrushPressed;
	CoopMagicBrushPressed.DrawAs = ESlateBrushDrawType::Image;
	CoopMagicBrushPressed.SetResourceObject(CoopMagicTexture);
	CoopMagicBrushPressed.TintColor = FSlateColor(FLinearColor::Gray);
	CoopMagicButtonStyle.SetPressed(CoopMagicBrushPressed);
	FSlateBrush CoopMagicBrushDisabled;
	CoopMagicBrushDisabled.DrawAs = ESlateBrushDrawType::Image;
	CoopMagicBrushDisabled.SetResourceObject(CoopMagicTexture);
	CoopMagicBrushDisabled.TintColor = FSlateColor(FLinearColor::Red);
	CoopMagicButtonStyle.SetDisabled(CoopMagicBrushDisabled);
	CoopMagicButton->SetStyle(CoopMagicButtonStyle);
	UCanvasPanelSlot* CoopMagicButtonSlot = Canvas->AddChildToCanvas(CoopMagicButton);
	CoopMagicButtonSlot->SetSize(FVector2D(CoopMagicTexture->GetSizeX(), CoopMagicTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	CoopMagicButtonSlot->SetAlignment(FVector2D(0, 0));
	CoopMagicButtonSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	CoopMagicButtonSlot->SetPosition(FVector2D(54, -45) * UI_PIXEL_TO_UNIT);

	UButton* MiscMenuButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	FButtonStyle MiscMenuButtonStyle;
	UTexture2D* MiscMenuTexture = UISprite->GetFrame(43);
	FSlateBrush MiscMenuBrushNormal;
	MiscMenuBrushNormal.DrawAs = ESlateBrushDrawType::Image;
	UTexture2D* MiscMenuTextureGrayscale = NewGrayscaleTexture(MiscMenuTexture);
	MiscMenuBrushNormal.SetResourceObject(MiscMenuTextureGrayscale);
	MiscMenuButtonStyle.SetNormal(MiscMenuBrushNormal);
	FSlateBrush MiscMenuBrushHovered;
	MiscMenuBrushHovered.DrawAs = ESlateBrushDrawType::Image;
	MiscMenuBrushHovered.SetResourceObject(MiscMenuTexture);
	MiscMenuButtonStyle.SetHovered(MiscMenuBrushHovered);
	FSlateBrush MiscMenuBrushPressed;
	MiscMenuBrushPressed.DrawAs = ESlateBrushDrawType::Image;
	MiscMenuBrushPressed.SetResourceObject(MiscMenuTexture);
	MiscMenuBrushPressed.TintColor = FSlateColor(FLinearColor::Gray);
	MiscMenuButtonStyle.SetPressed(MiscMenuBrushPressed);
	FSlateBrush MiscMenuBrushDisabled;
	MiscMenuBrushDisabled.DrawAs = ESlateBrushDrawType::Image;
	MiscMenuBrushDisabled.SetResourceObject(MiscMenuTexture);
	MiscMenuBrushDisabled.TintColor = FSlateColor(FLinearColor::Red);
	MiscMenuButtonStyle.SetDisabled(MiscMenuBrushDisabled);
	MiscMenuButton->SetStyle(MiscMenuButtonStyle);
	UCanvasPanelSlot* MiscMenuButtonSlot = Canvas->AddChildToCanvas(MiscMenuButton);
	MiscMenuButtonSlot->SetSize(FVector2D(MiscMenuTexture->GetSizeX(), MiscMenuTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MiscMenuButtonSlot->SetAlignment(FVector2D(0, 0));
	MiscMenuButtonSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	MiscMenuButtonSlot->SetPosition(FVector2D(27, -30) * UI_PIXEL_TO_UNIT);
}

bool UPALBattleMenu::GoBack() {
	return false;
}