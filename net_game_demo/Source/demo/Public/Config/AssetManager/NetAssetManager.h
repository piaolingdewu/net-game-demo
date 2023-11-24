// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/AssetManager.h"
#include "NetAssetManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DEMO_API UNetAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:

	virtual void StartInitialLoading() override;

	UFUNCTION(BlueprintCallable)
	void LoadConfig();

	UFUNCTION(BlueprintPure)
	static UNetAssetManager* GetAssetManager();
public:
	static UNetAssetManager& Get();
};
