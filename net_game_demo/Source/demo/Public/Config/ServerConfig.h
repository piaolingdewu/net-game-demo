// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ServerConfig.generated.h"


/**
 * 
 */

USTRUCT(BlueprintType,Blueprintable)
struct FHostRecord
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ServerName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ServerPort;
	
};
UCLASS(Blueprintable)
class DEMO_API UServerConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UServerConfig();

	// 当前使用的版本
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FName UserVersion;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TMap<FName,FHostRecord> server_record;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TMap<FName,FHostRecord> loginServer;

public: // 默认用于测试的账号
	// 默认账号
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FString DefaultAccount="aaa";

	// 默认密码
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FString DefaultPassword="a";

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
public:
	UFUNCTION(BlueprintPure)
	static UServerConfig* GetServerConfig();
	
	static FString GetUrl();

	
};
