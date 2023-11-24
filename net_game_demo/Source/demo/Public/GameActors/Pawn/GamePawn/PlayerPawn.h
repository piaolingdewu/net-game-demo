// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameActors/Pawn/NetBasePawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"
class AMonsterPawn;

UCLASS(Config=Game)
class DEMO_API APlayerPawn : public ANetBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void SetDie();

	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnDie")
	void k2_SetDie();

public:

	// 通知服务器 销毁自己
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SetReadyToDestroy();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
public: // 当玩家发生了碰撞
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMonsterOverlap")
	void k2_OnMonsterOverlap(AMonsterPawn* MonsterPawn);
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	// 上一帧的坐标
	FVector last_pos;
};
