// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AndyActor.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "AeroPlaneActor.generated.h"

UCLASS()
class AR_TEST_API AAeroPlaneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAeroPlaneActor();

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UMaterial* MaterialComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		URotatingMovementComponent* rotatingMovementComponent;

	UMaterialInstanceDynamic* DynamicMaterialInst;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AAndyActor* target;
	//float distance;
	//float time;
	float speed = 10.0f;
	bool bCanMove = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void addHealthToAndy();

	//Function to 'pause' & 'unpause' the object.
	//This just stops movement
	UFUNCTION()
		void setBCanMove(bool value);
};
