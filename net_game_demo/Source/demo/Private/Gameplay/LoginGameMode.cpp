// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/SubGameMode/LoginGameMode.h"

#include "Gameplay/PlayerState/NetPlayerState.h"
// Sets default values
ALoginGameMode::ALoginGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->PlayerStateClass=ANetPlayerState::StaticClass();
}

// Called when the game starts or when spawned
void ALoginGameMode::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALoginGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

