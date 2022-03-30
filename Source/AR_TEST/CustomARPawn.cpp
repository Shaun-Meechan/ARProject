// Fill out your copyright notice in the Description page of Project Settings.
//NOTE: We have not added selecting an Andy yet. There is no real point unless everything else works!

#include "CustomARPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "CustomActor.h"
#include "ARPlaneActor.h"
#include "ARProjectile.h"
#include "AREnemy.h"
#include "ARTypes.h"
#include "AeroPlaneActor.h"
#include "AeroPlaneCollisionActor.h"
#include "ARPin.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SetRootComponent(cameraComponent);

	static ConstructorHelpers::FObjectFinder<UARSessionConfig>
		config(TEXT("ARSessionConfig'/Game/HelloAR/Blueprints/D_ARSessionConfig.D_ARSessionConfig'"));
	configAsset = config.Object;

	//Fill colour array
	planeColours.Add(FColor(255, 255, 255, 255));//White
	planeColours.Add(FColor(231, 14, 9, 255));//Red
	planeColours.Add(FColor(208, 3, 31, 255));//Pinky red
	planeColours.Add(FColor(85, 5, 111, 255));//Dark purple
	planeColours.Add(FColor(34, 10, 121, 255));//mid purple
	planeColours.Add(FColor(12, 21, 118, 255));//Dark blue
	planeColours.Add(FColor(3, 78, 229, 255));//mid blue
	planeColours.Add(FColor(0, 101, 231, 255));//light blue
	planeColours.Add(FColor(0, 128, 168, 255));//greeny-cyan
	planeColours.Add(FColor(0, 78, 63, 255));//seaweed-green
	planeColours.Add(FColor(18, 109, 20, 255));//green
	planeColours.Add(FColor(66, 139, 17, 255));//light green
	planeColours.Add(FColor(156, 183, 10, 255));//yellowy-green
	planeColours.Add(FColor(255, 212, 11, 255));//yellow
	planeColours.Add(FColor(255, 136, 0, 255));//Light orange
	planeColours.Add(FColor(255, 80, 0, 255));//orange
}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();
	localPlayerController = UGameplayStatics::GetPlayerController(this,0);
	GEngine->AddOnScreenDebugMessage(-1,1, FColor::Blue, FString::Printf(TEXT("Scanning the environment. Please wait.")));

	UARSessionConfig* Config = configAsset;
	Config->bUseSceneDepthForOcclusion = true;
	Config->SetSessionTrackingFeatureToEnable(EARSessionTrackingFeature::SceneDepth);
	UARBlueprintLibrary::StartARSession(configAsset);

	for (TActorIterator<ABPEventManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		BPManager = *ActorItr;
	}

	for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		sceneDirectionalLight = *ActorItr;
	}

	//sceneDirectionalLight->SetLightColor(BPManager->bpGetARLightingColor());

	gameState = GameState::movingAndy;
	UE_LOG(LogTemp, Warning, TEXT("Finished start!"));
}

// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updatePlanesAndAnchors(DeltaTime);
	sceneDirectionalLight->SetLightColor(BPManager->bpGetARLightingColor());

	//IMAGE TRACKING!
	//Load all the tracked images
	trackedImages = UARBlueprintLibrary::GetAllGeometriesByClass<UARTrackedImage>();
	//Fix for finding out what image the player is looking at

	for (int i = 0; i < trackedImages.Num(); i++)
	{
		UARTrackedImage* trackedImage = trackedImages[i];
		if (trackedImage->GetDetectedImage())
		{
			//Image is valid
			if (trackedImage->GetDetectedImage()->GetFriendlyName().Equals("machine-gun") && foundMG == false)
			{
				foundMG = true;
					//fastWeapon = true;
					FActorSpawnParameters SpawnInfo;
					FRotator myRot(0.0f, 0.0f, 0.0f);
					FVector myLoc(trackedImage->GetLocalToWorldTransform().GetLocation());
					ADebugDrawActor* temp = GetWorld()->SpawnActor<ADebugDrawActor>(myLoc, myRot, SpawnInfo);
					temp->bIsMG = true;
			}
			else if (trackedImage->GetDetectedImage()->GetFriendlyName().Equals("rocket") && bFoundRocket == false)
			{
				bFoundRocket = true;
				FActorSpawnParameters SpawnInfo;
				FVector myLoc(trackedImage->GetLocalToWorldTransform().GetLocation());
				FRotator myRot(0.0f, 0.0f, 0.0f);
				ADebugDrawActor* temp = GetWorld()->SpawnActor<ADebugDrawActor>(myLoc, myRot, SpawnInfo);
			}
		}
	}
	
	if (gameState == mainState)
	{
		FHitResult hitResult;
		worldHitTest(FVector2D(0.0f, 0.0f), hitResult, 100000.0f);

		UClass* hitActorClass = UGameplayStatics::GetObjectClass(hitResult.GetActor());
		if (UKismetMathLibrary::ClassIsChildOf(hitActorClass, ADebugDrawActor::StaticClass()))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Attempting cast..."));
			ADebugDrawActor* temp = Cast<ADebugDrawActor>(hitResult.GetActor());
			if (temp != nullptr)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Cast worked!"));
				if (temp->bIsMG == true)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Set fast weapon to true"));
					setFastWeapon(true);
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("Set fast weapon to false"));
					setFastWeapon(false);
				}
			}
			else
			{
				//UE_LOG(LogTemp, Error, TEXT("Casted actor is not valid!"));
				return;
			}
		}
	}

	if (bIsFingerHeldDown && fastWeapon == true)
	{
		Fire(true);
	}
}

// Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACustomARPawn::OffScreenTouch);
}

void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos)
{
	FHitResult hitTestResult;

	if (fastWeapon == true)
	{
		bIsFingerHeldDown = true;
	}

	//Fire a projectile if player touces screen and the state is the main game state
	if (gameState == GameState::mainState)
	{
		Fire(true);
	}

	//Peform a hitTest, get the return values as hitTestResult
	if (!worldHitTest(FVector2D(screenPos), hitTestResult))
	{
		//HitTest returned false, get out
		//return;
	}
	
	//Get object of actor hit
	UClass* hitActorClass = UGameplayStatics::GetObjectClass(hitTestResult.GetActor());

	// if we've hit a target
	if (UKismetMathLibrary::ClassIsChildOf(hitActorClass,AAndyActor::StaticClass()))
	{
		selectedAndy = Cast<AAndyActor>(hitTestResult.GetActor());
		return;
	}
	
	testTraceTrackedObjects();
}

void ACustomARPawn::OffScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos)
{
	bIsFingerHeldDown = false;
}

bool ACustomARPawn::worldHitTest(FVector2D touchPos, FHitResult& hitResult, float traceVectorMultiplier)
{
	//Get the player controller
	//APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);

	//Perform deprojection taking 2D clicked area generating reference in 3D world-space
	FVector worldPosition;
	FVector worldDirection;
	bool deprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(localPlayerController, touchPos, /*out*/ worldPosition,/*out*/ worldDirection);

	//construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceVector = worldDirection * traceVectorMultiplier;
	traceVector = worldPosition + traceVector;

	//perform line trace
	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, worldPosition, traceVector, ECollisionChannel::ECC_WorldDynamic);

	//return if the operation was successful
	return traceSuccess;
}

void ACustomARPawn::setAndyActor(AActor* newAndy)
{
	andyActor = newAndy;
}

