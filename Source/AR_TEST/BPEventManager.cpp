// Fill out your copyright notice in the Description page of Project Settings.
//This CPP should not be used unless in exceptional circumstances!
//The functions in the header are intened to be used in BP not C++
//The exception is audio

#include "BPEventManager.h"
#include "Components/AudioComponent.h"

// Sets default values
ABPEventManager::ABPEventManager()
{
 	//Since this script should not do much don't tick
	PrimaryActorTick.bCanEverTick = false;

	//Audio stuff
	static ConstructorHelpers::FObjectFinder<USoundCue> audioCue(TEXT("SoundCue'/Game/Audio/Sfx.Sfx'"));

	//Store a reference to the cue asset
	backgroundAudioCue = audioCue.Object;

	//Create an audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	//Don't play sound as soon as we start
	AudioComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ABPEventManager::BeginPlay()
{
	Super::BeginPlay();
	
	//Play the background sfx
	AudioComponent->Play();
}

void ABPEventManager::PostInitializeComponents()
{
	AActor::PostInitializeComponents();

	if (AudioComponent->IsValidLowLevel())
	{
		AudioComponent->SetSound(backgroundAudioCue);
	}
}

// Called every frame
void ABPEventManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

