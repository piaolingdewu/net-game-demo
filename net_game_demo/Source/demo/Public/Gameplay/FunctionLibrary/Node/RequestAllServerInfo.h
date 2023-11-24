// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Type/RequestType.h"
#include "RequestAllServerInfo.generated.h"

/**
 * 
 */

UCLASS()
class DEMO_API URequestAllServerInfo : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext,BlueprintInternalUseOnly=true))
	static URequestAllServerInfo* RequestAllServerInfo(UObject* WorldContext, ALogin_Controller* login_controller);


public:

	UFUNCTION()
	void RequestComplete(const TArray<FServerData>& data);

public:

	UPROPERTY(BlueprintAssignable)
	FOnAllServerDataRecive Complete;

	void start();
	
	virtual void OnTimeOut() override;

private:
	TObjectPtr<ALogin_Controller> login_controller;
};
