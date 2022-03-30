// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AndyActor.h"
#include "AREnemy.generated.h"

UCLASS()
class AR_TEST_API AAREnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAREnemy();

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

	//Variables for movement
	AAndyActor* target;
	float distance;
	float time;
	float speed = 8.0f;
	bool bCanMove = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* CollisionComponent;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void setBCanMove(bool value);

};
