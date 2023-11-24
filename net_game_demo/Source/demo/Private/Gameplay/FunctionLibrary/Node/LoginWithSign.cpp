// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/FunctionLibrary/Node/LoginWithSign.h"

#include <chrono>

#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"

ULoginWithSign* ULoginWithSign::Try_LoginWithSign(UObject* WorldContext, const FString& Account,
                                                  const FString& Password)
{

	ensureMsgf(1,TEXT("TODO: 分数机制设置好了之后 就使用签名来登录和登出"));

	auto obj=NewObject<ULoginWithSign>(WorldContext);

	
	obj->account=Account;
	obj->password=Password;
	
	
	obj->start();

	return obj;
}

void ULoginWithSign::start()
{
	// 开始尝试登录
	//UServerFunctionLibrary::send_request()
	
	
}

FString ULoginWithSign::GetSign()
{
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	// 生成签名的方法 "{Account},{Password},{Time}"
	return FMD5::HashAnsiString(*FString::Format(TEXT("{0},{1},{2}"),{this->account,this->password,time}));
}
