// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Login_Controller.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "GameFramework/PlayerController.h"
#include "Type/GameType.h"
#include "PlayerPawnController.generated.h"

UCLASS()
class DEMO_API APlayerPawnController : public ALogin_Controller
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	APlayerPawnController();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	virtual void SetupInputComponent() override;
public: // 鼠标可视化投射
	AActor* GetMouseActor();
	UFUNCTION(BlueprintCallable)
	void ProjectiveMouse();
	

public: // 初始化PlayerStatus
	virtual void InitPlayerState() override;

	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnPlayerStateInit")
	void k2_OnPlayerStateInit();

	virtual void OnRep_PlayerState() override;
	
public:
	// 当同步到了pawn之后
	UFUNCTION(BlueprintImplementableEvent)
	void OnClientPossessPawn(const APawn* InPawn);
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void GetScoreFromServer();
	
	// 玩家网络分数更新
	UFUNCTION()
	void OnRep_PlayerScore_Init();

	// 同步数据到服务器
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void AsyncScoreToServer(const FPlayerScore& NewScore);
public:
	virtual void OnRep_Pawn() override;

public:
	// 生成一个玩家
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void SpawnPlayer();
	
public: // 增加玩家分数 和等级

	// 增加分数
	void AddScore(const int Offset);
	// 增加等级
	void AddRank(const int Offset);
	// 更新分数
	UFUNCTION()
	void OnRep_Remote_PlayerScore();
	// 更新分数
	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnUpdatePlayerScore")
	void OnUpdatePlayerScore(const FPlayerScore& NewScore);


	// 获取远程的分数
	UFUNCTION(BlueprintPure)
	void GetRemoteScore(FPlayerScore& RemoteScore);
public: // 发送分数
	UFUNCTION(Server,Reliable,BlueprintCallable)
	void SyncScore(const FPlayerScore& PlayerScore, const FString& in_account);


public:
	UPROPERTY(Replicated,ReplicatedUsing=OnRep_PlayerScore_Init)
	FPlayerScore PlayerScore_Init;
	
	
	UPROPERTY(BlueprintReadWrite)
	FPlayerScore PlayerScore_Local;
	UPROPERTY(Replicated,ReplicatedUsing=OnRep_Remote_PlayerScore)
	FPlayerScore Remote_PlayerScore;
	// 鼠标投射的类
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AActor> ProjectiveToWorldClass;
	UPROPERTY()
	TObjectPtr<AActor> MouseActor;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlayerPawn> PlayerClass;
	
	// 是否投射鼠标
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bEnableProjectiveMouse=true;
private:
	// 是否从服务器上初始化过了
	bool bInitFromServer=false;
};
