// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UDebugFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContext))
	static FString GetNetModeName(const UObject* WorldContext);
};
