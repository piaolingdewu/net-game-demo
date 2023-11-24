// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/AccountRegisterNode.h"

UAccountRegisterNode* UAccountRegisterNode::TryRegisterServer(UObject* WorldContext, const FString& in_account,
                                                              const FString& passwd, ALogin_Controller* pc, float timeout)
{
	auto obj=NewObject<UAccountRegisterNode>(WorldContext);

	obj->account=in_account;
	obj->password=passwd;
	obj->controller=pc;
	obj->request_timeout=timeout;
	
	obj->start();

	return obj;
}

void UAccountRegisterNode::OnCallbackRegisterResult(const ERegisterResultType& InLoginResult, const FString& Msg)
{
	// 查看注册结果
	OnAccountRegister.Broadcast(InLoginResult,Msg);
	
	this->SetReadyToDestroy();
}

void UAccountRegisterNode::start()
{
	// 尝试注册
	if (controller)
	{
		controller->OnRegisterResult.AddDynamic(this,&UAccountRegisterNode::OnCallbackRegisterResult);
		controller->Try_Register(account,password);
	}
}

void UAccountRegisterNode::OnTimeOut()
{
	// 超时了
	OnCallbackRegisterResult(ERegisterResultType::Fail_None,"Timeout");
}
