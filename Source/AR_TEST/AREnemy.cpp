// Fill out your copyright notice in the Description page of Project Settings.


#include "AREnemy.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "ARProjectile.h"
#include "AREnemy.h"
#include "CustomARPawn.h"
#include "EngineUtils.h"
#include "Math/Vector.h"
#include "Math/UnrealMathUtility.h"


// Sets default values
AAREnemy::AAREnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject < UBoxComponent>(TEXT("BoxComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AAREnemy::OnHit);
		//Make the box larger, this makes it easier for the player to hit the enemy
		CollisionComponent->SetBoxExtent(FVector(15.0f, 15.0f, 25.0f));
		RootComponent = CollisionComponent;
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset
		(TEXT("StaticMesh'/Game/Models/Zombie.Zombie'"));
		if (MeshAsset.Succeeded())
		{
			MeshComponent->SetStaticMesh(MeshAsset.Object);
		}

		static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial
		(TEXT("Material'/Game/Models/ZombieMat.ZombieMat'"));
		if (FoundMaterial.Succeeded())
		{
			DynamicMaterialInst = UMaterialInstanceDynamic::Create(FoundMaterial.Object, MeshComponent);
		}
		MeshComponent->SetMaterial(0, DynamicMaterialInst);
		MeshComponent->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
		MeshComponent->SetRelativeRotation(FRotator(0.0f, 100.0f, 80.0f));
		MeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AAREnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//Find the Andy
	for (TActorIterator<AAndyActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		target = *ActorItr;
	}
	
	//Make sure the Andy is valid, if it is calculate the distance between us and it
	if (target->IsValidLowLevel())
	{
		distance = FVector::Distance(this->GetActorLocation(), target->GetActorLocation());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-12, 1, FColor::Red, FString::Printf(TEXT("Unable to find Andy!")));
	}
}

// Called every frame
void AAREnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	time += (DeltaTime * speed);

	//Check if the game is paused
	if (bCanMove == false)
	{
		return;
	}

	//Check the Andy still exists
	if (target->IsValidLowLevelFast())
	{
		//If the Andy still exists move towards it
		FVector actorLoc = GetActorLocation();
		SetActorLocation(FMath::VInterpConstantTo(actorLoc, target->GetActorLocation(), DeltaTime, speed));
		//If we reach the Andy subtract some health and destroy ourselves(The remove enemy from Array fucntion handles this)
		if (GetActorLocation() == target->GetActorLocation())
		{
			for (TActorIterator<AAndyActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				AAndyActor* temp = *ActorItr;
				temp->decrementHealth();
				UE_LOG(LogTemp, Warning, TEXT("Lowering Andy health"));
			}

			for (TActorIterator<ACustomARPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				ACustomARPawn* temp = *ActorItr;
				temp->RemoveEnemyFromArray(this);
			}
		}
	}
	else
	{
		return;
	}
}

void AAREnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//If we were hit destory ourselves
	for (TActorIterator<ACustomARPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ACustomARPawn* temp = *ActorItr;
		temp->RemoveEnemyFromArray(this);
	}
}

void AAREnemy::setBCanMove(bool value)
{
	bCanMove = value;
}

