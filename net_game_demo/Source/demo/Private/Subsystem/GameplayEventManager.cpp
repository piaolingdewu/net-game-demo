// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameplayEventManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UGameplayEventManager::SendGameplayEvent(UObject* WorldContext, FGameplayTag EventTag, const FGameplayEventData& Payload)
{
	if (WorldContext&&WorldContext->GetWorld())
	{
		//UKismetSystemLibrary::PrintString(WorldContext->GetWorld(),FString::Format(TEXT("Spawn {0}:"),{EventTag.ToString()}));
		WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UGameplayEventManager>()->OnGameplayEventReceive.Broadcast(EventTag,Payload);
	}
	else
	{
		ensureMsgf(1,TEXT("UGameplayEventManager::SendGameplayEvent() WorldContext is nullptr"));
	}
}

UGameplayEventManager* UGameplayEventManager::GetCurrent(UObject* WorldContext)
{
	ensureMsgf(IsValid(WorldContext),TEXT("UGameplayEventManager::GetCurrent() WorldContext is nullptr"));
	return UGameplayStatics::GetGameInstance(WorldContext)->GetSubsystem<UGameplayEventManager>();
}
