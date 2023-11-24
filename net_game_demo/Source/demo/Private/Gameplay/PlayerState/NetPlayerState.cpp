// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/PlayerState/NetPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameActors/Controller/PlayerPawnController.h"
#include "Config/ServerConfig.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Gameplay/FunctionLibrary/Node/RequestPlayerScore.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ANetPlayerState::ANetPlayerState()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 默认为未登录
}

// Called when the game starts or when spawned
void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANetPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void ANetPlayerState::SyncScoreToServer_Implementation(const FPlayerScore& SyncPlayerScore)
{
	// 发送同步请求
	
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/update_player_score",{},{});
	request->OnProcessRequestComplete().BindWeakLambda(this,[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		auto [result,Msg]=UServerFunctionLibrary::check_response(Response);
		if (result)
		{
			UE_LOG(LogTemp,Display,TEXT("SyncScoreToServer Success"))
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("SyncScoreToServer Fail:%s"),*Msg);
		}
	});
	
	request->ProcessRequest();
}





