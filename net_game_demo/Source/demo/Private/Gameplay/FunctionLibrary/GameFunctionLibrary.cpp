// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/GameFunctionLibrary.h"

#include "EngineUtils.h"
#include "GameActors/Controller/Login_Controller.h"
#include "GameActors/Pawn/GamePawn/PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AActor* UGameFunctionLibrary::GetNearestActor(UObject* WorldContextObject, FVector Location, float Radius, TArray<TSubclassOf<AActor>> ActorsType)
{
	AActor* OutActors=nullptr;
	
	if (WorldContextObject&&WorldContextObject->GetWorld())
	{
		UWorld * world=WorldContextObject->GetWorld();
		for (TActorIterator<AActor> iter(world); iter; ++iter)
		{
			if (FVector::Dist(iter->GetActorLocation(),Location) <= Radius &&ActorsType.Contains(iter->GetClass()))
			{
				if (OutActors==nullptr)
				{
					OutActors = *iter;
					continue;
				}
				if (FVector::Dist(OutActors->GetActorLocation(),Location)>(FVector::Dist(iter->GetActorLocation(),Location)))
				{
					OutActors = *iter;
				}
			}
		}
	}
	return OutActors;
}

void UGameFunctionLibrary::GetNearestActors(UObject* WorldContextObject, FVector Location, float Radius,
                                            TArray<AActor*>& OutActors, TArray<TSubclassOf<AActor>> AActorsType)
{
	if (WorldContextObject)
	{
		if (UWorld * world=WorldContextObject->GetWorld())
		{
			for (TActorIterator<AActor> iter(world); iter; ++iter)
			{
				if (FVector::Dist(iter->GetActorLocation(),Location) <= Radius&&AActorsType.Contains(iter->GetClass()))
				{
					OutActors.Add(*iter);
				}
			}
		}
	}
}

APlayerPawn* UGameFunctionLibrary::GetNearestPlayer(UObject* WorldContextObject, FVector Location, float Radius)
{
	if (WorldContextObject&&WorldContextObject->GetWorld())
	{
		APlayerPawn* PlayerPawn=nullptr;
		
		UWorld* world=WorldContextObject->GetWorld();
		for (TActorIterator<APlayerPawn> iter(world); iter; ++iter)
		{
			if (FVector::Dist(iter->GetActorLocation(),Location) <= Radius)
			{
				if (PlayerPawn==nullptr)
				{
					PlayerPawn = *iter;
					continue;
				}
				if (FVector::Dist(PlayerPawn->GetActorLocation(),Location)>(FVector::Dist(iter->GetActorLocation(),Location)))
				{
					PlayerPawn = *iter;
				}
			}
		}
		return PlayerPawn;
	}
	return nullptr;
}

void UGameFunctionLibrary::GetMiniMonsterPos(UObject* WorldContext, const TArray<FVector>& PosList, float Radius, FVector& outLocation, TSubclassOf<AActor>
                                             MonsterClass, bool& bSuccess)
{
	if (WorldContext&&WorldContext->GetWorld()&&PosList.Num()>2)
	{
		std::vector<std::tuple<FVector,TArray<AActor*>>> PosPair;
		//std::vector<std::tuple<const FVector&,const TArray<AActor*>&>> PosPair; //使用引用 性能更好

		for (auto i:PosList)
		{
			TArray<AActor*> OutActors;
			UKismetSystemLibrary::SphereOverlapActors(WorldContext,i,Radius,{},MonsterClass,TArray<AActor*>(),OutActors);

			PosPair.push_back(std::make_tuple(i,OutActors));
		}

		// 查看指定密度下 最小
		auto result=std::min_element(PosPair.begin(),PosPair.end(),[](const std::tuple<FVector,TArray<AActor*>>& a,const std::tuple<FVector,TArray<AActor*>>& b)
		{
			// a<b
			// 去掉和坐标重叠的
			const float PlayerSize=50;
			
			for (auto i:std::get<1>(a))
				if (FVector::Dist(i->GetActorLocation(),std::get<0>(a))<PlayerSize)
				{
					return false;
				}
			if (std::get<1>(a).Num()<std::get<1>(b).Num())
			{
				return true;
			}
			return false;
		});
		outLocation=std::get<0>(*result);
		bSuccess=true;
		return;
		
	}
	else
	{
		if (PosList.Num()>1)
		{
			outLocation=PosList[0];
		}
	}
	bSuccess=false;
	
}

void UGameFunctionLibrary::GetAllPlayerName(UObject* WorldContext,TArray<FString>& outPlayers)
{
	int player_num=UGameplayStatics::GetNumPlayerControllers(WorldContext);

	for(int i=0;i<player_num;++i)
	{
		if (auto con=Cast<ALogin_Controller>(UGameplayStatics::GetPlayerController(WorldContext,i)))
		{
			outPlayers.Push(con->GetAccount());
		}
	}
}

