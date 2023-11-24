#include "GameLevelConfig.h"
#include "ServerConfig.h"
#include "Engine/AssetManager.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(LoadTest, "net_game_demo.demo.Public.Config.LoadTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool LoadTest::RunTest(const FString& Parameters)
{

	auto primaryId=FPrimaryAssetId("Config");
	// 通过Primaryid载入
	auto load_asset=UAssetManager::Get().LoadPrimaryAssetsWithType("Config");

	if (load_asset!=nullptr)
	{
		TArray<UObject*> LoadAssets;
		load_asset->WaitUntilComplete();
		load_asset.Get()->GetLoadedAssets(LoadAssets);
		int count=0;
		int was_load_count=0;
		load_asset->GetLoadedCount(was_load_count,count);
		
		UE_LOG(LogTemp,Display,TEXT("ConfigCount:%d"),count);

		for (auto i:LoadAssets)
		{
			UE_LOG(LogTemp,Display,TEXT("Load:%s"),*i->GetFullName());

			if (auto serverConfig=Cast<UServerConfig>(i))
			{
				
			}
		}
		UE_LOG(LogTemp,Display,TEXT("LoadConfig Finish"));
	}
	UE_LOG(LogTemp,Display,TEXT("---------------------------------------------------------------"));

	auto LevelConfig=UGameLevelConfig::GetGameLevelConfig();
	auto ServiceConfig=UServerConfig::GetServerConfig();
	
	UE_LOG(LogTemp,Display,TEXT("%s"),*LevelConfig->GetFullName());
	UE_LOG(LogTemp,Display,TEXT("%s"),*ServiceConfig->GetFullName());

	
	
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
