// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Type/GameType.h"
#include "UObject/Object.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Containers/Deque.h"
#include "ServerStatusSubsystem.generated.h"

/**
 * 用于向服务器发送心跳包，并且记录玩家的分数
 */
UCLASS()
class DEMO_API UServerStatusSubsystem : public UTickableWorldSubsystem
{
public:
	GENERATED_BODY()
public: 
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
public:
	
	// 将该地图添加到 专用服务器列表中
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	void AddLevelToServer(UObject* WorldContext);
	
public:
	UFUNCTION()
	void OnGameplayEventReceive(const FGameplayTag& EventTag, const FGameplayEventData& Payload);
	void BindWorldEvent();

public: // 服务器
	
	void StartServer();
	
	void StopServer();
	
	// 发送心跳包
	void SendHeartbeat(FString Status);
	
	// 更新人数
	void SendPlayers();

	//
	void SendPlayersWithList(const TArray<FString>& InPlayers);

	// 判断是否是游戏地图
	UFUNCTION(BlueprintPure)
	bool IsGameMap();

public: // 获取服务器的名称
	UFUNCTION(BlueprintPure)
	void GetServerName(FString& ServerName);
	UFUNCTION(BlueprintPure)
	void GetProcessID(FString& ProcessID);


public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	
	static UServerStatusSubsystem* GetCurrent(UObject* WorldContext);

public: // 玩家信息
	// 当前关卡的玩家信息 只会记录玩家当前的游戏信息
	UPROPERTY()
	TMap<APlayerController*,FPlayerScore> LevelPlayersScoreList;

public: // 服务器的更新
	
	// 心跳包
	bool bEnableHeartbeat=false;
	// 当前的服务器信息
	
	FServerInfo ServerInfo;
	// 半秒发一次的心跳包
	float heartbeat_rate=0.5;
	
private:
	// 最后十帧的tick长度
	TDeque<float> last_10_tick_queue;
	
	// 心跳包的计时器
	float tick_sum;

	int64 heartbeat_count=0;
	
};

