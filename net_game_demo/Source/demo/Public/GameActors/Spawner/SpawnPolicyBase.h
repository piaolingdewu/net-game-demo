// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SpawnPolicyBase.generated.h"

class AMonsterSpawner;

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class DEMO_API USpawnPolicyBase : public UGameplayAbility,public FTickableGameObject
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEventReceive, FGameplayTag, EventTag, const FGameplayEventData&, Payload);

public: // 玩法操作
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// 获取玩家数量
	UFUNCTION(BlueprintCallable, Category = "SpawnPolicyBase")
	int32 GetPlayerCount();

public:
	// 获取一个随机的放置地址
	UFUNCTION(BlueprintPure, Category = "SpawnPolicyBase")
	FVector GetRandomSpawnPosition();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void MyTick(float DeltaTime);
	// 激活
	
public:
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AMonsterSpawner> Spawner;
	
};
