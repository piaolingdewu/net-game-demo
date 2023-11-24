// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "GameActors/Spawner/SpawnerHelper.h"
#include "Subsystem/GameplayEventManager.h"
#include "Type/NetGameTagsData.h"
// Sets default values

APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//this->SetReplicatedMovement(FRepMovement::)

	
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	// 通知世界
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator=this;
	UGameplayEventManager::SendGameplayEvent(this,FNetGameTagsData::PlayerSpawnEventTag,GameplayEventData);
	this->last_pos=this->GetActorLocation();
	this->new_location=this->GetActorLocation();
	
}

void APlayerPawn::SetDie()
{
	FGameplayEventData GameplayEventData;
	GameplayEventData.Instigator=this;
	UGameplayEventManager::SendGameplayEvent(this,FNetGameTagsData::PlayerDeadEventTag,GameplayEventData);
	this->k2_SetDie();
}



void APlayerPawn::SetReadyToDestroy_Implementation()
{
	
	// 销毁
	
	this->Destroy();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!this->IsNetMode(NM_Standalone))
	{
		last_pos=this->GetActorLocation();
		// 判断是否要修正坐标   
		if (
			this->GetController()&&
			this->GetController()->IsLocalPlayerController()&&
			FVector::Dist(last_pos,new_location)>1)
		{
			SendCurrentPos(this->GetActorLocation());
		}
		else
		{
			UpdateMove();
		}
	}

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void APlayerPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
#ifdef WITH_SERVER_CODE
	// 发生碰撞
	if (OtherActor->IsA(AMonsterPawn::StaticClass()))
	{
		this->k2_OnMonsterOverlap(Cast<AMonsterPawn>(OtherActor));
	}
#endif
}





