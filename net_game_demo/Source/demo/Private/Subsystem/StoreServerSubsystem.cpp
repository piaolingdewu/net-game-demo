// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/StoreServerSubsystem.h"

#include "HttpManager.h"
#include "HttpModule.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Kismet/GameplayStatics.h"

void UStoreServerSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void UStoreServerSubsystem::GetScoreFromController(APlayerPawnController* pc)
{
	if (pc)
	{
		// 尝试发送请求
		TSharedRef<IHttpRequest,ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	}
}

void UStoreServerSubsystem::AsyncScoreToServer(APlayerPawnController* PC, const FPlayerScore& PlayerScore)
{
	// 同步数据
	if (PC)
	{
		// @TODO:写入数据到计分服务器
		
	}
}

UStoreServerSubsystem* UStoreServerSubsystem::GetCurrent(UObject* WorldContext)
{
	return UGameplayStatics::GetGameInstance(WorldContext)->GetSubsystem<UStoreServerSubsystem>();
}




