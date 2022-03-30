// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugDrawActor.generated.h"

UCLASS()
class AR_TEST_API ADebugDrawActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADebugDrawActor();
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UMaterial* MaterialComponent;
	UMaterialInstanceDynamic* semiTransparent;
	UMaterialInstanceDynamic* halfTransparent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Bool to check if the image is a MG
	bool bIsMG = false;

	virtual void SetSemiMat();
	virtual void SetHalfMat();

};
