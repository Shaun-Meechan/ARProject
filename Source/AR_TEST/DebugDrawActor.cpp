// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugDrawActor.h"

// Sets default values
ADebugDrawActor::ADebugDrawActor()
{
 	//This actor is used to appear over images and as such doesn't need to tick
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
	(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	MeshComponent->SetStaticMesh(MeshAsset.Object);
	MeshComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/semiTransparent.semiTransparent'"));
	semiTransparent = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
	MeshComponent->SetMaterial(0, semiTransparent);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundRedMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/halfTransparent.halfTransparent'"));
	halfTransparent = UMaterialInstanceDynamic::Create(FoundRedMaterial.Object, MeshComponent);

	MeshComponent->SetVisibility(false);
}

// Called when the game starts or when spawned
void ADebugDrawActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADebugDrawActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADebugDrawActor::SetSemiMat()
{
	MeshComponent->SetMaterial(0, semiTransparent);
}

void ADebugDrawActor::SetHalfMat()
{
	MeshComponent->SetMaterial(0, halfTransparent);
}

