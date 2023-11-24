// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GameState/NetGameState.h"


// Sets default values
ANetGameState::ANetGameState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANetGameState::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

