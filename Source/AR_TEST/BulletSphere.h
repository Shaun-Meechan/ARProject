// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletSphere.generated.h"

UCLASS()
class AR_TEST_API ABulletSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletSphere();

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UMaterial* MaterialComponent;
	UMaterialInstanceDynamic* DynamicMaterialInst;
	UMaterialInstanceDynamic* redMaterial;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void setRedMat();
	virtual void setWhiteMat();

};
