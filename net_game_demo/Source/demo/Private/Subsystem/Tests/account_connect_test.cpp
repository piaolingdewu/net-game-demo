#include "HttpManager.h"
#include "HttpModule.h"
#include "Config/ServerConfig.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/AutomationTest.h"
#include "Subsystem/AccountSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(account_connect_test, "net_game_demo.demo.Private.Subsystem.account_connect_test",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool account_connect_test::RunTest(const FString& Parameters)
{
	// Make the test pass by returning true, or fail by returning false.

	// 单元测试

	// 尝试登录
	
	auto world=UWorld::CreateWorld(EWorldType::Game,false);

	auto gameInstance=NewObject<UGameInstance>(world,UGameInstance::StaticClass());

	if (!gameInstance)
	{
		UE_LOG(LogTemp,Display,TEXT("GameInstance is null"));
		return false;
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("GameInstance is work"));
	}


	// 获取账号子系统
	auto account=gameInstance->GetSubsystem<UAccountSubsystem>();

	// 当前使用的版本
	UE_LOG(LogTemp,Display,TEXT("%s"),*UServerConfig::GetUrl());
	
	auto request=account->try_login("aaa","a", 2);
	request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		if (Request->GetStatus()==EHttpRequestStatus::Succeeded)
		{
			TSharedRef<TJsonReader<>>reader=TJsonReaderFactory<>::Create(Response->GetContentAsString());
			TSharedPtr<FJsonObject>jsonObject=MakeShareable(new FJsonObject());
			FJsonSerializer::Deserialize(reader,jsonObject);
			
			UE_LOG(LogTemp,Display,TEXT("Account:%s"),*jsonObject->Values["account"]->AsString());
		}
	});
	
	request->ProcessRequest();
	// 等待
	while (request->GetStatus()==EHttpRequestStatus::Processing)
	{
		FPlatformProcess::Sleep(0.1);
		FHttpModule::Get().GetHttpManager().Tick(0.1);
		//request.Get().Tick(0.1);
	}

	// 验证账号登陆

	// 验证账号注册
	
	// 验证账号是否存在
	
	return true;
}
