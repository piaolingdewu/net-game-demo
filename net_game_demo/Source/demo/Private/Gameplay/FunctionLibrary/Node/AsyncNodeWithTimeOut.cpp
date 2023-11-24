// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/AsyncNodeWithTimeOut.h"

TStatId UAsyncNodeWithTimeOut::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAsyncNodeWithTimeOut, STATGROUP_Tickables);
}

bool UAsyncNodeWithTimeOut::IsTickable() const
{
	return !IsTemplate()&&!bIsTimeout;
}

void UAsyncNodeWithTimeOut::Tick(float DeltaTime)
{
	if (request_timeout<=0&&!bIsTimeout)
	{
		this->OnTimeOut();
		this->bIsTimeout=true;
	}
}

