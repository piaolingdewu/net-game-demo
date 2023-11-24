// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MonsterSpawner.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameActors/Pawn/GamePawn/MonsterPawn.h"
#include "GDT_LittleObjScript/GDT_LightObjScript.h"
#include "UObject/Object.h"
#include "SpawnerScript.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DEMO_API USpawnerScript : public UGDT_LightObjScript
{
	GENERATED_BODY()
public:
	virtual void StartScript() override;
public: // 游戏事件

	// 事件订阅器
	UFUNCTION()
	void EventHandel(const FGameplayTag& EventTag, const FGameplayEventData& Payload);
 
	// 当玩家进入游戏
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnPlayerEnter")
	void k2_OnPlayerEnter(APlayerController* Controller);
	
	// 当玩家被放置
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnPlayerSpawn")
	void k2_OnPlayerSpawn(const APlayerPawn* PlayerPawn);

	// 当玩家离开游戏
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnPlayerLeave")
	void k2_OnPlayerLeave(const APlayerPawn* PlayerPawn);

	// 当怪物生成出来
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMonsterSpawn")
	void k2_OnMonsterSpawn(const AMonsterPawn* MonsterPawn);

	// 当怪物死亡
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMonsterDead")
	void k2_OnMonsterDead(const AMonsterPawn* MonsterPawn);

public:
};
