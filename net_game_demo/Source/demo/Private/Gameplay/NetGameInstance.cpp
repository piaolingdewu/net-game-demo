// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/NetGameInstance.h"

#include "Config/ServerConfig.h"
#include "Kismet/KismetSystemLibrary.h"


EBrowseReturnVal::Type UNetGameInstance::TryConnectServer(const FString& Host, const FString& Port,
                                                          const FString& account, const FString& password)
{
	auto BaseURL=WorldContext->LastURL;
	
	auto newURL=FURL(&BaseURL,*FString::Format(TEXT("{0}:{1}"),{Host,Port}),TRAVEL_Absolute);
	// PlayerURL.AddOption(*FString::Printf(TEXT("Name=%s"), *PlayerName));

	FString err;
	
	//newURL.AddOption(*AddOptions);

	newURL.AddOption(*FString::Printf(TEXT("account=%s"),*account));
	
	newURL.AddOption(*FString::Printf(TEXT("password=%s"),*password));
	
	if (!err.IsEmpty())
	{
		UE_LOG(LogTemp,Display,TEXT("Browse err:%s"),*err);
	}

	// option
	//newURL.GetOption()
	//UE_LOG(LogTemp,Display,TEXT("Options:%s"),*newURL.ToString());
	UKismetSystemLibrary::PrintString(this,newURL.ToString(),true,true,FLinearColor::Red,5.f);
	
	return GEngine->Browse(*this->WorldContext,newURL,err);
}
