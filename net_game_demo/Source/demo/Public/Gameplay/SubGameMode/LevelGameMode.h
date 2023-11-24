// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/NetGameMode.h"
#include "LevelGameMode.generated.h"

UCLASS(BlueprintType,Blueprintable)
class DEMO_API ALevelGameMode : public ANetGameMode
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;


private:
	TMap<FUniqueNetIdRepl,TTuple<FString,FString>> accountPair;
	
};
