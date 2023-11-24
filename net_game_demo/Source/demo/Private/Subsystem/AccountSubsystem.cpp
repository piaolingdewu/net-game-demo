// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/AccountSubsystem.h"

#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Config/ServerConfig.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UAccountSubsystem::try_login(const FString& user_name, const FString& passwd, float timeout)
{
	// 服务器不为空

	auto head=TMap<FString,FString>();
	head.Add({"account",user_name});
	head.Add({"password",passwd});

	return UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/valid_account",head,{}, 2);

}


TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UAccountSubsystem::try_register(const FString& user_name, const FString& passwd, float timeout)
{
	//TVariant<>
	// 尝试进行注册
	auto head=TMap<FString,FString>();
	head.Add({"Content-Type","application/json"});
	head.Add({"account",user_name});
	head.Add({"password",passwd});
	FString body=FString::Format(TEXT("{\"account\":\"{0}\",\"password\":\"{1}\"}"),{user_name,passwd});
	return UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/register_account",head,body, 2);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UAccountSubsystem::try_validAccount(const FString& user_name, const FString& passwd, float timeout)
{
	// 尝试进行账号验
    auto head=TMap<FString,FString>();
    head.Add({"account",user_name});
    head.Add({"password",passwd});

    return UServerFunctionLibrary::send_request("GET",UServerConfig::GetUrl()+"/valid_account_exists",head,{}, 2);
}

UAccountSubsystem* UAccountSubsystem::GetCurrent(UObject* WorldContext)
{
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UAccountSubsystem>();
}

