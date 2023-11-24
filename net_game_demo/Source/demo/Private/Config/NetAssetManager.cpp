// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/AssetManager/NetAssetManager.h"

#include "Config/GameLevelConfig.h"
#include "Engine/ObjectLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Type/NetGameTagsData.h"


void UNetAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 初始化Tag
	FNetGameTagsData::Get().InitGameplayTag();
	// 载入资产


	LoadConfig();
}

void UNetAssetManager::LoadConfig()
{
	FPrimaryAssetType type=FName("Config");

	auto ptr=this->LoadPrimaryAssetsWithType(type);
	if (ptr)
	{
		ptr->BindUpdateDelegate(FStreamableUpdateDelegate::CreateLambda([](TSharedRef<FStreamableHandle> handle)
		{
			UE_LOG(LogTemp,Warning,TEXT("LoadConfig Finish %s"),*handle->GetDebugName());
				
		}));

	}
	
	
}

UNetAssetManager* UNetAssetManager::GetAssetManager()
{
	return &Get();
}

UNetAssetManager& UNetAssetManager::Get()
{
	check(GEngine);

	if (UNetAssetManager* Singleton = Cast<UNetAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	// Fatal error above prevents this from being called.
	return *NewObject<UNetAssetManager>();
}
