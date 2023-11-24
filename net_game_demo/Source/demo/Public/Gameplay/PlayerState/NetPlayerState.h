// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Type/GameType.h"
#include "Type/LoginType.h"
#include "NetPlayerState.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceivedScore, const FPlayerScore&, PlayerScore,bool,bSuccess, const FString&, Msg);

UCLASS()
class DEMO_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANetPlayerState();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	// 同步分数
	UFUNCTION(Server,Reliable)
	void SyncScoreToServer(const FPlayerScore& SyncPlayerScore);

};
