// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "AndyActor.generated.h"

UCLASS()
class AR_TEST_API AAndyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAndyActor();

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UMaterial* MaterialComponent;
	UMaterialInstanceDynamic* DynamicMaterialInst;
	UMaterialInstanceDynamic* BlueMaterialInst;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UARPin* ARPin;

	float health = 1.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool HasValidARPin();

	virtual void AssignARPin(UARPin* newPin);

	virtual void setMatToDefault();

	virtual void setMatToBlue();

	virtual void decrementHealth();

	virtual void incrementHealth(float value);

	UFUNCTION(BlueprintCallable, Category = "MyCategory")
		float getHealth();
};
