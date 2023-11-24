// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/GetServerList.h"

#include "GameActors/Controller/Login_Controller.h"

UGetServerList* UGetServerList::GetServerList(UObject* WorldContext, ALogin_Controller* LoginController)
{
	auto obj=NewObject<UGetServerList>();

	obj->controller=LoginController;
	obj->request_timeout=2;
	obj->start();
	return obj;
}

void UGetServerList::OnReceiveServerList(const TArray<FGameMapConnectInfo>& ServerList, const FString& Msg)
{
	if (ServerList.IsEmpty())
	{
		this->OnServerListReceive.Broadcast(ServerList,Msg,false);
	}
	else
	{
		this->OnServerListReceive.Broadcast(ServerList,Msg,true);
	}
	this->SetReadyToDestroy();
}

void UGetServerList::start()
{
	if (controller)
	{
		controller->OnReceivedServerList.AddDynamic(this,&UGetServerList::OnReceiveServerList);
		
		controller->Try_GetServerList();
	}
}

void UGetServerList::OnTimeOut()
{
	OnReceiveServerList(TArray<FGameMapConnectInfo>(),"timeout");
}
