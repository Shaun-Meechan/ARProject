// Fill out your copyright notice in the Description page of Project Settings.
//Overall this class is just a copy of the BP version

#include "AndyActor.h"
#include "ARPin.h"
#include "EngineUtils.h"
#include "BPEventManager.h"

// Sets default values
AAndyActor::AAndyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	//MeshComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MeshComponent->SetupAttachment(SceneComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
	(TEXT("StaticMesh'/Game/HelloAR/Assets/Meshes/Andy_Mesh.Andy_Mesh'"));
	MeshComponent->SetStaticMesh(MeshAsset.Object);
	MeshComponent->SetWorldScale3D(FVector(2, 2, 2));
	//MeshComponent->SetRelativeScale3D(FVector(50, 50, 50));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
	(TEXT("Material'/Game/HelloAR/Assets/Materials/Andy_Mat_Default.Andy_Mat_Default'"));
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
	MeshComponent->SetMaterial(0, DynamicMaterialInst);

}

// Called when the game starts or when spawned
void AAndyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAndyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Make sure the AR pin is still valid and if it's not hide ourselves
	if (HasValidARPin())
	{
		EARTrackingState state = ARPin->GetTrackingState();
		if (state == EARTrackingState::Tracking)
		{
			SceneComponent->SetVisibility(true);
			SetActorTransform(ARPin->GetLocalToWorldTransform());
		}
		else if (state == EARTrackingState::StoppedTracking)
		{
			AssignARPin(nullptr);
		}
	}
	else
	{
		SceneComponent->SetVisibility(false);
	}

}

bool AAndyActor::HasValidARPin()
{
	return ARPin->IsValidLowLevel();
}

void AAndyActor::AssignARPin(UARPin* newPin)
{
	if (ARPin->IsValidLowLevel())
	{
		UARBlueprintLibrary::RemovePin(ARPin);
		ARPin = newPin;
	}
	else
	{
		ARPin = newPin;
	}
}

void AAndyActor::setMatToDefault()
{
	MeshComponent->SetMaterial(0,DynamicMaterialInst);
}

void AAndyActor::setMatToBlue()
{
	MeshComponent->SetMaterial(0, BlueMaterialInst);
}

void AAndyActor::decrementHealth()
{
	//GEngine->AddOnScreenDebugMessage(-21, 1, FColor::Blue, FString::Printf(TEXT("Andy health b4: %f"), health));
	health -= 0.1;

	if (health <= 0.0f)
	{
		//Game over!
		//Call BP to clear UI and display you lost 
		for (TActorIterator<ABPEventManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ABPEventManager* temp = *ActorItr;
			temp->bpPlayerLost();
		}
	}
	//GEngine->AddOnScreenDebugMessage(-22, 1, FColor::Blue, FString::Printf(TEXT("Andy health after: %f"), health));
}

void AAndyActor::incrementHealth(float value)
{
	health += value;
}

float AAndyActor::getHealth()
{
	return health;
}
