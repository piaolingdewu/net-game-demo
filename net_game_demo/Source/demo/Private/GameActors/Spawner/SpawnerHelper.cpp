// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Spawner/SpawnerHelper.h"
#include <ranges>
#include "EngineUtils.h"
#include "GameplayAbilityBlueprint.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameActors/Controller/MonsterController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/GameplayEventManager.h"


AMonsterSpawner* USpawnerHelper::GetSpawner()
{
	return AMonsterSpawner::Get();
}

AMonsterPawn* USpawnerHelper::SpawnMonster(UObject* WorldContext,FVector Location, TSubclassOf<AMonsterPawn> Class)
{
	if (auto world=WorldContext->GetWorld())
	{
		if (Class)
		{
			// 放置控制器
			auto aiController=world->SpawnActor<AMonsterController>();
			FActorSpawnParameters SpawnParameters;
			auto monster=world->SpawnActor<AMonsterPawn>(Class,Location,{},SpawnParameters);
			aiController->Possess(monster);
			return monster;
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("USpawnerHelper::SpawnMonster Class is Empty"));
		}
	}
	return nullptr;
}

void USpawnerHelper::GetCurrentMonsterNum(UObject* WorldContext, int& num)
{
	for (TActorIterator<AMonsterPawn> Iterator(WorldContext->GetWorld());Iterator;++Iterator)
		++num;
}

FVector USpawnerHelper::GetRandomSpawnPosition()
{
	if (GetSpawner())
		return GetSpawner()->GetRandomPoint();
	return FVector::Zero();
}
