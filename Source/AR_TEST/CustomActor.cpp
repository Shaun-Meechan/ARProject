// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomActor.h"

// Sets default values
ACustomActor::ACustomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MeshComponent->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
	(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	MeshComponent->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
	(TEXT("Material'/Game/HandheldARBP/Materials/M_BackgroundFade.M_BackgroundFade'"));
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
	MeshComponent->SetMaterial(0, DynamicMaterialInst);
}

// Called when the game starts or when spawned
void ACustomActor::BeginPlay()
{
	Super::BeginPlay();
	
	newTime = 0.0f;
}

// Called every frame
void ACustomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector v = GetActorLocation();
	newTime += DeltaTime;
	float scale = 2 / (3 - cos(2 * newTime));
	float x = scale * cos(newTime);
	float y = scale * sin(2 * newTime) / 2;
	v.X = (x * 20) - 100;
	v.Y = (y * 20);
	//SetActorLocation(v);

	FMatrix mat_init;
	mat_init = FMatrix::Identity;
	mat_init.M[3][0] = actorPos.X;
	mat_init.M[3][1] = actorPos.Y;
	mat_init.M[3][2] = actorPos.Z;

	FMatrix mat_moving = FMatrix::Identity;
	mat_moving.M[3][0] = v.X;
	mat_moving.M[3][1] = v.Y;
	mat_moving.M[3][2] = v.Z;

	FMatrix mat_final = mat_moving * mat_init;
	SetActorTransform(FTransform(mat_final));
}

void ACustomActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	actorPos = GetActorLocation();
}

