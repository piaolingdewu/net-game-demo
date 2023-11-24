// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "GameActors/Controller/Login_Controller.h"
#include "CreateRoomNode.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UCreateRoomNode : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject,BlueprintInternalUseOnly="true"))
	static UCreateRoomNode* CreateRoom(UObject* WorldContextObject, ALogin_Controller* Login_Controller, float TimeOut = 10.f);

	UFUNCTION()
	void OnReceiveRoomServer(FString IP, FString Port);
	UFUNCTION()
	void ReceiveRoomName(FString RoomId);
	
	void start();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateFinish,FString,IP,FString,Port);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeout);
	
	UPROPERTY(BlueprintAssignable)
	FOnCreateFinish OnCreateFinish;
	UPROPERTY(BlueprintAssignable)
	FOnTimeout OnTimeout;
	bool bOnce=false;

	virtual void OnTimeOut() override;

	UPROPERTY()
	TObjectPtr<ALogin_Controller> controller;

	FTimerDelegate PollServer;
	
	FTimerHandle PollServerHandle;
	
private:
	UPROPERTY()
	FString Account;
	UPROPERTY()
	FString RoomName;
	
	
};
