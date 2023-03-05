// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSpriteMeshComponent.h"
#include "PAL.h"

UPALSpriteMeshComponent::UPALSpriteMeshComponent() 
	: Sprite(nullptr), bChangedSprite(false), LocationOffset(FVector3d::Zero()), Layer(0), bChangedLayer(false)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshRef(TEXT("StaticMesh'/Game/PAL_Plane.PAL_Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> MaskMaterialRef(TEXT("Material'/Game/PAL_M_Character.PAL_M_Character'"));
	DynamicMaterial = UMaterialInstanceDynamic::Create(MaskMaterialRef.Object, nullptr);
	SetMaterial(0, DynamicMaterial);
	SetStaticMesh(PlaneMeshRef.Object);
	SetRelativeRotation(FRotator(0., 0., 90.));
}

void UPALSpriteMeshComponent::SetLocationOffset(const FVector3d& InLocationOffSet)
{
	LocationOffset = InLocationOffSet;
}

void UPALSpriteMeshComponent::SetSprite(UPALSprite* NewSprite)
{
	NewSprite->Preload();
	this->Sprite = NewSprite;
	bChangedSprite = true;
}

void UPALSpriteMeshComponent::SetFrame(SIZE_T FrameNum)
{
	if (!bChangedSprite && FrameNum == CurrentFrameNum)
	{
		return;
	}

	bChangedSprite = false;
	CurrentFrameNum = FrameNum;

	if (Sprite == nullptr || Sprite->GetFrameCount() <= FrameNum)
	{
		return;
	}
	
	UTexture2D* Texture = Sprite->GetFrame(FrameNum);
	DynamicMaterial->SetTextureParameterValue(FName("PAL_Texture2D"), Texture);
	if (Layer >= 0)
	{
		if (bChangedLayer)
		{
			SetRelativeRotation(FRotator(0., 0., 90.));
		}
		SetRelativeScale3D(FVector3d(Texture->GetSizeX() * PIXEL_TO_UNIT / 100., Texture->GetSizeY() * 2 / SQRT_3 * PIXEL_TO_UNIT / 100., 1.));
		SetRelativeLocation(LocationOffset + FVector3d(0, Layer * SQRT_3 * PIXEL_TO_UNIT, Texture->GetSizeY() / SQRT_3 * PIXEL_TO_UNIT + Layer * PIXEL_TO_UNIT));
	}
	else
	{
		double Roll = (Layer > -10) ? (9. * Layer) : (- 90 - 1. / Layer);
		if (bChangedLayer)
		{
			SetRelativeRotation(FRotator(0., 0., 90. + Roll));
		}
		double Sin = FMath::Sin(FMath::DegreesToRadians(60. - Roll));
		SetRelativeScale3D(FVector3d(Texture->GetSizeX() * PIXEL_TO_UNIT / 100., Texture->GetSizeY() / Sin * PIXEL_TO_UNIT / 100., 1.));
		SetRelativeLocation(LocationOffset + FVector3d(0, - Texture->GetSizeY() / Sin / 2 * FMath::Cos(FMath::DegreesToRadians(90. + Roll)) * PIXEL_TO_UNIT, Texture->GetSizeY() / Sin / 2 * FMath::Sin(FMath::DegreesToRadians(90. + Roll)) * PIXEL_TO_UNIT));
	}

	bChangedLayer = false;
}

void UPALSpriteMeshComponent::SetLayer(int16 InLayer)
{
	if (Layer != InLayer)
	{
		Layer = InLayer;
		bChangedLayer = true;
	}
}
