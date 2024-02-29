// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleGroundActor.h"
#include "PAL.h"

// Sets default values
APALBattleGroundActor::APALBattleGroundActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GroundMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PAL_Plane.PAL_Plane'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> GroundMaterialRef(TEXT("/Script/Engine.Material'/Game/PAL_M_Tile.PAL_M_Tile'"));
	UStaticMesh* GroundMesh = GroundMeshRef.Object;
	UMaterial* GroundMaterial = GroundMaterialRef.Object;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	DynamicMaterial = UMaterialInstanceDynamic::Create(GroundMaterial, nullptr);
	UStaticMeshComponent* StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent11"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(320 * PIXEL_TO_UNIT / 100., 200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent01"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., 200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(-320 * PIXEL_TO_UNIT, 0, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent21"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., 200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(320 * PIXEL_TO_UNIT, 0, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent10"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(0, -200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent12"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(0, 200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent00"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(-320 * PIXEL_TO_UNIT, -200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent02"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(-320 * PIXEL_TO_UNIT, 200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent20"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(320 * PIXEL_TO_UNIT, -200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMeshComponent22"));
	StaticMeshComponent->SetRelativeScale3D(FVector3d(-320 * PIXEL_TO_UNIT / 100., -200 * PIXEL_TO_UNIT / 100. * 2, 1.));
	StaticMeshComponent->SetRelativeLocation(FVector3d(320 * PIXEL_TO_UNIT, 200 * PIXEL_TO_UNIT * 2, 1.));
	StaticMeshComponent->SetStaticMesh(GroundMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

void APALBattleGroundActor::SetGroundTexture(UTexture2D* Texture)
{
	static FName TextureParameterName("PAL_Texture2D");
	DynamicMaterial->SetTextureParameterValue(TextureParameterName, Texture);
}

// Called when the game starts or when spawned
void APALBattleGroundActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APALBattleGroundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

