// Fill out your copyright notice in the Description page of Project Settings.


#include "ARProjectile.h"
#include "Components/AudioComponent.h"

// Sets default values
AARProjectile::AARProjectile()
{
 	//The projectile movement component handles movement so we don't need a tick
	PrimaryActorTick.bCanEverTick = false;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject < UBoxComponent>(TEXT("BoxComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AARProjectile::OnHit);
		CollisionComponent->SetBoxExtent(FVector(2.0, 2.0f, 2.0f));
		RootComponent = CollisionComponent;
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

	if (!ProjectileMovementComponent)
	{
		//Setup the projectileMovementComponent
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
	InitialLifeSpan = 3.0f;

	//Audio stuff
	static ConstructorHelpers::FObjectFinder<USoundCue> fireAudioCue(TEXT("SoundCue'/Game/Audio/gunFire.gunFire'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> explosionAudioCue(TEXT("SoundCue'/Game/Audio/explosion.explosion'"));

	//Store a reference to the cue asset
	FireAudioCue = fireAudioCue.Object;
	ExplosionAudioCue = explosionAudioCue.Object;

	//Create an audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	//Don't play sound as soon as we start
	AudioComponent->bAutoActivate = false;

	//Particle system stuff
	FString particleEffectPath = "/Game/FXVarietyPack/Particles/P_ky_explosion";
	particleSystem = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), NULL, *particleEffectPath));
}

// Called when the game starts or when spawned
void AARProjectile::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->Play();
	
}

void AARProjectile::PostInitializeComponents()
{
	AActor::PostInitializeComponents();

	//Make sure the audio component is valid, if it is set the audio to the fire sfx
	if (AudioComponent->IsValidLowLevel())
	{
		AudioComponent->SetSound(FireAudioCue);
	}
}

// Called every frame
void AARProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AARProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AARProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//When we hit something, stop the fire audio and switch to the explosion one
	AudioComponent->Stop();
	AudioComponent->SetSound(ExplosionAudioCue);
	AudioComponent->Play();
	//Check if we can actually spawn the particles - this is an optimisation
	if (bCanSpawnParticles == true)
	{
		particleSystemComponent = UGameplayStatics::SpawnEmitterAttached(particleSystem, RootComponent, NAME_None, GetActorLocation(), FRotator::ZeroRotator, EAttachLocation::KeepWorldPosition, false);
	}
	//Destroy the visible component and collision component. This makes it look like the missile is gone but isn't
	//This is needed to play the audio.
	CollisionComponent->DestroyComponent();
	MeshComponent->DestroyComponent();
}

