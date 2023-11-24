// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/GameLevelConfig.h"

#include <fstream>

#include "Engine/AssetManager.h"

FPrimaryAssetId UGameLevelConfig::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("LevelConfig");
}

FPrimaryAssetType UGameLevelConfig::GetPrimaryType()
{
	return FName("Config");
}

UGameLevelConfig* UGameLevelConfig::GetGameLevelConfig()
{
	return LoadObject<UGameLevelConfig>(nullptr, TEXT("/Game/Config/LevelConfig_Config"));;
}

UGameLevelConfig* UGameLevelConfig::LoadFromYamlConfig(FString file_path)
{
	ensureMsgf(1,TEXT("TODO: 这里应该从指定的文件导入。"));
	return nullptr;
}

