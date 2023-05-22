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

// HIT 되었나 확인
void AMyWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	// 히트된 오브젝트를 불러옴
	if (BoxHit.GetActor()) {

		if (ActorIsSameType(BoxHit.GetActor())) return;

		// 데미지 적용부
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
	// 인터페이스로 형변환
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	// Hit 된 Obj에서 GetHit함수 호출
	if (HitInterface) {
		// HitInterface->GetHit(BoxHit.ImpactPoint);
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AMyWeapon::BoxTrace(FHitResult& BoxHit)
{
	// Start ~ End까지 칼의 범위를 그림
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
	// 두번 피격 당하지 않도록 IgnoreActor에 현재 엑터를 집어넣을 것임 Unique를 사용해 set처럼 사용
	IgnoreActors.AddUnique(BoxHit.GetActor());
}

// 무기 장착 함수
void AMyWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	// 소켓을 붙임
	AttachMeshToSocket(InParent, InSocketName);

	// 캐릭터 스테이트를 변경
	ItemState = EItemState::EIS_Equipped;

	// 소유자와 선동자를 무기를 집은 사람으로 변경함
	this->SetOwner(NewOwner);
	this->SetInstigator(NewInstigator);
	PlayEquipSound();
	DisableSphereCollision();
	DeactivateEmbers();
}

void AMyWeapon::DeactivateEmbers()
{
	// 이펙트 삭제
	if (ItemEffect) {
		ItemEffect->Deactivate();
	}
}

void AMyWeapon::DisableSphereCollision()
{
	// 콜리전 삭제
	if (MySphere) {
		MySphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMyWeapon::PlayEquipSound()
{
	// EquipSound 확인 후 소리 재생
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


