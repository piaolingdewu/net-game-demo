// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	EBrowseReturnVal::Type TryConnectServer(const FString& Host, const FString& Port, const FString& account,
	                                        const FString& password);

	
	
};
