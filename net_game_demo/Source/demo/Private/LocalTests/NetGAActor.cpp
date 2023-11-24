// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalTests/NetGAActor.h"


// Sets default values
ANetGAActor::ANetGAActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent=CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilityComponent"));
	
}

// Called when the game starts or when spawned
void ANetGAActor::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

// Called every frame
void ANetGAActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UAbilitySystemComponent* ANetGAActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

