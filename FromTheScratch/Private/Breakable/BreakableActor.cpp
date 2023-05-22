// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Treasure.h"
#include "Items/Fruits.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);

	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{ 
	Super::BeginPlay();
	
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return;

	bBroken = true;

	UWorld* World = GetWorld();

	int temp = ImpactPoint.X;
	int EvenOrOddNum = temp % 2;
	FVector Location = GetActorLocation();
	Location.Z += 75;

	if (World && TreasureClasses.Num() > 0 && EvenOrOddNum) {
		World->SpawnActor<ATreasure>(TreasureClasses[FMath::RandRange(0, TreasureClasses.Num() - 1)], Location, GetActorRotation());
	}
	else if (World && FruitsClassess.Num() > 0 && !EvenOrOddNum) {
		World->SpawnActor<AFruits>(FruitsClassess[FMath::RandRange(0, FruitsClassess.Num() - 1)], Location, GetActorRotation());
	}

	FTimerHandle Timer;
	float time = 5.f;
	GetWorld()->GetTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([&]()
		{
			UE_LOG(LogTemp, Log, TEXT("implement"));
			this->Destroy();
		}
	), time, false);
}

