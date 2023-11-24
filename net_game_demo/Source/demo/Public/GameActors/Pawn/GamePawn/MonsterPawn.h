// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameActors/Actor/Bullet.h"
#include "GameActors/Pawn/NetBasePawn.h"
#include "GameFramework/Pawn.h"
#include "MonsterPawn.generated.h"

/*
 * 随机被放置在地图上 
 */
UCLASS()
class DEMO_API AMonsterPawn : public ANetBasePawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMonsterPawn();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public: // 怪物事件
	
	// 执行行为树
	void InitAndBehaviorTree();
	
	// 添加子弹伤害
	void AddBulletDamage(ABullet* Bullet);
	
	virtual void PossessedBy(AController* NewController) override;

	// 死亡
	UFUNCTION(NetMulticast,Reliable)
	void SetDie();

public:
	// 当怪物死亡时
	UFUNCTION(Client,Reliable)
	void OnCallClientMonsterDie();
	UFUNCTION(BlueprintImplementableEvent)
	void OnMonsterDie();
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;
	
	UPROPERTY(EditDefaultsOnly)
	bool bDestroyOnHit=true;
	
	bool bHasRunBehavior=false;

};
