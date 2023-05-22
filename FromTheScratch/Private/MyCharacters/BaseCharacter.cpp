#include "MyCharacters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Component/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Weapon/MyWeapon.h"
#include "MyCharacters/CharacterType.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("EnemyAttributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter) {
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die();
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) {
		CombatTarget = nullptr;
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName)
{
	if (SectionName.Num() <= 0) return -1;

	const int32 MaxSectionIndex = SectionName.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);

	PlayMontageSection(Montage, SectionName[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathReactMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX) {
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Start"));
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();
	
	const FVector CombatTargetLocaiton = CombatTarget->GetActorLocation();
	const FVector Location = this->GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocaiton).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;
	return CombatTargetLocaiton + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget) {
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	FVector Forward = GetActorForwardVector();

	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);

	FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	float cos = FVector::DotProduct(Forward, ToHit);
	float theta = FMath::Acos(cos);

	// 외적이 아래로 향하고 있으면. theta는 -
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0) {
		theta *= -1.f;
	}

	theta = FMath::RadiansToDegrees(theta);

	FName Section("FromBack");

	if (theta >= -45.f && theta < 45.f) {
		Section = FName("FromFront");
	}
	else if (theta < -45.f && theta >= -135.f) {
		Section = FName("FromLeft");
	}
	else if (theta >= 45.f && theta < 135.f) {
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
}
void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound) {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}
void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}
void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes) {
		// 데미지를 수신하고
		Attributes->ReceiveDamage(DamageAmount);
	}
}
void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage) {
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}
bool ABaseCharacter::CanAttack()
{
	return false;
}
void ABaseCharacter::Die_Implementation()
{
	Tags.Remove("EngageableTarget");
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}
bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}
void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}


void ABaseCharacter::SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

