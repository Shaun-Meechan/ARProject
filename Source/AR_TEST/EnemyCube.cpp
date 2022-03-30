// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCube.h"

// Sets default values
AEnemyCube::AEnemyCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
	(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	MeshComponent->SetStaticMesh(MeshAsset.Object);
	MeshComponent->SetWorldScale3D(FVector(1 / scalingFactor, 1 / scalingFactor, 1 / scalingFactor));
	MeshComponent->SetRelativeScale3D(FVector(1 / scalingFactor, 1 / scalingFactor, 1 / scalingFactor));


	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/M_BackgroundFade.M_BackgroundFade'"));
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
	MeshComponent->SetMaterial(0, DynamicMaterialInst);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundRedMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/Red.Red'"));
	redMaterial = UMaterialInstanceDynamic::Create(FoundRedMaterial.Object, MeshComponent);
}

// Called when the game starts or when spawned
void AEnemyCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCube::setRedMat()
{
	MeshComponent->SetMaterial(0, redMaterial);
}

void AEnemyCube::setWhiteMat()
{

	MeshComponent->SetMaterial(0, DynamicMaterialInst);
}
