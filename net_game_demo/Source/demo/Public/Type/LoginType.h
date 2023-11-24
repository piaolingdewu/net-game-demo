// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoginType.generated.h"
/**
 * 
 */


UENUM(BlueprintType)
enum class ELoginResultType : uint8
{
	// 登录成功
	LoginSuccess,
	// 登录失败_密码错误
	LoginFail_PasswdOrAccountError,
	// 登陆错误_账号不存在
	LoginFail_AccountNotFound,
	// 登录失败_登陆服务器丢失
	LoginFail_LoginServerLost,
	// 未知错误
	None,
};

UENUM(BlueprintType)
enum class ERegisterResultType : uint8
{
	// 注册成功
	RegisterSuccess,
	// 重复账号 注册失败
	RegisterFail_Exists,
	// 未知错误
	Fail_None,
};

USTRUCT(BlueprintType)
struct FGameMapConnectInfo
{
	GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadWrite)
	FString Server_Name;
	UPROPERTY(BlueprintReadWrite)
	FString Server_Port;
	UPROPERTY(BlueprintReadWrite)
	int MaxPlayer=0;
	UPROPERTY(BlueprintReadWrite)
	int CurrentPlayer=0;
	//延迟
	UPROPERTY(BlueprintReadWrite)
	int ping=0;
};