void ACustomARPawn::updatePlanesAndAnchors(float DeltaTime)
{
	FARSessionStatus status = UARBlueprintLibrary::GetARSessionStatus();
	if (status.Status == EARSessionStatus::Running)
	{
		//AR is running, update the plane actors!
		UpdatePlaneActors();
		if (gameState == GameState::mainState)
		{
			HandleEnemies(DeltaTime);
			HandleAeroPlanes(DeltaTime);
		}
	}
	else if(status.Status == EARSessionStatus::FatalError)
	{
		//The session encountrered a fatal error! Restart session
		ResetARCoreSession();
		UARSessionConfig* Config = configAsset;
		UARBlueprintLibrary::StartARSession(configAsset);
	}
}

void ACustomARPawn::ResetARCoreSession()
{
	for (int i = 0; i < spawnedActors.Num(); i++)
	{
		spawnedActors[i]->Destroy();
	}
	spawnedActors.Empty();
	selectedAndy = nullptr;

	for (int i = 0; i < enemiesInScene.Num(); i++)
	{
		enemiesInScene[i]->Destroy();
	}
	enemiesInScene.Empty();
	enemySpawnTimer = 0.0f;

	bCanSpawnAndy = true;
	BPManager->bpResetARCoreSession();

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ACustomARPawn::UpdatePlaneActors()
{
	UE_LOG(LogTemp, Warning, TEXT("Started UpdatePlaneActors"));
	if (gameState != GameState::movingAndy)
	{
		return;
	}
	BPManager->bpUpdatePlaneActors();
	UE_LOG(LogTemp, Warning, TEXT("Finished UpdatePlaneActors"));
	return;

	/*
	EARTrackingState state;
	bool bFoundGeo = false;
	int topLevelIndex = 0;
	int actorIndex = 0;

	TArray<UARPlaneGeometry*> geoArray;
	geoArray = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	GEngine->AddOnScreenDebugMessage(-14, 1, FColor::Blue, FString::Printf(TEXT("geoArray Length: %i"), geoArray.Num()));
	UARPlaneGeometry* updatedPlane;

	for (int i = 0; i < geoArray.Num(); i++)
	{
		if (geoArray[i]->IsValidLowLevel() == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Geo array contained non valid object"));
			continue;
		}
		updatedPlane = geoArray[i];
		GEngine->AddOnScreenDebugMessage(-13, 1, FColor::Blue, FString::Printf(TEXT("Entered main loop")));
		state = updatedPlane->GetTrackingState();
		bFoundGeo = false;

			if (this->IsInMap(updatedPlane) == true)
			{
				GEngine->AddOnScreenDebugMessage(-19, 1, FColor::Green, FString::Printf(TEXT("Found plane")));
				if (updatedPlane->GetSubsumedBy()->IsValidLowLevel())
				{
					//Destroy();
					//planeActorArray.RemoveAt(actorIndex);
					GEngine->AddOnScreenDebugMessage(-19, 1, FColor::Blue, FString::Printf(TEXT("Plane was subsumed")));
				}
				else
				{
					if (state == EARTrackingState::Tracking)
					{
						//planeActorArray[actorIndex]->UpdatePlanePolygonMesh();
						this->getFromMap(updatedPlane);
						GEngine->AddOnScreenDebugMessage(-18, 1, FColor::Blue, FString::Printf(TEXT("Updating plane mesh")));
					}
					else if (state == EARTrackingState::StoppedTracking)
					{
						//Destroy();
						//planeActorArray.RemoveAt(actorIndex);
						GEngine->AddOnScreenDebugMessage(-17, 1, FColor::Blue, FString::Printf(TEXT("Plane was stoped tracking")));
					}
				}
			}
			else
			{
				//Create a new plane actor to visualize the plane if it hasn't been created
				if (state == EARTrackingState::Tracking)
				{
					if (updatedPlane->GetSubsumedBy()->IsValidLowLevelFast() == false)
					{
						GEngine->AddOnScreenDebugMessage(-16, 1, FColor::Blue, FString::Printf(TEXT("Didn't find plane, making a new one")));
						//UE_LOG(LogTemp, Warning, TEXT("Making spawn info"));
						//ISSUE: Unreal crashes when trying to spawn actor.
						// SOLUTION: Spawn Actor in BP then pass into this CPP
						GEngine->AddOnScreenDebugMessage(-12, 1, FColor::Blue, FString::Printf(TEXT("About to try spawn actor..")));
						UE_LOG(LogTemp, Warning, TEXT("Trying to spawn actor using BP"));
						AARPlaneActor* temp = this->spawnARActor();
						if (temp->IsValidLowLevel())
						{
							UE_LOG(LogTemp, Warning, TEXT("Setting colour"));
							temp->SetColor(GetPlaneColor(topLevelIndex));
							UE_LOG(LogTemp, Warning, TEXT("Checking if it was added to the map"));
							if (this->addToMap(updatedPlane, temp) == true)
							{
								UE_LOG(LogTemp, Warning, TEXT("It was added to map!"));
								GEngine->AddOnScreenDebugMessage(-15, 1, FColor::Blue, FString::Printf(TEXT("Actor spawned!")));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("it wasn't added to map!"));
								GEngine->AddOnScreenDebugMessage(-15, 1, FColor::Red, FString::Printf(TEXT("ERROR: Failed to add actor to map!")));
							}
							UE_LOG(LogTemp, Warning, TEXT("Updating poly mesh"));
							//temp->UpdatePlanePolygonMesh();
							UE_LOG(LogTemp, Warning, TEXT("Incrementing topLevelIndex"));
							topLevelIndex++;
							UE_LOG(LogTemp, Warning, TEXT("Finished spawning actor"));
							if (temp->IsValidLowLevelFast() == true)
							{
								UE_LOG(LogTemp, Warning, TEXT("Confirmed temp actor is valid"));
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("Temp actor is no longer valid!"));
							}
						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-15, 1, FColor::Red, FString::Printf(TEXT("Actor not valid!")));
							UE_LOG(LogTemp, Warning, TEXT("Temp actor is not valid!"));
							temp = nullptr;
						}
						//if (ARActorClass.Get()->IsValidLowLevel())
						//{
						//	GEngine->AddOnScreenDebugMessage(-11, 1, FColor::Blue, FString::Printf(TEXT("Entered spawn function")));
						//	FActorSpawnParameters SpawnInfo;
						//	SpawnInfo.Owner = this;
						//	SpawnInfo.Instigator = GetInstigator();
						//	//UE_LOG(LogTemp, Warning, TEXT("Making rot info"));
						//	FRotator myRot(0.0f, 0.0f, 0.0f);
						//	//UE_LOG(LogTemp, Warning, TEXT("Making loc info"));
						//	FVector myLoc(0.0f, 0.0f, 0.0f);
						//	//UE_LOG(LogTemp, Warning, TEXT("spawning actor"));
						//	AARPlaneActor* temp = GetWorld()->SpawnActor<AARPlaneActor>(ARActorClass, myLoc, myRot, SpawnInfo);
						//	temp->SetColor(GetPlaneColor(topLevelIndex));
						//	//NOTE: We didn't set "ARCore Plane Object to be temp as is done in the BP, it's not clear what this does anyway
						//	//planeActorArray.Add(temp);
						//	GEngine->AddOnScreenDebugMessage(-13, 1, FColor::Blue, FString::Printf(TEXT("About to add actor to map...")));
						//	if (this->addToMap(updatedPlane, temp) == true)
						//	{
						//		GEngine->AddOnScreenDebugMessage(-15, 1, FColor::Blue, FString::Printf(TEXT("Actor spawned!")));
						//	}
						//	else
						//	{
						//		GEngine->AddOnScreenDebugMessage(-15, 1, FColor::Red, FString::Printf(TEXT("ERROR: Failed to add actor to map!")));
						//	}
						//	GEngine->AddOnScreenDebugMessage(-14, 1, FColor::Blue, FString::Printf(TEXT("Actor should have been spawned!")));
						//	temp->UpdatePlanePolygonMesh();
						//	topLevelIndex++;
						//}
						//else
						//{
						//	GEngine->AddOnScreenDebugMessage(-16, 1, FColor::Red, FString::Printf(TEXT("ERROR: ARActor class is not valid")));
						//}
						//UE_LOG(LogTemp, Warning, TEXT("Printing debug"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("updatedPlane->GetSubsumedBy()->IsValidLowLevelFast() retruned true (which should be fine)"));
					}
				}
			}

	}

	//for (UARPlaneGeometry* plane : UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>())
	//{
	//}
				*/
}

		FColor ACustomARPawn::GetPlaneColor(int index)
		{
			int numColours = planeColours.Num();
			int arrayIndex = index % numColours;
			return planeColours[arrayIndex];
		}

		void ACustomARPawn::testTraceTrackedObjects()
		{
			if (gameState != GameState::movingAndy)
			{
				return;
			}
			float posX = 0.0f;
			float posY = 0.0f;
			bool bIsCurrentlyDown = false;
			localPlayerController->GetInputTouchState(ETouchIndex::Touch1, posX, posY, bIsCurrentlyDown);
			FVector2D screenCoord = FVector2D(posX, posY);
			TArray<FARTraceResult> result = UARBlueprintLibrary::LineTraceTrackedObjects(screenCoord,false,false,false);
			if (result.IsValidIndex(0))
			{
				if (isHittingFrontSurface(&result[0], localPlayerController, screenCoord))
				{
					//spawn andy
					if (bCanSpawnAndy == true)
					{
						if (spawnAndy(&result[0]) == false)
						{
							GEngine->AddOnScreenDebugMessage(-4, 1, FColor::Red, FString::Printf(TEXT("Failed to spawn Actor!")));
						}
						else
						{
							selectedAndy = spawnedActors[spawnedActors.Num() - 1];
						}
					}
					else
					{
						if (selectedAndy->IsValidLowLevel())
						{

							selectedAndy->Destroy();
							spawnedActors[0]->Destroy();
							spawnAndy(&result[0]);
							selectedAndy = spawnedActors[spawnedActors.Num() - 1];

						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-4, 3, FColor::Red, FString::Printf(TEXT("Andy is not valid!")));
						}
						return;
					}
				}
			}
		}

		bool ACustomARPawn::spawnAndy(FARTraceResult * traceResult)
		{
			//We had to add the scene component here, not done in BP so this could cause an error!
			UARPin* pin = UARBlueprintLibrary::PinComponent(sceneComponent, traceResult->GetLocalToWorldTransform(), traceResult->GetTrackedGeometry());

			if (pin->IsValidLowLevel())
			{
				FActorSpawnParameters SpawnInfo;
				FRotator myRot(0.0f, 0.0f, 0.0f);
				FVector myLoc = pin->GetLocalToWorldTransform().GetLocation();
				AAndyActor* temp = GetWorld()->SpawnActor<AAndyActor>(myLoc, myRot, SpawnInfo);
				if (UARTrackedPoint* point = Cast<UARTrackedPoint>(traceResult->GetTrackedGeometry()))
				{
					temp->setMatToBlue();
				}
				else
				{
					temp->setMatToDefault();
				}

				temp->AssignARPin(pin);
				spawnedActors.Add(temp);
				bCanSpawnAndy = false;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool ACustomARPawn::isHittingFrontSurface(FARTraceResult * traceResult, APlayerController * playerController, FVector2D screenPosition)
		{
			FVector upVector = traceResult->GetLocalToWorldTransform().GetRotation().GetUpVector();
			FVector worldPosition;
			FVector worldDirection;
			//We actually only want the world direction so just ignore the rest of the data
			bool temp = UGameplayStatics::DeprojectScreenToWorld(playerController, screenPosition, worldPosition, worldDirection);
			if (FVector::DotProduct(upVector, worldDirection) < 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		void ACustomARPawn::Fire(bool shouldSpawnAtPlayerTouchPoint) const
		{
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);
			UWorld* World = GetWorld();
			float posX = 0.0f;
			float posY = 0.0f;
			bool bIsCurrentlyDown = false;
			FVector worldPosition;
			FVector worldDirection;
			if (World)
			{
				FActorSpawnParameters SpawnInfo;
				localPlayerController->GetInputTouchState(ETouchIndex::Touch1, posX, posY, bIsCurrentlyDown);
				FVector2D screenCoord = FVector2D(posX, posY);
				bool temp = UGameplayStatics::DeprojectScreenToWorld(localPlayerController, screenCoord, worldPosition, worldDirection);
				FRotator rotation = CameraRotation;
				//rotation.Pitch -= 45.0f;
				AARProjectile* Projectile;
				int32 viewX, viewY;
				localPlayerController->GetViewportSize(viewX, viewY);
				FVector myLoc = FVector(worldPosition.X, worldPosition.Y, worldPosition.Z);
				Projectile = World->SpawnActor<AARProjectile>(myLoc, rotation, SpawnInfo);
				if (fastWeapon == true)
				{
					Projectile->bCanSpawnParticles = false;
				}
				else
				{
					Projectile->bCanSpawnParticles = true;
				}
				//if (shouldSpawnAtPlayerTouchPoint == false)
				//{
				//	//Middle of screen
				//}
				//else
				//{
				//	FVector myLoc = FVector(0, screenCoord.X, screenCoord.Y);
				//	//This is moving with the players touch point but not quite right
				//	//The Y & Z are the issue here. They seem to be a 10* smaller than they should be?
				//	//FVector myLoc = FVector(worldPosition.X, worldPosition.Y, worldPosition.Z);
				//	Projectile = World->SpawnActor<AARProjectile>(myLoc, rotation, SpawnInfo);
				//}

				//GEngine->AddOnScreenDebugMessage(-2, 1, FColor::Blue, FString::Printf(TEXT("World pos data: %f %f %f"), worldPosition.X, worldPosition.Y, worldPosition.Z));
				//GEngine->AddOnScreenDebugMessage(-3, 1, FColor::Blue, FString::Printf(TEXT("Screen cord data pos data: %f %f "), screenCoord.X, screenCoord.Y));
				//GEngine->AddOnScreenDebugMessage(-4, 1, FColor::Blue, FString::Printf(TEXT("Screen size: %i %i "), viewX, viewX));

				if (Projectile)
				{
					FVector LaunchDirection = CameraRotation.Vector();
					FVector direction = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Vector();
					Projectile->FireInDirection(direction);
				}
			}
		}

		void ACustomARPawn::Pause()
		{
			bIsPaused = true;
			//Tell all actors/pawns that the game is paused
			for (int i = 0; i < enemiesInScene.Num(); i++)
			{
				enemiesInScene[i]->setBCanMove(false);
			}

			for (int i = 0; i < AeroPlanesInScene.Num(); i++)
			{
				AeroPlanesInScene[i]->setBCanMove(false);
			}
		}

		void ACustomARPawn::UnPause()
		{
			bIsPaused = false;
			//tell all actors/pawns that the game has been unpaused
			for (int i = 0; i < enemiesInScene.Num(); i++)
			{
				enemiesInScene[i]->setBCanMove(true);
			}

			for (int i = 0; i < AeroPlanesInScene.Num(); i++)
			{
				AeroPlanesInScene[i]->setBCanMove(true);
			}
		}

		void ACustomARPawn::SetGameState(GameState newState)
		{
			gameState = newState;
			if (gameState == GameState::mainState)
			{
				//Clear the AR planes as the player doens't need to see them anymore
				//BPManager->bpResetARCoreSession();
				BPManager->bpHideARPlanes();
				setFastWeapon(fastWeapon);
				if (selectedAndy->IsValidLowLevelFast() == true)
				{
					//FRotator myRot(0.0f, 0.0f, 0.0f);
					////We need to move the plane away from Andy into the sky
					//FVector myLoc = selectedAndy->GetActorLocation();
					//myLoc.Z += 125;
					//FActorSpawnParameters SpawnInfo;
					//AAeroPlaneActor* tempPlane = GetWorld()->SpawnActor<AAeroPlaneActor>(myLoc, myRot, SpawnInfo);
					//AAeroPlaneCollisionActor* tempPlaneCollision = GetWorld()->SpawnActor<AAeroPlaneCollisionActor>(myLoc, myRot, SpawnInfo);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-20, 3, FColor::Red, FString::Printf(TEXT("ERROR: Selected Andy is not valid!")));
				}
			}
		}

		bool ACustomARPawn::getFastWeapon()
		{
			return fastWeapon;
		}

		void ACustomARPawn::setFastWeapon(bool value)
		{
			fastWeapon = value;
			BPManager->bpChangeButtonColour();
		}

		void ACustomARPawn::HandleEnemies(float DeltaTime)
		{
			if (bIsPaused == true)
			{
				return;
			}

			if (enemySpawnTimer <= 0.0f && enemiesInScene.Num() < maxEnemies)
			{
				//Quickly check Array
				for (int i = 0; i < enemiesInScene.Num(); i++)
				{
					if (enemiesInScene[i]->IsValidLowLevelFast() == false)
					{
						enemiesInScene.RemoveAt(i);
					}
				}

				//spawn enemy
				//AAndyActor* temp = GetWorld()->SpawnActor<AAndyActor>(myLoc, myRot, SpawnInfo);
				if (selectedAndy->IsValidLowLevelFast() == true)
				{
					FRotator myRot(0.0f, 0.0f, 0.0f);
					//We need to move enemies away from Andy
					FVector myLoc = selectedAndy->GetActorLocation();
					myLoc.X += 125;
					FActorSpawnParameters SpawnInfo;
					AAREnemy* newEnemy = GetWorld()->SpawnActor<AAREnemy>(myLoc,myRot, SpawnInfo);
					enemySpawnTimer = 3.0f;
					enemiesInScene.Add(newEnemy);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-20, 3, FColor::Red, FString::Printf(TEXT("ERROR: Selected Andy is not valid!")));
				}
			}
			else
			{
				enemySpawnTimer -= DeltaTime;
			}
		}

		void ACustomARPawn::HandleAeroPlanes(float DeltaTime)
		{
			if (bIsPaused == true)
			{
				return;
			}

			//Quickly check Array
			for (int i = 0; i < AeroPlanesInScene.Num(); i++)
			{
				if (AeroPlanesInScene[i]->IsValidLowLevelFast() == false)
				{
					AeroPlanesInScene.RemoveAt(i);
				}
			}


			if (planeSpawnTimer <= 0.0f && AeroPlanesInScene.Num() < maxAeroPlanes)
			{
				FRotator myRot(0.0f, 0.0f, 0.0f);
				//We need to move the plane away from Andy into the sky
				FVector myLoc = selectedAndy->GetActorLocation();
				myLoc.Z += 125;
				FActorSpawnParameters SpawnInfo;
				AAeroPlaneActor* tempPlane = GetWorld()->SpawnActor<AAeroPlaneActor>(myLoc, myRot, SpawnInfo);
				AAeroPlaneCollisionActor* tempPlaneCollision = GetWorld()->SpawnActor<AAeroPlaneCollisionActor>(myLoc, myRot, SpawnInfo);
				planeSpawnTimer = 5.0f;
				AeroPlanesInScene.Add(tempPlane);
			}
			else
			{
				planeSpawnTimer -= DeltaTime;
			}
		}

		void ACustomARPawn::RemoveEnemyFromArray(AAREnemy* enemy)
		{
			for (int i = 0; i < enemiesInScene.Num(); i++)
			{
				if (enemiesInScene[i] == enemy)
				{
					enemiesInScene[i]->Destroy();
					enemiesInScene.RemoveAt(i);
					break;
				}
			}
		}
