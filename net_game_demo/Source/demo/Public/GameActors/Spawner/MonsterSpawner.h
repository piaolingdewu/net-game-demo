// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameActors/Pawn/GamePawn/MonsterPawn.h"
#include "MonsterSpawner.generated.h"
class USpawnerScript;

/*
 * 只存在于服务器上 放置Monster之后就会自己进行复制到客户端
 */
UCLASS(BlueprintType,Blueprintable)
class DEMO_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMonsterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:

public:
	UFUNCTION(BlueprintPure)
	FVector GetRandomPoint();

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void StartupSpawn();
	
	static AMonsterSpawner* Get();

public: // 成员变量

	UPROPERTY(BlueprintReadOnly)
	UBoxComponent* SpawnBoxComponent;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<AMonsterPawn> MonsterClass;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	TSubclassOf<USpawnerScript> SpawnerScriptClass;

	UPROPERTY()
	TObjectPtr<USpawnerScript> ScriptInstance;
	
	
};
