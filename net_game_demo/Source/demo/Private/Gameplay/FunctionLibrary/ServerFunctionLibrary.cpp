// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"

#include <chrono>

#include "HttpManager.h"
#include "HttpModule.h"
#include "SocketSubsystem.h"
#include "Config/ServerConfig.h"
#include "Gameplay/NetGameInstance.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::send_request(const FString& verb, FString Url,
                                                                                   const TMap<FString, FString>& headers, const FString& body, float timeout, bool bWaitResponse)
{
	auto request=FHttpModule::Get().CreateRequest();
	
	for (auto [head,val]:headers)
	{
		request->SetHeader(head,val);
	}
	request->SetTimeout(timeout);
	request->SetVerb(verb);
	request->SetURL(Url);
	if (!body.IsEmpty())
	{
		request->SetContentAsString(body);
	}
	if (bWaitResponse)
	{
		if (!request->ProcessRequest())
		{
			UE_LOG(LogTemp,Warning,TEXT("Request Faile"));
		}
		
		while (request->GetStatus()==EHttpRequestStatus::Processing)
		{
			FPlatformProcess::Sleep(0.1);
			FHttpModule::Get().GetHttpManager().Tick(0.1);
			//request.Get().Tick(0.1);
		}
	}
	//request->ClearTimeout();
	
	return request;

}

TRequestResultWithMsg UServerFunctionLibrary::check_response(FHttpResponsePtr Response)
{
	
	switch (Response->GetResponseCode()) {
	case EHttpResponseCodes::RequestTimeout:
		// timeout
		return TRequestResultWithMsg(false,"timeout ");

		break;
	case EHttpResponseCodes::Ok:

		// 完成请求
		auto rev_body=Response->GetContentAsString();
		TSharedRef<TJsonReader<>>reader=TJsonReaderFactory<>::Create(Response->GetContentAsString());
		TSharedPtr<FJsonObject>jsonObject=MakeShareable(new FJsonObject());
		FJsonSerializer::Deserialize(reader,jsonObject);

		if (jsonObject.IsValid())
		{
			auto result=jsonObject->Values["success"]->AsBool();
			auto msg=jsonObject->Values["msg"]->AsString();
			return TRequestResultWithMsg(result,msg);

		}

		// 并不是标准的json Response
		UE_LOG(LogTemp,Warning,TEXT("unknow json type "));
		return TRequestResultWithMsg(false,TEXT("unknow json type "));
		break;
	}
	return TRequestResultWithMsg(false,"Unknow Error:status code is "+FString::FromInt(Response->GetResponseCode()));
}

TRequestResultWithJson UServerFunctionLibrary::get_response_to_json(FHttpResponsePtr Response)
{
	TSharedPtr<FJsonObject>jsonObject=MakeShareable(new FJsonObject());
	if (Response->GetResponseCode()==200)
	{
		TSharedRef<TJsonReader<>>reader=TJsonReaderFactory<>::Create(Response->GetContentAsString());
		FJsonSerializer::Deserialize(reader,jsonObject);
		return TRequestResultWithJson(true,jsonObject);
	}
	UE_LOG(LogTemp,Warning,TEXT("Response Failed"));
	return TRequestResultWithJson(false,nullptr);
}

void UServerFunctionLibrary::GetCurrentServerHost(UObject* WorldContext, FString& host, FString& port)
{
	auto url=WorldContext->GetWorld()->URL;
	port=FString::FromInt(url.Port);

	if (WorldContext&&WorldContext->GetWorld())
	{
		ISocketSubsystem* socket_subsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		bool canBind=false;
		TSharedRef<FInternetAddr> local_address = socket_subsystem->GetLocalHostAddr(*GLog,canBind);
		host=local_address->ToString(false);
	}
}

FString UServerFunctionLibrary::GetServerHost_V2(UObject* WorldContext)
{
	

	if (WorldContext&&WorldContext->GetWorld())
	{
		auto url=WorldContext->GetWorld()->GetAddressURL();
		return url;
	}
	return {};
}

