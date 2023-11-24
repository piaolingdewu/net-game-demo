// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "GameplayEventManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayEventReceive, const FGameplayTag&, EventTag, const FGameplayEventData&, Payload);

UCLASS()
class DEMO_API UGameplayEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static void SendGameplayEvent(UObject* WorldContext, FGameplayTag EventTag, const FGameplayEventData& Payload);

	
public:
	UPROPERTY(BlueprintAssignable)
	FGameplayEventReceive OnGameplayEventReceive;
public:
	static UGameplayEventManager* GetCurrent(UObject* WorldContext);
};
