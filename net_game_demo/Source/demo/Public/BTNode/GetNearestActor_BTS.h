// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "GetNearestActor_BTS.generated.h"


/**
 * 
 */
UCLASS()
class DEMO_API UGetNearestActor_BTS : public UObject
{
	GENERATED_BODY()
public:
	
	
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FBlackboardKeySelector BlackboardKey;
	
};
