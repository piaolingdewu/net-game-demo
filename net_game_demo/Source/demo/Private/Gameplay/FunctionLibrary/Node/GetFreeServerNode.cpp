// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/GetFreeServerNode.h"

#include "Config/ServerConfig.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"

UGetFreeServerNode* UGetFreeServerNode::GetFreeServer(UObject* WorldContext, FString in_account, ALogin_Controller* controller)
{
	auto obj=NewObject<UGetFreeServerNode>(WorldContext);
	obj->controller=controller;
	obj->account=in_account;
	obj->start();
	
	
	return obj;
}
/*
 * 得到的一个结果例子
{
	"success": true,
	"msg": "",
	"data": {
		"ip": "130.226.23.126",
		"port": "1000"
	}
}
 */

void UGetFreeServerNode::OnCallbackGetFreeServer(const FGameMapConnectInfo& ServerInfo, const bool bSuccess,
	const FString& Msg)
{
	OnReceiveFreeServer.Broadcast(ServerInfo,bSuccess,Msg);
	SetReadyToDestroy();
}

void UGetFreeServerNode::start()
{
	// 获取一个服务器

	if (controller)
	{
		controller->OnReceivedFreeServer.AddDynamic(this,&UGetFreeServerNode::OnCallbackGetFreeServer);
		controller->Try_RequestFreeServer(this->account);
	}
	
}

void UGetFreeServerNode::OnTimeOut()
{
	OnCallbackGetFreeServer({},false,"timeout");
}
