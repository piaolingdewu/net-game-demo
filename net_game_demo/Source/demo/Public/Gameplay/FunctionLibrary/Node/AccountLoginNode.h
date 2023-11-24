// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Interfaces/IHttpRequest.h"
#include "AccountLoginNode.generated.h"

/**
 * 
 */
// 增加一个超时时间

UCLASS()
class DEMO_API UAccountLoginNode : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext,BlueprintInternalUseOnly="true"))
	static UAccountLoginNode* TryLoginServer(UObject* WorldContext, const FString& user_name, const FString& passwd, ALogin_Controller* playerController, float
	                                         Timeout = 2.0f);


public:

	UFUNCTION()
	void OnLoginResult(ELoginResultType LoginResult,FString Msg);

	void start();

	UPROPERTY(BlueprintAssignable)
	FOnLoginResult OnComplete;
	virtual void OnTimeOut() override;

public:
	FString Password;
	FString Account;
	UPROPERTY()
	TObjectPtr<ALogin_Controller> controlelr;
};
