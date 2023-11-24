// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Pawn/NetBasePawn.h"

#include "Net/UnrealNetwork.h"


// Sets default values
ANetBasePawn::ANetBasePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloatingPawnMovement=CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
	BoxComponent=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent=BoxComponent;

}

// Called when the game starts or when spawned
void ANetBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ANetBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANetBasePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetBasePawn, new_location);
	//DOREPLIFETIME_CONDITION(ANetBasePawn, new_location,COND_InitialOrOwner);
}

void ANetBasePawn::UpdateMove()
{
	this->SetActorLocation(FMath::Lerp(this->GetActorLocation(),new_location,this->lerp_speed));
}

void ANetBasePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ANetBasePawn::SendCurrentPos_Implementation(const FVector& pos)
{
	// 发送坐标到服务器
	this->new_location=pos;
	// 同时设置自己的坐标 @TODO:这里如何编写移动
	this->SetActorLocation(pos);
}

