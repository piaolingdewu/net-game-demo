// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameLevelConfig.generated.h"



/**
 * 玩家在进入游戏的时候会进入两套场景
 */
UCLASS(Blueprintable)
class DEMO_API UGameLevelConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSoftObjectPtr<UWorld> LoginMap;
	
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSoftObjectPtr<UWorld> PlayMap;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetPrimaryType();

public:
	// 读取指定的配置文件

	// 最大同时玩家人数
	size_t MaxPlayer=3;
	// 心跳包之间的间隔
	float HeartBeatInterval=0.5f;

public:
	// 获取载入地图
	static UGameLevelConfig* GetGameLevelConfig();
	// 从配置文件中载入
	static UGameLevelConfig* LoadFromYamlConfig(FString file_path);
};
