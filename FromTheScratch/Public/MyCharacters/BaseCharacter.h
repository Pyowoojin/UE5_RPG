// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "MyCharacters/CharacterType.h"
#include "BaseCharacter.generated.h"

class AMyWeapon;
class UAnimMontage;
class UAttributeComponent;

UCLASS()
class FROMTHESCRATCH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual bool CanAttack();

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	bool IsAlive();
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnable(ECollisionEnabled::Type CollisionEnabled);
	void DisableMeshCollision();
	
	/*
	몽타주 애니메이션 재생
	*/
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void DisableCapsule();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AMyWeapon* EquippedWeapon;

	/*
	* 컴포넌트
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	/*
	* Animation Motages -> 블루프린트에서 매핑시켜주면 됨
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;

	/*
	* Animation Motages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DeathReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray <FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray <FName> DeathMontageSections;

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);

public :
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
