// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSpriteMeshComponent.h"
#include "PAL.h"

UPALSpriteMeshComponent::UPALSpriteMeshComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Sprite(nullptr), bChangedSprite(false), LocationOffset(FVector3d::Zero()), Layer(0), bChangedLayer(false)
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaskMaterialRef(TEXT("Material'/Game/PAL_M_Character.PAL_M_Character'"));
	DynamicMaterial = UMaterialInstanceDynamic::Create(MaskMaterialRef.Object, nullptr);
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

	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();
	const FColor* TextureData = reinterpret_cast<const FColor*>(Texture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;
	const float InvWidth = 1.0f / Width;
	const float InvHeight = 1.0f / Height;
	int32 TriangleIndex = 0;
	for (int32 Y = 0; Y < Height; Y++)
	{
		for (int32 X = 0; X < Width; X++)
		{
			const FColor& Color = TextureData[Y * Width + X];
			if (Color.A != 0)
			{
				Vertices.Add(FVector(X, Y, 0));
				UV0.Add(FVector2D(X * InvWidth, Y * InvHeight));
				Vertices.Add(FVector(X, Y + 1, 0));
				UV0.Add(FVector2D(X * InvWidth, (Y + 1) * InvHeight));
				Vertices.Add(FVector(X + 1, Y, 0));
				UV0.Add(FVector2D((X + 1) * InvWidth, Y * InvHeight));
				Triangles.Add(TriangleIndex++);
				Triangles.Add(TriangleIndex++);
				Triangles.Add(TriangleIndex++);
				Vertices.Add(FVector(X + 1, Y, 0));
				UV0.Add(FVector2D((X + 1) * InvWidth, Y * InvHeight));
				Vertices.Add(FVector(X, Y + 1, 0));
				UV0.Add(FVector2D(X * InvWidth, (Y + 1) * InvHeight));
				Vertices.Add(FVector(X + 1, Y + 1, 0));
				UV0.Add(FVector2D((X + 1) * InvWidth, (Y + 1) * InvHeight));
				Triangles.Add(TriangleIndex++);
				Triangles.Add(TriangleIndex++);
				Triangles.Add(TriangleIndex++);
			}
		}
	}
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), false);

	DynamicMaterial->SetTextureParameterValue(FName("PAL_Texture2D"), Texture);
	SetMaterial(0, DynamicMaterial);

	if (Layer >= 0)
	{
		if (bChangedLayer)
		{
			SetRelativeRotation(FRotator(0., 0., 90.));
		}
		SetRelativeScale3D(FVector3d(PIXEL_TO_UNIT, 2 / SQRT_3 * PIXEL_TO_UNIT, 1.));
		SetRelativeLocation(LocationOffset + FVector3d(- Texture->GetSizeX() * PIXEL_TO_UNIT / 2., Layer * SQRT_3 * PIXEL_TO_UNIT, Texture->GetSizeY() * 2 / SQRT_3 * PIXEL_TO_UNIT + Layer * PIXEL_TO_UNIT));
	}
	else
	{
		double Roll = (Layer > -10) ? (9. * Layer) : (- 90 - 1. / Layer);
		if (bChangedLayer)
		{
			SetRelativeRotation(FRotator(0., 0., 90. + Roll));
		}
		double Sin = FMath::Sin(FMath::DegreesToRadians(60. - Roll));
		SetRelativeScale3D(FVector3d(PIXEL_TO_UNIT, 1. / Sin * PIXEL_TO_UNIT, 1.));
		SetRelativeLocation(LocationOffset + FVector3d(- Texture->GetSizeX() * PIXEL_TO_UNIT / 2., - Texture->GetSizeY() / Sin * FMath::Cos(FMath::DegreesToRadians(90. + Roll)) * PIXEL_TO_UNIT, Texture->GetSizeY() / Sin * FMath::Sin(FMath::DegreesToRadians(90. + Roll)) * PIXEL_TO_UNIT));
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
