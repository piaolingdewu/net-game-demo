// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/ServerConfig.h"


UServerConfig::UServerConfig()
{
	UserVersion=TEXT("Test");

	server_record.Add(UserVersion,{"127.0.0.1","7001"});
	server_record.Add(TEXT("Stable"),{"192.168.10.246","7001"});
}

FPrimaryAssetId UServerConfig::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ServiceConfig");
}

UServerConfig* UServerConfig::GetServerConfig()
{
	return LoadObject<UServerConfig>(nullptr, TEXT("/Game/Config/ServerConfig_Config"));;
}

FString UServerConfig::GetUrl()
{
	FString out_url="";

	FString Server;
	// 读取命令行参数
	FParse::Value(FCommandLine::Get(),TEXT("ControllerServer"),Server);

	if (!Server.IsEmpty())
	{
		out_url=Server;
	}else
	{
		if (auto config=GetServerConfig())
		{
			out_url=FString::Format(TEXT("{0}:{1}"),{*config->server_record[config->UserVersion].ServerName,*config->server_record[config->UserVersion].ServerPort});
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("Can't find ServerConfig"));
		}

	}
	return out_url;
}
