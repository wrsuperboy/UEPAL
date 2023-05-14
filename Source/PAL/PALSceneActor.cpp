// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSceneActor.h"
#include "PAL.h"

// Sets default values
APALSceneActor::APALSceneActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TileMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PAL_Tile.PAL_Tile'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> TileMaterialRef(TEXT("/Script/Engine.Material'/Game/PAL_M_Tile.PAL_M_Tile'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DecoratorMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PAL_Plane.PAL_Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> DecoratorMaterialRef(TEXT("/Script/Engine.Material'/Game/PAL_M_Decorator.PAL_M_Decorator'"));
	TileMesh = TileMeshRef.Object;
	TileMaterial = TileMaterialRef.Object;
	DecoratorMesh = DecoratorMeshRef.Object;
	DecoratorMaterial = DecoratorMaterialRef.Object;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

void APALSceneActor::SetTile(SIZE_T X, SIZE_T Y, SIZE_T H, const FPALPosition3d& Position, UTexture2D* Texture)
{
	UStaticMeshComponent* StaticMeshComponent = TileComponents[X * 128 * 2 + Y * 2 + H];
	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(StaticMeshComponent->GetMaterial(0));
	static FName TextureParameterName("PAL_Texture2D");
	DynamicMaterial->SetTextureParameterValue(TextureParameterName, Texture);
	StaticMeshComponent->SetRelativeLocation(Position.toLocation());
	StaticMeshComponent->SetRelativeScale3D(FVector3d(32 * PIXEL_TO_UNIT / 100., 32 * PIXEL_TO_UNIT / 100., 1.));
}

void APALSceneActor::AddDecorator(const FPALPosition3d& Position, UTexture2D* Texture)
{
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
	StaticMeshComponent->SetStaticMesh(DecoratorMesh);
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DecoratorMaterial, nullptr);
	static FName TextureParameterName("PAL_Texture2D");
	DynamicMaterial->SetTextureParameterValue(TextureParameterName, Texture);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetRelativeLocation(Position.toLocation() + FVector3d(0, Texture->GetSizeY() * PIXEL_TO_UNIT / 2 / 2. * 3., Texture->GetSizeY() * PIXEL_TO_UNIT / 2 / 2. * SQRT_3));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(Texture->GetSizeX() * PIXEL_TO_UNIT / 100., Texture->GetSizeY() * PIXEL_TO_UNIT / 100., 1.));
	StaticMeshComponent->SetRelativeRotation(FRotator(0., 0., 60.));
	StaticMeshComponent->RegisterComponent();
	DecoratorMeshComponents.Add(StaticMeshComponent);
}

void APALSceneActor::ClearDecorators()
{
	for (UStaticMeshComponent* DecoratorComponent : DecoratorMeshComponents)
	{
		DecoratorComponent->DestroyComponent();
	}
	DecoratorMeshComponents.Empty();
}

// Called when the game starts or when spawned
void APALSceneActor::BeginPlay()
{
	Super::BeginPlay();
}

void APALSceneActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	TileComponents.SetNumUninitialized(64 * 2 * 128);
	for (SIZE_T X = 0; X < 64; X++)
	{
		for (SIZE_T H = 0; H < 2; H++)
		{
			for (SIZE_T Y = 0; Y < 128; Y++)
			{
				UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass());
				StaticMeshComponent->SetStaticMesh(TileMesh);
				UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(TileMaterial, nullptr);
				StaticMeshComponent->SetRelativeLocation(FPALPosition3d((double)X * 32 + (double)H * 16, (double)Y * 32 + (double)H * 16, 0).toLocation());
				StaticMeshComponent->SetMaterial(0, DynamicMaterial);
				StaticMeshComponent->SetupAttachment(RootComponent);
				StaticMeshComponent->RegisterComponent();
				TileComponents[X * 128 * 2 + Y * 2 + H] = StaticMeshComponent;
			}
		}
	}
}

// Called every frame
void APALSceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

