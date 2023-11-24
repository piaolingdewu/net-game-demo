// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ServerStatusSubsystem.h"
#include "EngineUtils.h"
#include "Config/GameLevelConfig.h"
#include "Config/ServerConfig.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/GameplayEventManager.h"
#include "Type/NetGameTagsData.h"
#include "chrono"
#include "GameActors/Controller/Login_Controller.h"
#include "Gameplay/FunctionLibrary/GameFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UServerStatusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UServerStatusSubsystem::Deinitialize()
{
	Super::Deinitialize();


	if (bEnableHeartbeat)
	{
		StopServer();
	}
}

void UServerStatusSubsystem::AddLevelToServer(UObject* WorldContext)
{
#if WITH_SERVER_CODE
	// 向服务器发送本服务器的消息
	StartServer();
	ServerInfo.server_heartbeat.last_hart_beat_time=UServerFunctionLibrary::GetUTCMilliSecond();
	// 启用心跳包
	this->bEnableHeartbeat=true;
#endif

}

void UServerStatusSubsystem::OnGameplayEventReceive(const FGameplayTag& EventTag, const FGameplayEventData& Payload)
{
	if (EventTag.MatchesTag(FNetGameTagsData::PlayerSpawnEventTag))
	{
		// 玩家被放置了出来
		auto player_controller=Cast<APlayerController>(Cast<APlayerPawn>(Payload.Target)->GetController());
		if (player_controller)
		{
			LevelPlayersScoreList[player_controller].spawnCount+=1;
		}
		else
		{
			ensureMsgf(1,TEXT("Player Controller Not Found"));
		}
	}
}

void UServerStatusSubsystem::BindWorldEvent()
{
	UGameplayEventManager::GetCurrent(this)->OnGameplayEventReceive.AddDynamic(this,&UServerStatusSubsystem::OnGameplayEventReceive);
}

void UServerStatusSubsystem::StartServer()
{
	ServerInfo=FServerInfo();
	// 发送初始化请求

	TMap<FString, FString> headers;
	FString host;
	FString port;
	UServerFunctionLibrary::GetCurrentServerHost(this->GetWorld(),host, port);

	UE_LOG(LogTemp,Display,TEXT("Start Server: %s:%s"),*host,*port);
	FString ServerName;
	//GetServerName(ServerName);
	GetProcessID(ServerName);
	
	headers.Add({"Content-Type","application/json"});

	if (!ServerName.IsEmpty())
	{
		headers.Add({"ServerName","ServerName"});
	}
	
	FString body=FString::Format(TEXT("{\"ip\": \"{0}\", \"port\": \"{1}\",\"server_name\":\"{2}\"}"),{host,port,ServerName});

	
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/add_server",headers,body, 2);
	request->ProcessRequest();
	ServerInfo.server_addr.ip=host;
	ServerInfo.server_addr.port=port;
}

void UServerStatusSubsystem::StopServer()
{
	// 发送最后一个包
	//SendHeartbeat("Stop");
	FString host;
	FString ip;
	FString server_name;
	
	//this->GetServerName(server_name);
	GetProcessID(server_name);

	TMap<FString, FString> headers;
	headers.Add({"Content-Type","application/json"});
	
	UServerFunctionLibrary::GetCurrentServerHost(this->GetWorld(),host, ip);
	
	FString body=FString::Format(TEXT("{\"ip\": \"{0}\", \"port\": \"{1}\",\"server_name\":\"{2}\"}"),{host,ip,server_name});
	
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/stop_server",headers,body, 2);
	
	request->ProcessRequest();
}

/* 心跳包的格式
{
	"server_addr": {
		"ip": "38.144.2.239",
		"port": "1002"
	},
	"server_heartbeat": {
		"last_hart_beat_time": 1496979111383,
		"last_10_ticks_average_time": 1053534078373,
		"last_tick_time": 1489816062430
	}
}
*/

void UServerStatusSubsystem::SendHeartbeat(FString Status)
{
	// 计算前10帧的延迟
	float average_time=0;

	for(auto i:last_10_tick_queue)
		average_time+=i;
	
	average_time/=10;

	// 将时间转化成毫秒
	int64 last_10_ticks_average_time=(int)(average_time*1000);
	
	

	
	ServerInfo.server_heartbeat.status=Status;
	{
		// 计算一下玩家数量
		TActorIterator<APlayerController> Iterator(this->GetWorld());
		int32 player_count=0;
		for(;Iterator;++Iterator)
		{
			player_count+=1;
		}
	}
	
	FString ServerName;
	//this->GetServerName(ServerName);
	GetProcessID(ServerName);


	//UE_LOG(LogTemp,Display,TEXT("ServerName: %s"),*ServerName);
	
	ServerInfo.server_heartbeat.last_10_ticks_average_time=last_10_ticks_average_time;
	
	auto last_heartbeat_time=ServerInfo.server_heartbeat.last_hart_beat_time;

	auto body=FString::Format(
		TEXT("{\"server_addr\": {\"ip\": \"{0}\", \"port\": \"{1}\",\"server_name\":\"{2}\"}, \"server_heartbeat\": {\"last_hart_beat_time\": {3}, \"last_10_ticks_average_time\": {4}, \"last_tick_time\": {5}, \"status\":\"{6}\"}}"),{
			ServerInfo.server_addr.ip,
			ServerInfo.server_addr.port,
			ServerName,
			ServerInfo.server_heartbeat.last_hart_beat_time,
			ServerInfo.server_heartbeat.last_10_ticks_average_time,
			ServerInfo.server_heartbeat.last_tick_time,
			ServerInfo.server_heartbeat.status
		});

	TMap<FString, FString> headers;

	headers.Add({"Content-Type","application/json"});
	
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/heartbeat",headers,body,2, false);
	

	request->OnProcessRequestComplete().BindWeakLambda(this,[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully){
		// 处理成功 记录最后发出的
		switch (Request->GetStatus())
		{
		case EHttpRequestStatus::NotStarted: break;
		case EHttpRequestStatus::Processing: break;
		case EHttpRequestStatus::Failed:
			// 失败
			ServerInfo.server_heartbeat.last_hart_beat_time=last_heartbeat_time;
			UKismetSystemLibrary::PrintString(this,TEXT("SendHeartbeat Failed"));

			break;
		case EHttpRequestStatus::Failed_ConnectionError: break;
		case EHttpRequestStatus::Succeeded:
			// 发送心跳包成功
			ServerInfo.server_heartbeat.last_hart_beat_time=UServerFunctionLibrary::GetUTCMilliSecond();
			//UKismetSystemLibrary::PrintString(this,TEXT("SendHeartbeat Success"));

			break;
		default: ;
		}
	});
	request->ProcessRequest();
	heartbeat_count+=1;

	if (heartbeat_count%5==0)
	{
		// 更新当前玩家数量
		this->SendPlayers();
	}
}

