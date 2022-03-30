// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Math/Color.h"
#include "BPEventManager.generated.h"

UCLASS()
class AR_TEST_API ABPEventManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABPEventManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Used for the audio comp
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Functions to be implemented in BP:
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		void bpUpdatePlaneActors();

	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		void bpResetARCoreSession();	

	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		void bpHideARPlanes();	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		void bpPlayerLost();

	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		void bpChangeButtonColour();

	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		FColor bpGetARLightingColor();

	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		float bpGetARLightingColorTempKelvins();	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
		float bpGetARLightingIntensityLumens();

	UPROPERTY(Category = "Audio", VisibleAnywhere)
		USoundCue* backgroundAudioCue;

	UPROPERTY(Category = "Audio", VisibleAnywhere)
		UAudioComponent* AudioComponent;
};
