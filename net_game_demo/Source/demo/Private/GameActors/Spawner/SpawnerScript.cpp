// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Spawner/SpawnerScript.h"

#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/GameplayEventManager.h"
#include "Type/NetGameTagsData.h"

void USpawnerScript::StartScript()
{
	Super::StartScript();
	// 监听事件
	
	UGameplayEventManager::GetCurrent(this->GetWorld())->OnGameplayEventReceive.AddDynamic(this,&USpawnerScript::EventHandel);
}

void USpawnerScript::EventHandel(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	//UKismetSystemLibrary::PrintString(this,FString::Format(TEXT("Trriger:{0}"),{EventTag.ToString()}));
	
	// 怪物死亡事件
	if (IsValid(Payload.Instigator))
	{
		if (EventTag.MatchesTag(FNetGameTagsData::MonsterDeadEventTag))
		{
				const AMonsterPawn* MonsterPawn = Cast<AMonsterPawn>(Payload.Instigator.Get());
				if (MonsterPawn)
				{
					k2_OnMonsterDead(MonsterPawn);
				}
			return ;
		}

		if (EventTag.MatchesTag(FNetGameTagsData::MonsterSpawnEventTag))
		{
			// 怪物放置标签
			const AMonsterPawn* MonsterPawn = Cast<AMonsterPawn>(Payload.Instigator.Get());
			if (MonsterPawn)
			{
				k2_OnMonsterSpawn(MonsterPawn);
			}
		}
		if (EventTag.MatchesTag(FNetGameTagsData::PlayerSpawnEventTag))
		{
			// 玩家被放置
			const APlayerPawn* PlayerPawn=Cast<APlayerPawn>(Payload.Instigator.Get());
			k2_OnPlayerSpawn(PlayerPawn);
		}

		if (EventTag.MatchesTag(FNetGameTagsData::PlayerDeadEventTag))
		{
			// 玩家死亡
			const APlayerPawn* PlayerPawn=Cast<APlayerPawn>(Payload.Instigator.Get());
			k2_OnPlayerLeave(PlayerPawn);
		}

		if (EventTag.MatchesTag(FNetGameTagsData::PlayerJoinEventTag))
		{
			// 玩家进入游戏
			// 玩家死亡
			const APlayerPawn* PlayerPawn=Cast<APlayerPawn>(Payload.Instigator.Get());
			k2_OnPlayerEnter(Cast<APlayerController>(PlayerPawn->GetController()));
		}
	}
	else
	{
		ensureMsgf(1,TEXT("USpawnerScript::EventHandel() Payload.Target is nullptr"));
	}
}
