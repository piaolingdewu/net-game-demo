// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MonsterSpawner.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnerHelper.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API USpawnerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

public:
	UFUNCTION(BlueprintPure)
	static AMonsterSpawner* GetSpawner();

	// 放置怪物
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static AMonsterPawn* SpawnMonster(UObject* WorldContext,FVector Location,TSubclassOf<AMonsterPawn> Class);

	// 获取现在的所有的怪物
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static void GetCurrentMonsterNum(UObject* WorldContext,int &num);

	// 获取场景内的一个随机坐标
	UFUNCTION(BlueprintPure)
	static FVector GetRandomSpawnPosition();

	
};
