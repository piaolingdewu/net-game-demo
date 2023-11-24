// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpResponse.h"
#include "Type/LoginType.h"
#include "GetFreeServerNode.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UGetFreeServerNode : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:
	// 获取一个空闲的服务器
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static UGetFreeServerNode* GetFreeServer(UObject* WorldContext, FString in_account, ALogin_Controller* controller);

public:

public:
	UFUNCTION()
	void OnCallbackGetFreeServer(const FGameMapConnectInfo& ServerInfo, const bool bSuccess, const FString& Msg);
	void start();

	virtual void OnTimeOut() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnReceivedFreeServer OnReceiveFreeServer;
	ALogin_Controller* controller;
	FString account;
};
