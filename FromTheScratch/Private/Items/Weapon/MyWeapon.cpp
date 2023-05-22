// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/MyWeapon.h"
#include "MyCharacters/MyCharacter.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"

AMyWeapon::AMyWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetBoxExtent(FVector(2.5f, 3.5f, 35.f));
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceStart->SetRelativeLocation(FVector(0.f, 0.f, -25.f));

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
	BoxTraceEnd->SetRelativeLocation(FVector(0.f, 0.f, 40.5f));

}

void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AMyWeapon::OnBoxOverlap);
}

// HIT �Ǿ��� Ȯ��
void AMyWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	// ��Ʈ�� ������Ʈ�� �ҷ���
	if (BoxHit.GetActor()) {

		if (ActorIsSameType(BoxHit.GetActor())) return;

		// ������ �����
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(),Damage,GetInstigatorController(),this,UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		
	}
}

bool AMyWeapon::ActorIsSameType(AActor* OtherActor)
{
	return (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) ||
		(GetOwner()->ActorHasTag(TEXT("EngageableTarget")) && OtherActor->ActorHasTag(TEXT("EngageableTarget")));
}

void AMyWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	// �������̽��� ����ȯ
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	// Hit �� Obj���� GetHit�Լ� ȣ��
	if (HitInterface) {
		// HitInterface->GetHit(BoxHit.ImpactPoint);
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AMyWeapon::BoxTrace(FHitResult& BoxHit)
{
	// Start ~ End���� Į�� ������ �׸�
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors) {
		ActorsToIgnore.Add(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);
	// �ι� �ǰ� ������ �ʵ��� IgnoreActor�� ���� ���͸� ������� ���� Unique�� ����� setó�� ���
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

// ���� ���� �Լ�
void AMyWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	// ������ ����
	AttachMeshToSocket(InParent, InSocketName);

	// ĳ���� ������Ʈ�� ����
	ItemState = EItemState::EIS_Equipped;

	// �����ڿ� �����ڸ� ���⸦ ���� ������� ������
	this->SetOwner(NewOwner);
	this->SetInstigator(NewInstigator);
	PlayEquipSound();
	DisableSphereCollision();
	DeactivateEmbers();
}

void AMyWeapon::DeactivateEmbers()
{
	// ����Ʈ ����
	if (ItemEffect) {
		ItemEffect->Deactivate();
	}
}

void AMyWeapon::DisableSphereCollision()
{
	// �ݸ��� ����
	if (MySphere) {
		MySphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMyWeapon::PlayEquipSound()
{
	// EquipSound Ȯ�� �� �Ҹ� ���
	if (EquipSound) {
		UGameplayStatics::PlaySoundAtLocation(this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AMyWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}


