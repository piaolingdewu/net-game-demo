// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Controller/Login_Controller.h"
#include "Config/ServerConfig.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ALogin_Controller::ALogin_Controller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bAutoManageActiveCameraTarget=false;
}

// Called when the game starts or when spawned
void ALogin_Controller::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALogin_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALogin_Controller::ReceivedRoomID_Implementation(const FString& RoomID)
{
	this->OnCreateRoomFinish.Broadcast(RoomID);
}

void ALogin_Controller::CreateRoom_Implementation()
{
	// @TODO: 思路 1. 发送创建房间请求，2. 等待服务器返回房间id 3. 不停的查询房间id的端口和ip 一旦存在就加入其中。

	auto request=UServerFunctionLibrary::Create_Room(this->account,2);
	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [bSuccess,msg]=UServerFunctionLibrary::check_response(Response);

		if (bSuccess)
		{
			auto [Success,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			if (Success)
			{
				auto server_id=json_data->Values["data"]->AsObject()->Values["server_name"]->AsString();
				this->ReceivedRoomID(server_id);
			}

		}
	});

	request->ProcessRequest();
	
	
}

void ALogin_Controller::Join_Server_Implementation()
{
	FString Port;
	FString Host;
	//UServerFunctionLibrary::join_server(ServerData);
	UServerFunctionLibrary::GetCurrentServerHost(this,Host,Port);

	auto request=UServerFunctionLibrary::join_server(Host,Port,this->account,this->password, 2);
	
	request->OnProcessRequestComplete().BindWeakLambda(this,[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [bsuccess,msg]=UServerFunctionLibrary::check_response(Response);

		if (bsuccess)
		{
			UE_LOG(LogTemp,Warning,TEXT("Join Server Success: %s"),*account);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Join Server Failed: %s"),*msg);
		}
	});

	

	request->ProcessRequest();
}

void ALogin_Controller::Leave_Server_Implementation()
{
	FString Host;
	FString Port;
	//UServerFunctionLibrary::join_server(ServerData);
	UServerFunctionLibrary::GetCurrentServerHost(this->GetWorld(),Host,Port);
	
	auto request=UServerFunctionLibrary::leave_server(Host,Port,this->account,this->password, 2);
	
	request->OnProcessRequestComplete().BindWeakLambda(this,[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [bsuccess,msg]=UServerFunctionLibrary::check_response(Response);
		if (bsuccess)
		{
			UE_LOG(LogTemp,Warning,TEXT("Leave Server Success acc:%s "),*account);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Join Server Failed: %s"),*msg);
		}
	});
	request->ProcessRequest();
}



void ALogin_Controller::ReceivedLoginResult_Implementation(const ELoginResultType& InLoginResult, const FString& Msg)
{
	this->OnLoginResult.Broadcast(InLoginResult,Msg);
}


void ALogin_Controller::Try_RequestFreeServer_Implementation(const FString& in_account)
{
	TMap<FString, FString> headers;
	// 这一段是运行在服务器上的
	headers.Add({"account",in_account});
	auto request=UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/get_free_server",headers,{});

	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		// 尝试获取列表
		auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
		
		if (result)
		{
			auto [a,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			auto host=json_data->Values["data"]->AsObject()->Values["ip"]->AsString();
			auto port=json_data->Values["data"]->AsObject()->Values["port"]->AsString();
			
			this->ReceivedFreeServer(FGameMapConnectInfo{host,port},true,Msg);
		}
		else
		{
			// 调用失败
			//this->ReceivedScore({0});
			this->ReceivedFreeServer(FGameMapConnectInfo{},false,Msg);
		}
	});
	
	
	request->ProcessRequest();
}

void ALogin_Controller::ReceivedFreeServer_Implementation(const FGameMapConnectInfo& ServerInfo, const bool bSuccess,
	const FString& Msg)
{
	// 本地
	this->OnReceivedFreeServer.Broadcast(ServerInfo,bSuccess,Msg);
}


void ALogin_Controller::Try_Request_Score_Implementation(const FString& in_account)
{
	// 当前正在服务器上执行
	if (true/* @TODO: 目前限制强制登录 不做其他考虑*/)
	{
		TMap<FString, FString> headers;
		headers.Add({"account",in_account});
		
		auto request=UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/get_player_score",headers,{});
		
		request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
				// 尝试获取列表
				auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
			
				if (result)
				{
					auto [a,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
					auto rank=(int)json_data->Values["data"]->AsObject()->Values["rank"]->AsNumber();
					auto score=(int)json_data->Values["data"]->AsObject()->Values["score"]->AsNumber();
					// 记录当前分数到Remote上
					
					this->ReceivedRequestScore(FPlayerScore{score,rank},true,Msg);
				}
				else
				{
					// 调用失败
					//this->ReceivedScore({0});
					this->ReceivedRequestScore(FPlayerScore{},false,Msg);
				}
		});
		request->ProcessRequest();
	}
	else
	{
		// 未登录
		//UKismetSystemLibrary::PrintString(this,FString::Format(TEXT("{0}:该控制器没有登录"),{this->GetPlayerController()->GetName()}),true,true,FLinearColor::Red,5.0f);
		
	}

	
}


