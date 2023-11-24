#include "Gameplay/FunctionLibrary/ServerFunctionLibrary.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(DateTests, "net_game_demo.demo.Public.LocalTests.DateTests",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool DateTests::RunTest(const FString& Parameters)
{
	// Make the test pass by returning true, or fail by returning false.


	//auto world=UWorld::CreateWorld(EWorldType::Game, false);


	auto time=UServerFunctionLibrary::GetUTCMilliSecond();

	UE_LOG(LogTemp,Display,TEXT("Time:%lld"),time);

	auto f1=[]()->std::tuple<FString,int64>
	{
		auto time=UServerFunctionLibrary::GetUTCMilliSecond();
		return {"GetUTCMilliSecond",time};
	};



	
	return true;
}
