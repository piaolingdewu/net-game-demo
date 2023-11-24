// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameActors/Controller/Login_Controller.h"
#include "ServerFunctionLibrary.generated.h"


// TRequestResult(结果，错误信息)
using TRequestResultWithMsg = TTuple<bool,FString>;

// 自定义的Result (结果，T)
template<typename T>
using TRequestResultWithT = TTuple<bool,T>;

// 自定义的Result (结果，Json数据)
using TRequestResultWithJson = TTuple<bool,TSharedPtr<FJsonObject>>;



// 自定义的Result (，T)
template<typename T>
using TResult = TTuple<std::string,T>;



/**
 * 
 */
UCLASS()
class DEMO_API UServerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 尝试发送请求
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> send_request(const FString& verb, FString Url, const TMap<FString, FString>& headers, const FString& body, float timeout = 2.0f, bool bWaitResponse=false);

	// 查看request是否成功
	static TRequestResultWithMsg check_response(FHttpResponsePtr Response);

	// 查看request返回的json数据
	static TRequestResultWithJson get_response_to_json(FHttpResponsePtr Response);

	// 获取当前地址
	UFUNCTION(BlueprintPure,Category="Server",meta=(WorldContext=WorldContext))
	static void GetCurrentServerHost(UObject* WorldContext, FString &host, FString &port);

	// 获取地址
	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContext))
	static FString GetServerHost_V2(UObject* WorldContext);
	UFUNCTION(BlueprintPure,meta=(WorldContext=WorldContext))
	static FString GetIpFromNetDriver(UObject* WorldContext);


public: // 账号请求
	// 尝试登录
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> try_login(const FString& account, const FString& In_Passwd);
	
public:
	// 加入到服务器
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> join_server(const FString& Host, const FString& Port, const FString& account, const FString& password, float timeout);
	// 离开服务器
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> leave_server(const FString& Host, const FString& Port, const FString& account, const FString& password, float timeout);

	// 转换到服务器
	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContext))
	static bool GoToServer(UObject* WorldContext, const FString& Host, const FString& Port,
	                       const FString& account, const FString& password);

public: // 专用服务器相关
	// 在服务器列表中添加一个服务器
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> add_serverInfo_to_remote(const FString& hostName, const FString& port, bool bAutoProcess);
	// 在服务器列表中移除一个服务器
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> remove_serverInfo_to_remote(const FString& hostName, const FString& port, bool bAutoProcess);
	// 发送服务器心跳包
	//static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> send_server_heartbeat(const FString& hostName, const FString& port, const FServerInfo& ServerInfo, bool bAutoProcess);
	// 获取服务器列表
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> get_all_server(float timeout);
	// 获取所有服务器信息
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> get_all_server_info(float timeout = 2);
	// 创建玩家的专属房间
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Create_Room(const FString& Account, int timeout=2);
	
	static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> get_server_from_name(const FString& ServerName,float timeout=2.0);


public: // 时间
	static int64 GetUTCMilliSecond();
};
