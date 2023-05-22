// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MyItem.h"
#include "FromTheScratch/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyItem::AMyItem() : Amplitude(0.25f), TimeConstant(5.f)
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	MySphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	MySphere->SetupAttachment(RootComponent);

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyItem::BeginPlay()
{
	Super::BeginPlay();

	MySphere->OnComponentBeginOverlap.AddDynamic(this, &AMyItem::OnSphereOverlap);
	MySphere->OnComponentEndOverlap.AddDynamic(this, &AMyItem::OnSphereEndOverlapped);
}

float AMyItem::TransformSin()
{
	return Amplitude* FMath::Sin(RunningTime * TimeConstant);
}

float AMyItem::TransformCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AMyItem::SpawnPickupSystem()
{
	if (PickupEffect) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation()
		);
	}
}

void AMyItem::SpawnPickupSound()
{
	if (PickupSound) {
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AMyItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface) {
		PickupInterface->SetOverlappingItem(this);
	}
}

void AMyItem::OnSphereEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface) {
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	
	const float z = TransformSin();

	if (ItemState == EItemState::EIS_Hovering) {
		AddActorWorldOffset(FVector(0.f, 0.f, z));
	}
}