void UServerStatusSubsystem::SendPlayers()
{
	// 获取当前所有的玩家
	auto player_nums=UGameplayStatics::GetNumPlayerControllers(this->GetWorld());
	TArray<FString> Players;
	
	for (auto i=0;i<player_nums;++i)
	{
		if (auto login_controller=Cast<ALogin_Controller>(UGameplayStatics::GetPlayerController(this->GetWorld(),i)))
		{
			if (!login_controller->IsLogout())
			{
				Players.Push(login_controller->GetAccount());
			}
		}
	}

	SendPlayersWithList(Players);
}

void UServerStatusSubsystem::SendPlayersWithList(const TArray<FString>& InPlayers)
{
	TMap<FString, FString> headers;
	headers.Add({"Content-Type","application/json"});

	// 获取当前所有的玩家
	FString player_list;

	for (auto i=0;i<InPlayers.Num();++i)
	{
		player_list.Append(FString::Format(TEXT("\"{0}\""),{InPlayers[i]}));

			
			
		if (i+1<InPlayers.Num())
		{
			player_list.Append(",");
		}
	}
	
	FString ProcessID;
	GetProcessID(ProcessID);

	const FString Body=FString::Format(TEXT("{\"server_add\":{\"ip\":\"{0}\",\"port\":\"{1}\",\"server_name\":\"{2}\"},\"account_list\":[{3}]}"),{
		                                   this->ServerInfo.server_addr.ip,
		                                   this->ServerInfo.server_addr.port,
		                                   ProcessID,
		                                   player_list
	                                   });

	//UE_LOG(LogTemp,Display,TEXT("update_server_json->:%s"),*Body);
	
	
	
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/update_players",headers,Body,2, false);

	request->OnProcessRequestComplete().BindWeakLambda(this,[](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [bSuccess,Msg]=UServerFunctionLibrary::check_response(Response);

		if (bSuccess)
		{
			// 成功
			//UKismetSystemLibrary::PrintString(this,TEXT("SendPlayerStatus Success"));
			// 更新玩家数量请求成功
			UE_LOG(LogTemp,Display,TEXT("update players:%s"),*Msg);
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("SendPlayerStatus Failed: %s"),*Msg);
			
		}
	});
	
	//UE_LOG(LogTemp,Display,TEXT("%s"),*Body);
	
	request->ProcessRequest();

}

void UServerStatusSubsystem::Tick(float DeltaTime)
{
	// 最后一帧的时间
	if (bEnableHeartbeat)
	{
		auto name=this->GetWorld()->GetName();
		ServerInfo.server_heartbeat.last_tick_time=UServerFunctionLibrary::GetUTCMilliSecond();
		
		last_10_tick_queue.PushFirst(DeltaTime);
		if (last_10_tick_queue.Num()>10)
		{
			last_10_tick_queue.PopLast();
		}
		tick_sum+=DeltaTime;
		
		if (tick_sum>this->heartbeat_rate)
		{
			// 发送心跳包
			SendHeartbeat("Running");
			tick_sum=0;
		}
	}
	
}

bool UServerStatusSubsystem::IsTickable() const
{
	return Super::IsTickable()&&UKismetSystemLibrary::IsDedicatedServer(this);
}

TStatId UServerStatusSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UServerStatusSubsystem, STATGROUP_Tickables);
}

UServerStatusSubsystem* UServerStatusSubsystem::GetCurrent(UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetSubsystem<UServerStatusSubsystem>();
}



bool UServerStatusSubsystem::IsGameMap()
{
	if (auto world=this->GetWorld())
	{
		auto gameMap=UGameLevelConfig::GetGameLevelConfig()->PlayMap;
		if (gameMap.IsValid())
		{
			if (this->GetPackage()==gameMap->GetPackage())
			{
				return true;
			}
		}
		else
		{
			// 没有载入到内存中 所以肯定不是游戏地图
			return false;
		}
	}
	return false;
}


void UServerStatusSubsystem::GetServerName(FString& ServerName)
{
	// 目前没有地方使用
	
	FParse::Value(FCommandLine::Get(), TEXT("ServerName"), ServerName);
}

void UServerStatusSubsystem::GetProcessID(FString& ProcessID)
{
	//FPlatformProcess::process
	
	ProcessID=FString::FromInt(FPlatformProcess::GetCurrentProcessId());
}

