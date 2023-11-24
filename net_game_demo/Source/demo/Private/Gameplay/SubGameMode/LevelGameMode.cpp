// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/SubGameMode/LevelGameMode.h"

#include "GameActors/Controller/PlayerPawnController.h"
#include "Gameplay/FunctionLibrary/GameFunctionLibrary.h"
#include "Gameplay/PlayerState/NetPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/ServerStatusSubsystem.h"


// Sets default values
ALevelGameMode::ALevelGameMode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->PlayerStateClass=ANetPlayerState::StaticClass();
	
}

// Called when the game starts or when spawned
void ALevelGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALevelGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	for (auto [NetID,account]:accountPair)
	{
		if (NetID==NewPlayer->GetNetConnection()->PlayerId)
		{
			if(auto playerController=Cast<APlayerPawnController>(NewPlayer))
			{
				playerController->RecordAccount(account.Key,account.Value);
				// 添加用户

			}
		}
	}

	TArray<FString> InPlayers;
	for(auto [netid,account]:accountPair)
	{
		InPlayers.Push(account.Key);
	}
	
	UServerStatusSubsystem::GetCurrent(this)->SendPlayersWithList(InPlayers);
}

void ALevelGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	FString Account;
	FString password;
	if (Address=="127.0.0.1")
	{
		// 设置默认账号
		Account="aaa";
		password="a";
		// 记录账号
		// 是调试 直接允许进入
	}else
	{
		Account=UGameplayStatics::ParseOption(Options,"account");
		password=UGameplayStatics::ParseOption(Options,"password");
		if (Account.IsEmpty()||password.IsEmpty())
		{
			ErrorMessage="Account or Password is Empty";
			return Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

			// 不重复
		}
		// @TODO:判断是否人数超纲 
		
	}
	accountPair.Add(UniqueId,{Account,password});

	return Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void ALevelGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	// 游戏登出


	if (IsValid(Exiting->GetNetConnection()))
	{
		TArray<FString> Players;
		accountPair.Remove(Exiting->GetNetConnection()->PlayerId);

		for(auto [netid,account]:accountPair)
		{
			Players.Push(account.Key);
		}	
		if (auto server_status_subsystem=UServerStatusSubsystem::GetCurrent(this->GetWorld()))
		{
			server_status_subsystem->SendPlayersWithList(Players);
		}

	}

}


