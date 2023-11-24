// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
class DEMO_API FNetGameTagsData
{
public:
	static FGameplayTag MonsterDamageEventTag;
	FNetGameTagsData();

public:
	FGameplayTag AddTag(const FString& TagName, const FString& descript);

	void InitGameplayTag();

public:
	// 获取唯一的实例
	static FNetGameTagsData& Get();
public:
	// 怪物死亡标签
	static  FGameplayTag MonsterDeadEventTag;

	// 玩家加入标签
	static  FGameplayTag PlayerJoinEventTag;

	// 玩家死亡标签
	static  FGameplayTag PlayerDeadEventTag;

	// 玩家进入标签
	static  FGameplayTag PlayerSpawnEventTag;

	// 怪物降生标签
	static FGameplayTag MonsterSpawnEventTag;
	
private:
	bool bInit=false;
};
