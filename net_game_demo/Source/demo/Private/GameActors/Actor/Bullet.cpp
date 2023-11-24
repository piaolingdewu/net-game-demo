// Fill out your copyright notice in the Description page of Project Settings.

#include "GameActors/Actor/Bullet.h"
#include "GameActors/Pawn/GamePawn/MonsterPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bReplicates=true;

	BulletBoundComponent=CreateDefaultSubobject<USphereComponent>(TEXT("BulletBoundComponent"));
	ProjectileMovementComponent=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	RootComponent=BulletBoundComponent;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	
	if (this->HasAuthority()&&!BulletOwner)
	{
		// 初始化构造失败
		UKismetSystemLibrary::PrintString(this,TEXT("BulletOwner is null"),true,true,FLinearColor::Red,5.0f);
		this->Destroy();
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// 如果对方是Monster
	if (auto monster=Cast<AMonsterPawn>(OtherActor))
	{
		if (BulletOwner)
		{
			monster->AddBulletDamage(this);
			// 发起事件
			
			if (this->bDestroyOnHit)
			{
				this->Destroy();
			}
		}
	}
}


ABullet* ABullet::SpawnBullet(APlayerPawn* InOwner, TSubclassOf<ABullet> BulletClass, FVector Location,
                              FRotator Rotation)
{
	if (InOwner&&InOwner->GetWorld())
	{
		FTransform trans;
		trans.SetLocation(Location);
		trans.SetRotation(Rotation.Quaternion());
		
		
		auto bullet=Cast<ABullet>(UGameplayStatics::BeginDeferredActorSpawnFromClass(InOwner,BulletClass,trans,ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

		bullet->BulletOwner=InOwner;
		bullet->SetInstigator(InOwner);

		UGameplayStatics::FinishSpawningActor(bullet,trans);
		
		return bullet;
	}
	return nullptr;
}

void ABullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABullet,BulletOwner);
	
}

	


