// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/AccountLoginNode.h"

#include "GameActors/Controller/Login_Controller.h"
#include "Type/LoginType.h"

UAccountLoginNode* UAccountLoginNode::TryLoginServer(UObject* WorldContext, const FString& user_name, const FString& passwd, ALogin_Controller* playerController, float
                                                     Timeout)
{
	auto obj=NewObject<UAccountLoginNode>(WorldContext);

	obj->Account=user_name;
	obj->Password=passwd;
	obj->controlelr=playerController;
	obj->request_timeout=Timeout;
	obj->RegisterWithGameInstance(WorldContext);
	
	obj->start();
	return obj;
}



void UAccountLoginNode::OnLoginResult(ELoginResultType LoginResult,FString Msg)
{
	this->OnComplete.Broadcast(LoginResult,Msg);
	this->SetReadyToDestroy();
}

void UAccountLoginNode::start()
{
	if (controlelr)
	{
		controlelr->OnLoginResult.AddDynamic(this,&UAccountLoginNode::OnLoginResult);
		//controlelr->.AddDynamic(this,&UAccountLoginNode::OnLoginResult);
		
		controlelr->Try_Login(this->Account,this->Password);
		
	}
	else
	{
		this->SetReadyToDestroy();
	}
}

void UAccountLoginNode::OnTimeOut()
{
	OnLoginResult(ELoginResultType::LoginFail_LoginServerLost,TEXT("time out"));
}


