// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActors/Controller/Login_Controller.h"
#include "GameFramework/GameModeBase.h"
#include "NetGameMode.generated.h"

UCLASS()
class DEMO_API ANetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

private:
	
	// id:{account&password,is_valid,AccountController}
	
};

