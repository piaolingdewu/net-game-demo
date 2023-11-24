// Fill out your copyright notice in the Description page of Project Settings.


#include "Type/NetGameTagsData.h"
#include "GameplayTagsManager.h"


FNetGameTagsData *NetGameTags= nullptr;
FGameplayTag FNetGameTagsData::MonsterDeadEventTag;
FGameplayTag FNetGameTagsData::PlayerJoinEventTag;
FGameplayTag FNetGameTagsData::PlayerDeadEventTag;
FGameplayTag FNetGameTagsData::PlayerSpawnEventTag;
FGameplayTag FNetGameTagsData::MonsterSpawnEventTag;
FGameplayTag FNetGameTagsData::MonsterDamageEventTag;


FNetGameTagsData::FNetGameTagsData()
{

}


FGameplayTag FNetGameTagsData::AddTag(const FString& TagName, const FString& descript)
{
	return UGameplayTagsManager::Get().AddNativeGameplayTag(*TagName,descript);
}

void FNetGameTagsData::InitGameplayTag()
{
	if (!bInit)
	{
		MonsterDeadEventTag=AddTag(TEXT("Monster.Dead"),TEXT("怪物死亡"));

		PlayerDeadEventTag=AddTag(TEXT("Player.Dead"),TEXT("玩家死亡"));

		PlayerSpawnEventTag=AddTag(TEXT("Player.Enter"),TEXT("玩家进入"));

		MonsterSpawnEventTag=AddTag(TEXT("Monster.Spawn"),TEXT("怪物降生"));
		
		MonsterDamageEventTag=AddTag(TEXT("Monster.Damage"),TEXT("怪物受到伤害"));
		
		bInit=true;
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("SpawnerTags::InitGameplayTag() has been called"));
	}
}

FNetGameTagsData& FNetGameTagsData::Get()
{
	if (NetGameTags==nullptr)
		NetGameTags=new FNetGameTagsData();
	return *NetGameTags;
}
