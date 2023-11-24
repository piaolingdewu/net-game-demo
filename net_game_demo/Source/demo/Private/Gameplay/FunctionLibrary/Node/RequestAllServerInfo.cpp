// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/RequestAllServerInfo.h"

#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"


URequestAllServerInfo* URequestAllServerInfo::RequestAllServerInfo(UObject* WorldContext, ALogin_Controller* login_controller)
{
	auto obj=NewObject<URequestAllServerInfo>(WorldContext);
	obj->login_controller=login_controller;
	obj->start();
	return obj;
}

void URequestAllServerInfo::RequestComplete(const TArray<FServerData>& data)
{
	Complete.Broadcast(data);
	this->SetReadyToDestroy();
	
}

void URequestAllServerInfo::start()
{
	if (login_controller)
	{
		login_controller->OnReceivedServerInfoList.AddDynamic(this,&URequestAllServerInfo::RequestComplete);
		login_controller->RequestAllServerInfo();
	}
}

void URequestAllServerInfo::OnTimeOut()
{
	this->RequestComplete({});
}
