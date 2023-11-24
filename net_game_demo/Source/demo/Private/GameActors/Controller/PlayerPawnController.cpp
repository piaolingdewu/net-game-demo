// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Controller/PlayerPawnController.h"

#include "GameActors/Spawner/SpawnerHelper.h"
#include "GameFramework/PlayerStart.h"
#include "Gameplay/FunctionLibrary/GameFunctionLibrary.h"
#include "Gameplay/SubGameMode/LevelGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/StoreServerSubsystem.h"
#include <iterator>
#include <algorithm>
#include <tuple>
#include <vector>
#include "Config/ServerConfig.h"
#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"

// Sets default values
APlayerPawnController::APlayerPawnController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bAutoManageActiveCameraTarget=false;

}

// Called when the game starts or when spawned
void APlayerPawnController::BeginPlay()
{
	Super::BeginPlay();
	// 等待服务器初始化完成

	if (!UKismetSystemLibrary::IsServer(this))
	{
		USpawnerHelper::GetSpawner()->SetOwner(this);
	}
}

// Called every frame
void APlayerPawnController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsNetMode(NM_Client))
	{
		ProjectiveMouse();
	}
}

void APlayerPawnController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

AActor* APlayerPawnController::GetMouseActor()
{
	if (IsNetMode(NM_Client)&&ProjectiveToWorldClass)
	{
		if (!MouseActor)
		{
			MouseActor=this->GetWorld()->SpawnActor<AActor>(ProjectiveToWorldClass);
		}
		return MouseActor.Get();
	}
	return nullptr;
}

void APlayerPawnController::ProjectiveMouse()
{
	if (this->IsNetMode(NM_Client))
	{
		if (bEnableProjectiveMouse&&this->IsNetMode(NM_Client))
		{
			FVector WorldLocation;
			FVector WorldDirect;
			// 将鼠标投射到本地
			this->DeprojectMousePositionToWorld(WorldLocation,WorldDirect);
			

			FPlane Plane=UKismetMathLibrary::MakePlaneFromPointAndNormal(FVector::ZeroVector+FVector::UpVector*50,FVector::UpVector);
			UKismetMathLibrary::ProjectVectorOnToPlane(WorldDirect,FVector::UpVector);
			float out_dist;
			FVector HitPoint;
			//FVector::PointPlaneProject()
			UKismetMathLibrary::LinePlaneIntersection(WorldLocation,WorldLocation+WorldDirect*1000000,Plane,out_dist,HitPoint);
			// 转向目标
			this->GetMouseActor()->SetActorLocation(HitPoint);

			if (this->GetPawn())
			{
				auto rot=UKismetMathLibrary::FindLookAtRotation((this->GetPawn()->GetActorLocation()*FVector(1,1,0)),(HitPoint*FVector(1,1,0)));
				this->GetPawn()->SetActorRotation(rot);
			}

			return ;
		}

	}
}

void APlayerPawnController::InitPlayerState()
{
	Super::InitPlayerState();

}

void APlayerPawnController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// 初始化PlayerStatus构造完成
	this->k2_OnPlayerStateInit();
}



FString get_current_NetMode(UObject* WorldContext)
{
	FString out_net_name="invalid";
	
	if (GEngine&&WorldContext)
	{
		switch (GEngine->GetNetMode(WorldContext->GetWorld()))
		{
		case NM_Standalone:
			out_net_name=("Standalone");
			break;
		case NM_DedicatedServer:
			out_net_name="DedicatedServer";
			break;
		case NM_ListenServer:
			out_net_name="ListenServer";
			break;
		case NM_Client:
			out_net_name="Client";
			break;
		case NM_MAX:
			out_net_name="invalid";
			break;
		}
	}
	return out_net_name;
	
}



void APlayerPawnController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void APlayerPawnController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerPawnController,PlayerScore_Init);
	DOREPLIFETIME(APlayerPawnController,Remote_PlayerScore);
	
}



void APlayerPawnController::OnRep_PlayerScore_Init()
{
	bInitFromServer=true;
	// 玩家分数更新 @TODO:分数更新
	this->PlayerScore_Local.score+=PlayerScore_Init.score;
	this->PlayerScore_Local.rank+=PlayerScore_Init.rank;
}


