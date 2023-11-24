// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Subsystem/AccountSubsystem.h"
#include "Type/GameType.h"
#include "Type/LoginType.h"
#include "Type/RequestType.h"
#include "Login_Controller.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, ELoginResultType, Result, FString, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegisterResult, const ERegisterResultType&, RegisterResult, const FString&, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceivedServerList, const TArray<FGameMapConnectInfo>&, ServerList, const FString&, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnReceivedFreeServer, const FGameMapConnectInfo&, ServerInfo,const bool, bSuccess,const FString&, Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedRequestScore, const FPlayerScore&, PlayerScore, bool ,bSuccess, const FString& ,Msg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllServerDataRecive,const TArray<FServerData>&,ServerListData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCreateRoomFinish,FString,RoomId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceivedServerAddrFromName,FString,IP,FString,Port);



UCLASS()
class DEMO_API ALogin_Controller : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALogin_Controller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public: // 尝试登录
	
	UFUNCTION(Server,Reliable)
	void Try_Login(const FString& in_account,const FString& in_passwd);
	
	// 登录结果
	UFUNCTION(Client,Reliable)
	void ReceivedLoginResult(const ELoginResultType& InLoginResult,const FString& Msg);
public: // 尝试注册
	UFUNCTION(Server,Reliable)
	void Try_Register(const FString& in_account,const FString& in_passwd);

	UFUNCTION(Client,Reliable)
	void ReceivedRegisterResult(const ERegisterResultType& InLoginResult, const FString& Msg);
public: // 加入服务器
	UFUNCTION(Server,Reliable)
	void Join_Server();
	UFUNCTION(Server,Reliable)
	void Leave_Server();

	// 创建房间
	UFUNCTION(Server,Reliable)
	void CreateRoom();
	UFUNCTION(Client,Reliable)
	void ReceivedRoomID(const FString& RoomID);
	UFUNCTION(Client,Reliable)
	void ReceivedServerNameResult(const FString& IP,const FString& Port);
	
	
public: // 尝试获取一个空闲的服务器
	UFUNCTION(Server,Reliable)
	void Try_RequestFreeServer(const FString& in_account);

	UFUNCTION(Client,Reliable)
	void ReceivedFreeServer(const FGameMapConnectInfo& ServerInfo,const bool bSuccess,const FString& Msg);

	UFUNCTION(Server,Reliable)
	void RequestAllServerInfo();

	UFUNCTION(Client,Reliable)
	void OnReciveAllServerInfo(const TArray<FServerData>& ServerListData);

	UFUNCTION(Server,Reliable)
	void GetServerFromName(const FString& ServerName);

public: // 尝试获取分数
	UFUNCTION(Server,Reliable)
	void Try_Request_Score(const FString& in_account);

	UFUNCTION(Client,Reliable)
	void ReceivedRequestScore(FPlayerScore PlayerScore, bool bSuccess, const FString& Msg);

	

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 获取账号
	FString GetAccount();

	UFUNCTION(BlueprintPure)
	void GetAccount(FString &out_Account,FString &out_Password);
	
	void update_account(const FString& in_account, const FString& in_password);

	UFUNCTION(BlueprintCallable)
	void RecordAccount(const FString& in_account,const FString& in_password);

public: // 标记为登出
	void MarkLogout();
	bool IsLogout();

public: //账号相关
	UFUNCTION()
	void OnRep_Account();

	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnRep_Account")
	void k2_OnRep_Account();

public: //委托
	UPROPERTY(BlueprintAssignable)
	FOnLoginResult OnLoginResult;
	UPROPERTY(BlueprintAssignable)
	FOnLoginResult OnSyncLoginResult;

	UPROPERTY(BlueprintAssignable)
	FOnRegisterResult OnRegisterResult;
	UPROPERTY(BlueprintAssignable)
	FOnReceivedServerList OnReceivedServerList;
	UPROPERTY(BlueprintAssignable)
	FReceivedRequestScore OnReceivedRequestScore;
	UPROPERTY(BlueprintAssignable)
	FOnReceivedFreeServer OnReceivedFreeServer;
	UPROPERTY(BlueprintAssignable)
	FOnAllServerDataRecive OnReceivedServerInfoList;
	UPROPERTY()
	FOnCreateRoomFinish OnCreateRoomFinish;
	UPROPERTY()
	FReceivedServerAddrFromName ReceivedServerAddrFromName;

	
	
private:

	// 账号和密码
	UPROPERTY(BlueprintReadOnly,Replicated,meta=(AllowPrivateAccess),ReplicatedUsing=OnRep_Account)
	FString account;
	UPROPERTY(BlueprintReadOnly,Replicated,meta=(AllowPrivateAccess),ReplicatedUsing=OnRep_Account)
	FString password;

	// 是否是登出的状态
	TOptional<bool> bIsLogout;

public: // 获取所有服务器列表

	// 尝试获取所有服务器
	UFUNCTION(Server,Reliable)
	void Try_GetServerList();

	UFUNCTION(Client,Reliable)
	void ReceivedServerList(const TArray<FGameMapConnectInfo> & ServerList, const FString& Msg);
	TArray<FGameMapConnectInfo> CurrentServerList;
};
