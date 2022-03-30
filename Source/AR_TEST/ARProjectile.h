// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "ARProjectile.generated.h"

UCLASS()
class AR_TEST_API AARProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARProjectile();

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UMaterial* MaterialComponent;

	UMaterialInstanceDynamic* DynamicMaterialInst;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Used for the audio comp
	virtual void PostInitializeComponents() override;

	UPROPERTY(BlueprintReadOnly)
		UParticleSystemComponent* particleSystemComponent = nullptr;

	UPROPERTY()
		UParticleSystem* particleSystem = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Box collision component
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* CollisionComponent;

	UPROPERTY(Category = "Movement", VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(Category = "Audio", VisibleAnywhere)
		USoundCue* FireAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere)
		USoundCue* ExplosionAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere)
		UAudioComponent* AudioComponent;

	void FireInDirection(const FVector& ShootDirection);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	bool bCanSpawnParticles = true;
};
