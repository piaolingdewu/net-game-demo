// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Gameplay/PlayerState/NetPlayerState.h"
#include "GDT_FunctionLibrary/GDT_FunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "UObject/Object.h"
#include "Type/GameType.h"
#include "AccountSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UAccountSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public: // 账号相关
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> try_login(const FString& user_name, const FString& passwd, float timeout);
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> try_register(const FString& user_name, const FString& passwd, float timeout);
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> try_validAccount(const FString& user_name, const FString& passwd, float timeout);



	static UAccountSubsystem* GetCurrent(UObject* WorldContext);

public:
	
};
