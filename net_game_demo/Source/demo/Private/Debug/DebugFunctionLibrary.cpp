// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/DebugFunctionLibrary.h"

FString UDebugFunctionLibrary::GetNetModeName(const UObject* WorldContext)
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
