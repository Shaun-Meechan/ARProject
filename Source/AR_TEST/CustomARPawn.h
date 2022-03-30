// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DebugDrawActor.h"
#include "ARPlaneActor.h"
#include "ARSessionConfig.h"
#include "ARTrackable.h"
#include "ARBlueprintLibrary.h"
#include "AndyActor.h"
#include "ARProjectile.h"
#include "AeroPlaneActor.h"
#include "BPEventManager.h"
#include "AREnemy.h"
#include "GameFramework/PlayerController.h"
#include "Engine/DirectionalLight.h"
#include "CustomARPawn.generated.h"

//Enum to describe the game state.
UENUM()
enum GameState
{
	movingAndy UMETA(DisplayName = "Moving Andy"),
	mainState UMETA(DisplayName = "Main State"),
	paused UMETA(DisplayName = "Paused")
};

UCLASS()
class AR_TEST_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* sceneComponent;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* cameraComponent;

	// Projectile class to spawn.
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		TSubclassOf<class AARPlaneActor> ARActorClass;

	//Array of actors in scene
	TArray<AAREnemy*> enemiesInScene;
	TArray<AAeroPlaneActor*> AeroPlanesInScene;

	//Array of tracked images
	TArray<UARTrackedImage*>  trackedImages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
		TEnumAsByte<GameState> gameState = movingAndy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//The AR session config
	UARSessionConfig* configAsset;
	//The tracked planes
	TArray<FARTraceResult> trackedObjects;
	//Should we continue looking for planes?
	//No if we are not placing the Andy
	bool shouldTrace = true;
	//TArray<ADebugDrawActor*> candidatePlanes;

	//Can we move the Andy?
	//No if the player has confirmed the placement
	bool bCanSpawnAndy = true;
	//The Andy acting as the base
	AActor* andyActor;
	APlayerController* localPlayerController;
	//Plane colours, same at the BP
	TArray<FColor> planeColours;
	//Used for moving the Andy actor
	TArray<AAndyActor*> spawnedActors;
	AAndyActor* selectedAndy;

	//Spawn timers for enemies and aeroplanes
	float enemySpawnTimer = 0.0f;
	float planeSpawnTimer = 0.0f;

	//AARPlaneActor* tempARActor;

	//Refence to the BP manager for BP functions
	ABPEventManager* BPManager;

	//Is the game paused?
	bool bIsPaused = false;

	//Is the finger held down?
	bool bIsFingerHeldDown = false;

	//An integer representing how many enemies we can have spawned at one time
	uint8 maxEnemies = 10;
	uint8 maxAeroPlanes = 2;

	//IMAGE TRACKING
	bool fastWeapon = false;
	bool foundMG = false;
	bool bFoundRocket = false;

	//LIGHTING
	ADirectionalLight* sceneDirectionalLight;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Do something when the screen is touched
	virtual void OnScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos);

	//Player took finger off scree
	virtual void OffScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos);

	virtual bool worldHitTest(FVector2D touchPos, FHitResult& hitResult, float traceVectorMultiplier = 1000.0f);

	//Set the Andy actor var
	virtual void setAndyActor(AActor* newAndy);

	//Replaces the BP implementation of the same name
	virtual void updatePlanesAndAnchors(float DeltaTime);

	//This function clears up all the data made by an AR session
	UFUNCTION(BlueprintCallable, Category = "MyCategory")
	void ResetARCoreSession();

	//This function replaces the BP implemnation of the same name
	virtual void UpdatePlaneActors();

	//This returns a FVector4 representing a colour
	UFUNCTION(BlueprintCallable, Category = "MyCategory")
	FColor GetPlaneColor(int index);

	//Place an AndyAnchorActor when touch the screen function from BP
	virtual void testTraceTrackedObjects();

	//This function spawns an andy on screen.
	virtual bool spawnAndy(FARTraceResult* traceResult);

	virtual bool isHittingFrontSurface(FARTraceResult* traceResult, APlayerController* playerController, FVector2D screenPosition);

	//Functions that can be called from BPs
	UFUNCTION(BlueprintCallable, Category="MyCategory")
	void Fire(bool shouldSpawnAtPlayerTouchPoint) const;

	UFUNCTION(BlueprintCallable, Category="MyCategory")
	void Pause();

	UFUNCTION(BlueprintCallable, Category="MyCategory")
	void UnPause();	
	
	UFUNCTION(BlueprintCallable, Category="MyCategory")
	void SetGameState(GameState newState);

	UFUNCTION(BlueprintCallable, Category="MyCategory")
	bool getFastWeapon();

	UFUNCTION(BlueprintCallable, Category="MyCategory")
	void setFastWeapon(bool value);

	//This function handles spawning enemies
	virtual void HandleEnemies(float DeltaTime);

	//This function handles spawning aero planes
	virtual void HandleAeroPlanes(float DeltaTime);

	virtual void RemoveEnemyFromArray(AAREnemy* enemy);
};
