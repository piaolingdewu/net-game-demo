// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RequestType.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FServerHeartBeat
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	int32 last_10_ticks_average_time=0;

	UPROPERTY(BlueprintReadWrite)
	int32 last_hart_beat_time=0;

	UPROPERTY(BlueprintReadWrite)
	int32 last_tick_time=0;

	UPROPERTY(BlueprintReadWrite)
	FString status="";
};
USTRUCT(BlueprintType)
struct FServerRuntimeData
{
	GENERATED_BODY()

	public:

	UPROPERTY(BlueprintReadWrite)
	int64 create_time=0;

	UPROPERTY(BlueprintReadWrite)
	int32 heart_beat_speed=0;

	UPROPERTY(BlueprintReadWrite)
	FServerHeartBeat heartbeat;

	UPROPERTY(BlueprintReadWrite)
	int32 max_player=0;

	UPROPERTY(BlueprintReadWrite)
	int32 player_count=0;

	UPROPERTY(BlueprintReadWrite)
	TArray<FString> player_list={};

	UPROPERTY(BlueprintReadWrite)
	int32 round=0;
};

USTRUCT(BlueprintType)
struct FServerData
{
	GENERATED_BODY()

	public:
	UPROPERTY(BlueprintReadWrite)
	FString ip="127.0.0.1";

	UPROPERTY(BlueprintReadWrite)
	int32 port=0;

	UPROPERTY(BlueprintReadWrite)
	FServerRuntimeData runtime_data;
};

