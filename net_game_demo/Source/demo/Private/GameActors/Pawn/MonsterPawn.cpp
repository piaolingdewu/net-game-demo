// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Pawn/GamePawn/MonsterPawn.h"

#include "GameActors/Actor/Bullet.h"
#include "GameActors/Controller/MonsterController.h"
#include "GameActors/Controller/PlayerPawnController.h"
#include "GameActors/Spawner/SpawnerHelper.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/GameplayEventManager.h"
#include "Type/NetGameTagsData.h"


// Sets default values
AMonsterPawn::AMonsterPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//this->AIControllerClass=AMonsterController::StaticClass();
	this->bReplicates=true;
}

// Called when the game starts or when spawned
void AMonsterPawn::BeginPlay()
{
	Super::BeginPlay();
	// 放置到世界之后添加控制器
	// 执行行为
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator=this;
	UGameplayEventManager::SendGameplayEvent(this,FNetGameTagsData::MonsterSpawnEventTag,GameplayEventData);
}


// Called every frame
void AMonsterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 在服务器上结算移动
	
}

// Called to bind functionality to input
void AMonsterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AMonsterPawn::InitAndBehaviorTree()
{
	//if (!Cast<AAIController>(Controller.Get()))
	//{
	//	// 生成控制器
	//	Controller=GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	//	Controller->Possess(this);
	//}
	if (auto apc=Cast<AAIController>(Controller.Get()))
	{
		if (BehaviorTree)
		{
			apc->RunBehaviorTree(BehaviorTree);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Monster Default Behavior Tree Not Set"));
		}
	}
}

void AMonsterPawn::AddBulletDamage(ABullet* Bullet)
{
	// 目前直接销毁
	if (this->bDestroyOnHit)
	{
		if (Bullet->BulletOwner&&Bullet->BulletOwner->GetController())
		{
			if (auto playerctl=Cast<APlayerPawnController>(Bullet->BulletOwner->GetController()))
			{
				//UScoreSubsystem::GetCurrent(this)->AddScoreToController(playerctl);
				// 请求加分

				playerctl->AddScore(1);
			}
			// 判断服务器上的Monster是否被销毁
			
		}
		this->SetDie();
	}
}

void AMonsterPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAndBehaviorTree();
}

void AMonsterPawn::OnCallClientMonsterDie_Implementation()
{
	this->OnMonsterDie();
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator=this;
	UGameplayEventManager::SendGameplayEvent(this,FNetGameTagsData::MonsterDeadEventTag,GameplayEventData);
}

void AMonsterPawn::SetDie_Implementation()
{
	this->OnCallClientMonsterDie();
	
	this->Destroy();
}

