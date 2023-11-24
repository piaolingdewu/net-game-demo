// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Spawner/SpawnPolicyBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "GameActors/Controller/PlayerPawnController.h"
#include "GameActors/Spawner/MonsterSpawner.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"



void USpawnPolicyBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	this->Spawner=Cast<AMonsterSpawner>(ActorInfo->AvatarActor);
}

int32 USpawnPolicyBase::GetPlayerCount()
{
	if (UKismetSystemLibrary::IsServer(this))
	{
		// 运行在服务器
		return this->GetWorld()->GetAuthGameMode()->GetNumPlayers();
	}
	// 必须运行
	UE_LOG(LogTemp,Display,TEXT("USpawnPolicyBase::GetPlayerCount() is not runing server"));
	return 0;
}

FVector USpawnPolicyBase::GetRandomSpawnPosition()
{
	if (this->Spawner)
	{
		return this->Spawner->GetRandomPoint();
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("USpawnPolicyBase::GetRandomSpawnPositon() Spawner is nullptr"));
	}
	return FVector::Zero();
}



bool USpawnPolicyBase::IsTickable() const
{
	return !IsTemplate();
}

void USpawnPolicyBase::Tick(float DeltaTime)
{
	MyTick(DeltaTime);
}

TStatId USpawnPolicyBase::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USpawnPolicyBase, STATGROUP_Tickables);
}
