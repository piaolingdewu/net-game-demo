// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Session/NetGameSession.h"


// Sets default values
ANetGameSession::ANetGameSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ANetGameSession::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetGameSession::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

