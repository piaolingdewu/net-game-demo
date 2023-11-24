// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/CreateRoomNode.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"



UCreateRoomNode* UCreateRoomNode::CreateRoom(UObject* WorldContext, ALogin_Controller* Login_Controller,
                                             float TimeOut)
{
	auto obj=NewObject<UCreateRoomNode>(WorldContext);
	obj->controller=Login_Controller;

	obj->start();
	
	return obj;
}

void UCreateRoomNode::OnReceiveRoomServer(FString IP, FString Port)
{
	// 轮询到了服务器
	if (!bOnce)
	{
		this->GetWorld()->GetTimerManager().ClearTimer(PollServerHandle);
		this->OnCreateFinish.Broadcast(IP,Port);
		this->bOnce=true;
	}
	this->SetReadyToDestroy();
}

void UCreateRoomNode::ReceiveRoomName(FString RoomId)
{
	// 房间创建完毕，但是要获取对应的玩家房间的id
	this->RoomName=RoomId;
	
	// this->GetWorld()->GetTimerManager().SetTimer()
	this->controller->ReceivedServerAddrFromName.AddDynamic(this,&UCreateRoomNode::OnReceiveRoomServer);
	// 开始轮询服务器
	PollServer=FTimerDelegate::CreateWeakLambda(this,[this]()
	{
		if (!bOnce)
		{
			this->controller->GetServerFromName(this->RoomName);
		}

	});

	this->GetWorld()->GetTimerManager().SetTimer(PollServerHandle,PollServer,0.2,true);
}

void UCreateRoomNode::start()
{
	if (controller)
	{
		controller->OnCreateRoomFinish.AddDynamic(this,&UCreateRoomNode::ReceiveRoomName);
		controller->CreateRoom();
	}
}

void UCreateRoomNode::OnTimeOut()
{
	this->OnTimeout.Broadcast();
	SetReadyToDestroy();
}
