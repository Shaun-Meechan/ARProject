// Fill out your copyright notice in the Description page of Project Settings.


#include "AeroPlaneActor.h"
#include "EngineUtils.h"

// Sets default values
AAeroPlaneActor::AAeroPlaneActor()
{
 	//This actor doesn't actually need a tick since the movement component handles frame movement
	PrimaryActorTick.bCanEverTick = false;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
		(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
		if (MeshAsset.Succeeded())
		{
			MeshComponent->SetStaticMesh(MeshAsset.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
		(TEXT("Material'/Game/Materials/AeroPlaneMaterial.AeroPlaneMaterial'"));
		if (FoundMaterial.Succeeded())
		{
			DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
		}
		MeshComponent->SetMaterial(0, DynamicMaterialInst);
		MeshComponent->SetRelativeScale3D(FVector(0.25f, 0.1f, 0.1f));
		MeshComponent->SetupAttachment(RootComponent);
	}

	if (!rotatingMovementComponent)
	{
		rotatingMovementComponent = CreateAbstractDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	}
}

// Called when the game starts or when spawned
void AAeroPlaneActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Find the andy
	for (TActorIterator<AAndyActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		target = *ActorItr;
	}

	//If we found the Andy setup the rotation movement component
	if (target->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(-12, 1, FColor::Green, FString::Printf(TEXT("Found Andy!")));
		//distance = FVector::Distance(this->GetActorLocation(), target->GetActorLocation());
		rotatingMovementComponent->PivotTranslation = FVector(100.0f,0.0f,0.0f);
		rotatingMovementComponent->RotationRate = FRotator(0, speed, 0);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-12, 1, FColor::Red, FString::Printf(TEXT("Unable to find Andy!")));
	}
	
}

// Called every frame
void AAeroPlaneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAeroPlaneActor::addHealthToAndy()
{
	target->incrementHealth(0.1f);
}

void AAeroPlaneActor::setBCanMove(bool value)
{
	if (value == true)
	{
		rotatingMovementComponent->RotationRate = FRotator(0, speed, 0);
	}
	else
	{
		rotatingMovementComponent->RotationRate = FRotator(0, 0, 0);
	}
}
