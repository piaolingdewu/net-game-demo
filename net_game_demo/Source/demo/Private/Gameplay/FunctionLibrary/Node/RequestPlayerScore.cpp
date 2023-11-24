// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/RequestPlayerScore.h"
#include "Config/ServerConfig.h"
#include "GameActors/Controller/Login_Controller.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

URequestPlayerScore* URequestPlayerScore::RequestPlayerScore(UObject* WorldContext, ALogin_Controller* PlayerController,const FString& account)
{
	auto obj=NewObject<URequestPlayerScore>(WorldContext);

	obj->playerController=PlayerController;
	obj->Account=account;
	obj->start();

	return obj;
}

void URequestPlayerScore::OnCallbackRequestPlayerScore(const FPlayerScore& PlayerScore, bool bSuccess,
	const FString& Msg)
{
	if (this->IsReadyForFinishDestroy())
	{
		this->OnReceivedScore.Broadcast(PlayerScore,bSuccess,Msg);
		this->SetReadyToDestroy();
	}
}


void URequestPlayerScore::start()
{
	if (this->playerController)
	{
		playerController->OnReceivedRequestScore.AddDynamic(this,&URequestPlayerScore::OnCallbackRequestPlayerScore);
		
		playerController->Try_Request_Score(Account);
	}
}

void URequestPlayerScore::OnTimeOut()
{
	OnCallbackRequestPlayerScore({},false,"timeout");
}