void ALogin_Controller::ReceivedRequestScore_Implementation(FPlayerScore PlayerScore, bool bSuccess, const FString& Msg)
{
	// 获取分数
	
	this->OnReceivedRequestScore.Broadcast(PlayerScore,bSuccess,Msg);
}


void ALogin_Controller::RequestAllServerInfo_Implementation()
{
	// 获取所有的服务器
	auto request=UServerFunctionLibrary::get_all_server_info(2);

	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
				TArray<FServerData> OutServerList;
		auto [bSuccess,Msg]=UServerFunctionLibrary::check_response(Response);
		if (bSuccess)
		{
			// 读取值
			auto [data_success,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			if (data_success)
			{
				auto data_list=json_data->Values["data"]->AsArray();
				for (auto item:data_list)
				{
					// 转化成obj
					auto jsonObject=item->AsObject();
					
					FServerData serverData;
		
					TSharedPtr<FJsonObject> addrObject = jsonObject->GetObjectField("addr");
					serverData.ip = addrObject->GetStringField("ip");
					serverData.port = addrObject->GetIntegerField("port");

					TSharedPtr<FJsonObject> runtimeDataObject = jsonObject->GetObjectField("runtime_data");

					serverData.runtime_data.create_time = runtimeDataObject->GetIntegerField("create_time");
					serverData.runtime_data.heart_beat_speed = runtimeDataObject->GetIntegerField("heart_beat_speed");

					TSharedPtr<FJsonObject> heartbeatObject = runtimeDataObject->GetObjectField("heartbeat");
					serverData.runtime_data.heartbeat.last_10_ticks_average_time = heartbeatObject->GetIntegerField("last_10_ticks_average_time");
					serverData.runtime_data.heartbeat.last_hart_beat_time = heartbeatObject->GetIntegerField("last_hart_beat_time");
					serverData.runtime_data.heartbeat.last_tick_time = heartbeatObject->GetIntegerField("last_tick_time");
					serverData.runtime_data.heartbeat.status = heartbeatObject->GetStringField("status");

					serverData.runtime_data.max_player = runtimeDataObject->GetIntegerField("max_player");
					serverData.runtime_data.player_count = runtimeDataObject->GetIntegerField("player_count");

					const TArray<TSharedPtr<FJsonValue>>* playerListArray;
					if (runtimeDataObject->TryGetArrayField("player_list", playerListArray))
					{
						for (const TSharedPtr<FJsonValue>& value : *playerListArray)
						{
							FString playerName = value->AsString();
							serverData.runtime_data.player_list.Add(playerName);
						}
					}
					serverData.runtime_data.round = runtimeDataObject->GetIntegerField("round");

					OutServerList.Push(serverData);
				}
				
				this->OnReciveAllServerInfo(OutServerList);
			}
		}

		
		
		
	});
	
	

	request->ProcessRequest();
}

void ALogin_Controller::OnReciveAllServerInfo_Implementation(const TArray<FServerData>& ServerListData)
{
	OnReceivedServerInfoList.Broadcast(ServerListData);
}


void ALogin_Controller::GetServerFromName_Implementation(const FString& ServerName)
{
	auto request=UServerFunctionLibrary::get_server_from_name(ServerName);

	request->OnProcessRequestComplete().BindWeakLambda(this,[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [bSuccess,Msg]=UServerFunctionLibrary::check_response(Response);

		if (bSuccess)
		{
			auto [Success,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			if (Success)
			{
				auto ip=json_data->Values["data"]->AsObject()->Values["ip"]->AsString();
				auto port=json_data->Values["data"]->AsObject()->Values["port"]->AsString();
				this->ReceivedServerNameResult(ip,port);
			}
		}else
		{
			//UE_LOG(LogTemp,Error,TEXT(__FUNCTION__":获取服务器失败:%s"),*Msg);
		}

		
	});

	request->ProcessRequest();
}

void ALogin_Controller::ReceivedServerNameResult_Implementation(const FString& IP, const FString& Port)
{
	this->ReceivedServerAddrFromName.Broadcast(IP,Port);
}

void ALogin_Controller::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALogin_Controller,account);
	DOREPLIFETIME(ALogin_Controller,password);
	
}



