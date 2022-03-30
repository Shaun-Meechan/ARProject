// Fill out your copyright notice in the Description page of Project Settings.


#include "AeroPlaneCollisionActor.h"
#include "AeroPlaneActor.h"
#include "EngineUtils.h"

// Sets default values
AAeroPlaneCollisionActor::AAeroPlaneCollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject < UBoxComponent>(TEXT("BoxComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AAeroPlaneCollisionActor::OnHit);
		CollisionComponent->SetBoxExtent(FVector(20.0, 10.0f, 40.0f));
		RootComponent = CollisionComponent;
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
		(TEXT("StaticMesh'/Game/HelloAR/Assets/Meshes/Andy_Mesh.Andy_Mesh'"));
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
		MeshComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
		MeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AAeroPlaneCollisionActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Find the aeroplane actors
	for (TActorIterator<AAeroPlaneActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		target = *ActorItr;
	}

	//Debug if we found it or not
	if (target->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Found aeroplane!")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-12, 1, FColor::Red, FString::Printf(TEXT("Unable to find Andy!")));
	}

}

// Called every frame
void AAeroPlaneCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if the aeroplane is still alive and if it is move to the location
	//It is very unlikely this will return false but worth checking
	if (target->IsValidLowLevelFast())
	{
		this->SetActorLocation(target->GetActorLocation());
		this->SetActorRotation(target->GetActorRotation());
	}
}

//When we are hit we give the Andy some health then destory the aeroplane and ourselves
void AAeroPlaneCollisionActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, FString::Printf(TEXT("Collision on AeroPlane actor!")));
	target->addHealthToAndy();
	target->Destroy();
	Destroy();
}

