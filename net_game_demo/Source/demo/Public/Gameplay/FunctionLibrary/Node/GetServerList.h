// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "Type/LoginType.h"

#include "GetServerList.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UGetServerList : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static UGetServerList* GetServerList(UObject* WorldContext,ALogin_Controller* LoginController);

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnServerListReceive,const TArray<FGameMapConnectInfo>&, ServerList,const FString&, Msg,bool,bSuccess);

	UFUNCTION()
	void OnReceiveServerList(const TArray<FGameMapConnectInfo>& ServerList, const FString& Msg);
	
	void start();

	UPROPERTY(BlueprintAssignable)
	FOnServerListReceive OnServerListReceive;
	
public:
	virtual void OnTimeOut() override;
public:
	
	ALogin_Controller* controller;
};
