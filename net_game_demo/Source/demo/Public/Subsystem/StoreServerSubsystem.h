// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "GameActors/Controller/PlayerPawnController.h"
#include "UObject/Object.h"
#include "StoreServerSubsystem.generated.h"

/**
 * 这类是在服务器上运行的。
 */
UCLASS()
class DEMO_API UStoreServerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UFUNCTION()
	void GetScoreFromController(APlayerPawnController* pc);
	
	// 同步客户端的数据的值
	void AsyncScoreToServer(APlayerPawnController* PC, const FPlayerScore& PlayerScore);
	
	static UStoreServerSubsystem* GetCurrent(UObject* WorldContext);
	
public:
	// 新建一个Get请求
	// 获取请求
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreRecive,)
};
