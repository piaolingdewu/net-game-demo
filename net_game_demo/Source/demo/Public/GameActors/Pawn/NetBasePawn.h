// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "NetBasePawn.generated.h"

UCLASS()
class DEMO_API ANetBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ANetBasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public: // 网络连接移动
	// 向服务器发送移动的算法

	UFUNCTION(Server,Unreliable)
	void SendCurrentPos(const FVector &pos);

protected:
	// 移动修正
	void UpdateMove();

public:
	virtual void PossessedBy(AController* NewController) override;
public:
	// 默认当距离上次的移动的距离超过1之后 就通知执行移动
	UPROPERTY(EditDefaultsOnly)
	float move_replication_threshold=1;

	UPROPERTY(Replicated,BlueprintReadOnly)
	FVector new_location;

	// 移动修正的插值速度
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float lerp_speed=0.5;
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	UFloatingPawnMovement* FloatingPawnMovement;
	
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY()
	bool bPossessed=false;
	
};
