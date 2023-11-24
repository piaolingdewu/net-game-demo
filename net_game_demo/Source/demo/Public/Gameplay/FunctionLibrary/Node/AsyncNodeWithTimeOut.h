// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncNodeWithTimeOut.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UAsyncNodeWithTimeOut : public UBlueprintAsyncActionBase,public FTickableGameObject
{
	GENERATED_BODY()
public:

	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;


	// 当超时
	virtual void OnTimeOut() {};
	
public:
	float request_timeout=2.0f;

	bool bIsTimeout=false;
};