void APlayerPawnController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	// 获取到Pawn在客户端当中
	OnClientPossessPawn(this->GetPawn());
	
}

void APlayerPawnController::SpawnPlayer_Implementation()
{
	if (!IsValid(PlayerClass)||this->GetPawn())
	{
		return;
	}


	std::vector<AActor*> SportActors;

	TArray<AActor*> sas;

	UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),sas);
	for (auto i:sas)
		SportActors.push_back(i);

	std::vector<FVector> SpawnPosList;

	std::transform(SportActors.begin(),SportActors.end(),std::back_inserter(SpawnPosList),[](const AActor* actor)->FVector{
		return actor->GetActorLocation();
	});


	bool bSuccess=false;
	FVector outLocation;
	
	TArray<FVector> in_param;
	for (auto i:SpawnPosList)
		in_param.Push(i);
	UGameFunctionLibrary::GetMiniMonsterPos(this,in_param,300,outLocation,AMonsterPawn::StaticClass(),bSuccess);
	if (bSuccess)
	{
		std::vector<std::tuple<AActor*,FVector>> result;

		std::transform(SportActors.begin(),SportActors.end(),std::back_inserter(result),[](AActor* actor){
			return std::make_tuple(actor,actor->GetActorLocation());
		});
		
		if (auto gameMode=this->GetWorld()->GetAuthGameMode())
		{
			auto [SportActor,Pos]=*std::find_if(result.begin(),result.end(),[&outLocation](const std::tuple<AActor*,FVector> &item)
			{
				if (std::get<1>(item)==outLocation)
				{
					return true;
				}
				return false;
			});
			
			auto playerPawn=this->GetWorld()->SpawnActor<APawn>(PlayerClass,Pos,FRotator::ZeroRotator);
			
			this->Possess(playerPawn);
			return ;
		}
	}
	return ;
}

void APlayerPawnController::AddScore(const int Offset)
{
	// 加分 操作PlayerState
	this->PlayerScore_Local.score+=Offset;

	// 同步分数到计分服务器
	this->AsyncScoreToServer(this->PlayerScore_Local);

	this->OnUpdatePlayerScore(this->PlayerScore_Local);
	
}

void APlayerPawnController::AddRank(const int Offset)
{
	this->PlayerScore_Local.rank+=Offset;
	
	this->AsyncScoreToServer(this->PlayerScore_Local);
	
	this->OnUpdatePlayerScore(this->PlayerScore_Local);

}

void APlayerPawnController::OnRep_Remote_PlayerScore()
{
	// 本地分数更新
	if (auto netPlayerState=Cast<ANetPlayerState>(this->PlayerState))
	{
		//this->PlayerScore_Local=netPlayerState->PlayerScore;
	}
}


void APlayerPawnController::GetRemoteScore(FPlayerScore& RemoteScore)
{
	RemoteScore=Remote_PlayerScore;
}



void APlayerPawnController::SyncScore_Implementation(const FPlayerScore& PlayerScore, const FString& in_account)
{

	TMap<FString, FString> Headers;
	FString Body=FString::Format(TEXT("{  \"score\": {0},  \"rank\": {1}  }"),{PlayerScore.score,PlayerScore.rank});
	Headers.Add("Content-Type","application/json");
	
	Headers.Add("account",in_account);
	auto request=UServerFunctionLibrary::send_request("POST",UServerConfig::GetUrl()+"/update_player_score",Headers,Body, 2);
	//request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	//{
	//	UE_LOG(LogTemp,Display,TEXT(" Success %s"),*Response->GetContentAsString());
	//});
	request->ProcessRequest();

}

void APlayerPawnController::AsyncScoreToServer_Implementation(const FPlayerScore& NewScore)
{
	// 将数值发送到服务器
	this->Remote_PlayerScore=NewScore;
	//UStoreServerSubsystem::GetCurrent(this)->AsyncScoreToServer(this,PlayerScore_Local);
}

void APlayerPawnController::GetScoreFromServer_Implementation()
{
	UStoreServerSubsystem::GetCurrent(this)->GetScoreFromController(this);
}


