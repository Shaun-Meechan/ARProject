// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletSphere.h"

// Sets default values
ABulletSphere::ABulletSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MeshComponent->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
	(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	MeshComponent->SetStaticMesh(MeshAsset.Object);
	MeshComponent->SetWorldScale3D(FVector(0.25f, 0.25f, 0.25f));
	MeshComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	//MeshComponent->SetWorldScale3d(FVector(0.5, 0.5f, 0.5));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/M_BackgroundFade.M_BackgroundFade'"));
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
	MeshComponent->SetMaterial(0, DynamicMaterialInst);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundRedMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/Red.Red'"));
	redMaterial = UMaterialInstanceDynamic::Create(FoundRedMaterial.Object, MeshComponent);
}

// Called when the game starts or when spawned
void ABulletSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletSphere::setRedMat()
{

	MeshComponent->SetMaterial(0, redMaterial);

}
void ABulletSphere::setWhiteMat()
{

	MeshComponent->SetMaterial(0, DynamicMaterialInst);
}