FString UServerFunctionLibrary::GetIpFromNetDriver(UObject* WorldContext)
{
	if (WorldContext&&WorldContext->GetWorld())
	{
		if (auto NetDriver=WorldContext->GetWorld()->GetNetDriver())
		{
			if (auto connection=NetDriver->ServerConnection)
			{
				return connection->LowLevelGetRemoteAddress();
			}
		}
	}
	return {};
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::try_login(const FString& account,
                                                                                const FString& In_Passwd)
{
	auto head=TMap<FString,FString>();
	head.Add({"account",account});
	head.Add({"password",In_Passwd});

	return UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/valid_account",head,{}, 2);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::join_server(const FString& Host,
	const FString& Port, const FString& account, const FString& password, float timeout)
{
	FString Body=FString::Format(TEXT("{\"login\": {\"account\": \"{0}\", \"password\": \"{1}\"}, \"server_add\": {\"ip\": \"{2}\", \"port\": \"{3}\"}}"),{account,password,Host,Port});
	
	auto Headers=TMap<FString,FString>();
	
	Headers.Add({"Content-Type","application/json"});
	
	return UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/join_server",Headers,Body);

}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::leave_server(const FString& Host,
	const FString& Port, const FString& account, const FString& password, float timeout)
{
	FString Body=FString::Format(TEXT("{\"login\": {\"account\": \"{0}\", \"password\": \"{1}\"}, \"server_add\": {\"ip\": \"{2}\", \"port\": \"{3}\"}}"),{account,password,Host,Port});
	
	auto Headers=TMap<FString,FString>();
	
	Headers.Add({"Content-Type","application/json"});
	
	return UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/quit_server",Headers,Body);
}

bool UServerFunctionLibrary::GoToServer(UObject* WorldContext, const FString& Host, const FString& Port,
                                        const FString& account, const FString& password)
{
	if (auto gameInstance=Cast<UNetGameInstance>(UGameplayStatics::GetGameInstance(WorldContext)))
	{
		switch (gameInstance->TryConnectServer(Host,Port, account, password))
		{
		case EBrowseReturnVal::Success:
			return true;
			break;
		case EBrowseReturnVal::Failure:
			return false;
			break;
		case EBrowseReturnVal::Pending:
			return true;
		}
	}
	return false;
}


TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::add_serverInfo_to_remote(const FString& hostName,
                                                                                               const FString& port,bool bAutoProcess)
{
	FString body=FString::Format(TEXT("\"ip\":{0},\"port\":{1}"),{hostName,port});
	auto request=send_request("POST",UServerConfig::GetUrl()+"/add_server",{{"Content-Type","application/json"}},body,5.0f,false);

	if (bAutoProcess)
	{
		request->ProcessRequest();
	}
	return request;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::remove_serverInfo_to_remote(
	const FString& hostName, const FString& port,bool bAutoProcess)
{
	FString body=FString::Format(TEXT("\"ip\":{0},\"port\":{1}"),{hostName,port});
	
	auto request=send_request("POST",UServerConfig::GetUrl()+"/stop_server",{{"Content-Type","application/json"}},body,5.0f,false);
	if (bAutoProcess)
	{
		request->ProcessRequest();
	}
	
	return request;
}

//TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::send_server_heartbeat(const FString& hostName,
//                                                                                            const FString& port, const FServerInfo& ServerInfo,bool bAutoProcess)
//{
//	auto request=	send_request("POST",UServerConfig::GetUrl()+"/heartbeat",{{"Content-Type","application/json"}},ServerInfo.to_json(),5.0f,false);;
//	if (bAutoProcess)
//	{
//		request->ProcessRequest();
//	}
//	return request;
//}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::get_all_server(float timeout)
{
	auto request=send_request("GET",UServerConfig::GetUrl()+"/get_all_server",{{"Content-Type","application/json"}},TEXT(""),timeout,false);

	return request;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::get_all_server_info(float timeout)
{
	return send_request("GET",UServerConfig::GetUrl()+"/get_all_servers_info",{{"Content-Type","application/json"}},TEXT(""),timeout,false);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::Create_Room(const FString& Account, int timeout)
{
	return send_request("POST",UServerConfig::GetUrl()+"/create_room",{{"Content-Type","application/json"},{"Account",Account}},TEXT(""),timeout,false);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UServerFunctionLibrary::get_server_from_name(
	const FString& ServerName, float timeout)
{
	return send_request("GET",UServerConfig::GetUrl()+"/get_server_from_name",{{"Content-Type","application/json"},{"ServerName",ServerName}},TEXT(""),timeout,false);
}

int64  UServerFunctionLibrary::GetUTCMilliSecond()
{
	FDateTime date=FDateTime::UtcNow();
	return date.ToUnixTimestamp()*1000+date.GetMillisecond();
}
