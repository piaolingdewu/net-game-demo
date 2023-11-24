// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncNodeWithTimeOut.h"
#include "LoginWithSign.generated.h"

/**
 * 借助账号的签名来登录 TODO: 分数机制设置好了之后 就使用签名来登录和登出
 */
UCLASS()
class DEMO_API ULoginWithSign : public UAsyncNodeWithTimeOut
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable,meta=(DeprecatedFunction))
	[[deprecated]] static ULoginWithSign* Try_LoginWithSign(UObject* WorldContext,const FString& Account,const FString& Password);
public:
	void start();
private:
	// 获取签名
	FString GetSign();
	
	FString password;
	FString account;
};
