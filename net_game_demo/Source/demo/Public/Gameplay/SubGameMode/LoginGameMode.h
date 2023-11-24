// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/NetGameMode.h"
#include "Interfaces/IHttpRequest.h"
#include "Type/GameType.h"
#include "LoginGameMode.generated.h"

UCLASS(BlueprintType,Blueprintable)
class DEMO_API ALoginGameMode : public ANetGameMode
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALoginGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

