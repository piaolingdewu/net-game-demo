// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Spawner/MonsterSpawner.h"
#include "GameActors/Pawn/GamePawn/MonsterPawn.h"
#include "GameActors/Spawner/SpawnerScript.h"
#include "Subsystem/GameplayEventManager.h"
#include "Type/NetGameTagsData.h"


TObjectPtr<AMonsterSpawner> GMonsterSpawner;

// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBoxComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnnerComponent"));

	this->bReplicates=false;
	RootComponent=SpawnBoxComponent;
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
	GMonsterSpawner=this;
	// 放置
}

void AMonsterSpawner::Destroyed()
{
	Super::Destroyed();
	GMonsterSpawner=nullptr;
}


// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



FVector AMonsterSpawner::GetRandomPoint()
{
	FVector extent=this->SpawnBoxComponent->GetScaledBoxExtent();

	return this->GetActorLocation()+FMath::RandPointInBox(FBox(-extent,extent));
}

void AMonsterSpawner::StartupSpawn_Implementation()
{
	if (SpawnerScriptClass)
	{
		auto obj=UGDT_LightObjScript::CreateGDT_ObjScript<USpawnerScript>(this,SpawnerScriptClass);
		
		ScriptInstance=obj;
	}

}

AMonsterSpawner* AMonsterSpawner::Get()
{
	return GMonsterSpawner;
}