FString ALogin_Controller::GetAccount()
{
	return this->account;
}

void ALogin_Controller::GetAccount(FString& out_Account, FString& out_Password)
{
	out_Account=this->account;
	out_Password=this->password;
}

void ALogin_Controller::MarkLogout()
{
	this->bIsLogout=false;
}

bool ALogin_Controller::IsLogout()
{
	return bIsLogout.Get(false);
}

void ALogin_Controller::OnRep_Account()
{
	this->k2_OnRep_Account();
	
}

void ALogin_Controller::update_account(const FString& in_account, const FString& in_password)
{
	this->account=in_account;
	this->password=in_password;
}

void ALogin_Controller::RecordAccount(const FString& in_account, const FString& in_password)
{
	account=in_account;
	password=in_password;
}


void ALogin_Controller::Try_GetServerList_Implementation()
{
	// 这里尝试在服务器里获取专用服务器列表
	auto request=UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/get_all_servers",{},{}, 2);

	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		// 尝试获取列表
		auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
		
		if (result)
		{
			auto [a,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			TArray<FGameMapConnectInfo> out_list;
			for (auto server_info:json_data->Values["data"]->AsArray())
			{
				auto ip=server_info->AsObject()->Values["ip"]->AsString();
				auto port=server_info->AsObject()->Values["port"]->AsString();
				
				// @TODO: 这里可以设置玩家数量
				out_list.Add({ip,port,3});
				
			}
			this->ReceivedServerList(out_list,Msg);
		}
		else
		{
			// 调用失败
			this->ReceivedServerList({},Msg);
		}
	});
	request->ProcessRequest();
}

void ALogin_Controller::ReceivedServerList_Implementation(const TArray<FGameMapConnectInfo>& ServerList,const FString& Msg)
{
	// 获取到了服务器列表 @TODO:这里可以分为十个或者五个一组 然后分页显示
	this->OnReceivedServerList.Broadcast(ServerList,Msg);
	this->CurrentServerList=ServerList;
}

void ALogin_Controller::Try_Login_Implementation(const FString& in_account, const FString& in_passwd)
{
	this->account=in_account;
	this->password=in_passwd;
	
	auto request=UServerFunctionLibrary::try_login(in_account,in_passwd);

	// 同步登录账号
	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
		if (result)
		{
			// 登录成功
			auto [bSuccess,json_data]=UServerFunctionLibrary::get_response_to_json(Response);
			FString code_string=json_data->Values["data"]->AsObject()->Values["result_code"]->AsString();
			auto code=FCString::Atoi(*code_string);
			
			auto msg=json_data->Values["data"]->AsObject()->Values["result_type"]->AsString();
			auto resutl=ELoginResultType::None;
			switch (code)
			{
			case 0:
				// success
				resutl=ELoginResultType::LoginSuccess;
				break;
			case 1:
				resutl=ELoginResultType::LoginFail_PasswdOrAccountError;
				break;
			case 2:
				resutl=ELoginResultType::LoginFail_AccountNotFound;
				break;
			default:
				resutl=ELoginResultType::None;
			}
			this->ReceivedLoginResult(resutl, msg);

		}
		else
		{
			// 登录错误 @TODO:这里以后要注意实现一下错误码 不知道错误码就只能打印错误信息了
			UE_LOG(LogTemp,Display,TEXT("login fail:%s"),*Msg);
			this->ReceivedLoginResult(ELoginResultType::LoginFail_LoginServerLost, Msg);
		}
	});
	request->ProcessRequest();
}

void ALogin_Controller::ReceivedRegisterResult_Implementation(const ERegisterResultType& InLoginResult,const FString& Msg)
{
	this->OnRegisterResult.Broadcast(InLoginResult,Msg);
}

void ALogin_Controller::Try_Register_Implementation(const FString& in_account, const FString& in_passwd)
{
	// 尝试注册
	auto request=UAccountSubsystem::try_register(in_account,in_passwd, 2);
	request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
		if (result)
		{
			// 注册成功
			UE_LOG(LogTemp,Display,TEXT("register success"));
			this->ReceivedRegisterResult(ERegisterResultType::RegisterSuccess,Msg);
		}
		else
		{
			// 注册错误
			UE_LOG(LogTemp,Display,TEXT("register fail:%s"),*Msg);
			this->ReceivedRegisterResult(ERegisterResultType::Fail_None,Msg);
		}
	});
	request->ProcessRequest();
}


