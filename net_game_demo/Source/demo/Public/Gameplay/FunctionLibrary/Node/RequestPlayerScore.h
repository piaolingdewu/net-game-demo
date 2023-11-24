// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Gameplay/PlayerState/NetPlayerState.h"
#include "Type/GameType.h"
#include "RequestPlayerScore.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API URequestPlayerScore : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:
	// 请求分数
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext,BlueprintInternalUseOnly="true"))
	static URequestPlayerScore* RequestPlayerScore(UObject* WorldContext, ALogin_Controller* PlayerController,const FString& account);

public:
	UFUNCTION()
	void OnCallbackRequestPlayerScore(const FPlayerScore& PlayerScore, bool bSuccess, const FString& Msg);
	void start();
	
	UPROPERTY(BlueprintAssignable)
	FOnReceivedScore OnReceivedScore;

	UPROPERTY()
	ALogin_Controller* playerController;

public:
	virtual void OnTimeOut() override;

	FString Account;
};
