// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <vector>
#include "GameFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UGameFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContextObject))
	static AActor* GetNearestActor(UObject* WorldContextObject, FVector Location, float Radius, TArray<TSubclassOf<AActor>> ActorsType);

	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContextObject))
	static void GetNearestActors(UObject* WorldContextObject, FVector Location, float Radius, TArray<AActor*>& OutActors, TArray<TSubclassOf<AActor>>
	                             AActorsType);

	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContextObject))
	static APlayerPawn* GetNearestPlayer(UObject* WorldContextObject, FVector Location, float Radius);

public:

	// 获取密度最小的位置
	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContext))
	static void GetMiniMonsterPos(UObject* WorldContext, const TArray<FVector>& PosList, float Radius, FVector& outLocation, TSubclassOf<AActor>
	                              MonsterClass, bool& bSuccess);

	// 获取值

	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContext))
	static void GetAllPlayerName(UObject* WorldContext, TArray<FString>& outPlayers);

};
