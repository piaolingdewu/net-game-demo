// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AccountRegisterNode.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UAccountRegisterNode : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static UAccountRegisterNode* TryRegisterServer(UObject* WorldContext, const FString& in_account, const FString& passwd, ALogin_Controller* pc, float timeout);

	UFUNCTION()
	void OnCallbackRegisterResult(const ERegisterResultType& InLoginResult, const FString& Msg);
	
	void start();
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAccountRegister,bool,result,FString,Msg);

	UPROPERTY(BlueprintAssignable)
	FOnRegisterResult OnAccountRegister;

	FString account;
	FString password;
	UPROPERTY()
	TObjectPtr<ALogin_Controller> controller;
public: // 当超时的时候
	virtual void OnTimeOut() override;
public:
	
};
