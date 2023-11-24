// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bullet.generated.h"

UCLASS()
class DEMO_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public: // 事件
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	UFUNCTION(BlueprintCallable)
	static ABullet* SpawnBullet(APlayerPawn* InOwner, TSubclassOf<ABullet> BulletClass, FVector Location, FRotator Rotation);


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	// 攻击怪物
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USphereComponent* BulletBoundComponent;

	// 抛射体组件
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;

	// 设置子弹的Owner
	UPROPERTY(BlueprintReadWrite,Replicated)
	APlayerPawn* BulletOwner;

	UPROPERTY(EditDefaultsOnly)
	bool bDestroyOnHit=true;
};
