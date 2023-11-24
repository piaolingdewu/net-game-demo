// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/NetGameMode.h"

#include "GameActors/Controller/Login_Controller.h"
#include "GameFramework/GameSession.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/OnlineEngineInterface.h"
#include "Subsystem/ServerStatusSubsystem.h"


// Sets default values
ANetGameMode::ANetGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
