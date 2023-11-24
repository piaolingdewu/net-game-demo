// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameType.generated.h"
/**
 * 
 */


// 玩家分数信息
USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	int score=0;
	UPROPERTY(BlueprintReadOnly)
	int rank=0;
	// 死亡次数
	UPROPERTY(BlueprintReadOnly)
	int deadCount=0;
	// 生成次数
	UPROPERTY(BlueprintReadOnly)
	int spawnCount=0;
	
};

// 服务器的内容
struct ServerAddr {
	FString ip="";
	FString port="";

	
};

struct ServerHeartbeat {
	// 最后一次心跳包的时间
	int64 last_hart_beat_time=0;
	// 最近十次的平均帧数
	int64 last_10_ticks_average_time=0.0;
	// 最后一次tick的时间 
	int64 last_tick_time=0;
	// 当前服务器的状态 只有Running和Stop两种
	FString status="Unknow";
	
};

struct FServerInfo {
public:
	ServerAddr server_addr;
	ServerHeartbeat server_heartbeat;

public:
	FString to_json() const;
};

