// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "Interfaces/PickupInterface.h"
#include "MyCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AMyItem;
class UAnimMontage;
class UEchoOverlay;
class ASoul;

UCLASS()
class FROMTHESCRATCH_API AMyCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void SetOverlappingItem(AMyItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Gold) override;
	virtual void EatFood(AFruits* Fruits) override;
	
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionDisable();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* MyCharContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CharMovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CharLookAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CharJumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CharEquip;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CharAttack;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* Dodge;

	/** Callback for Input Functions*/
	void CharMove(const FInputActionValue& Value);
	void CharLook(const FInputActionValue& Value);
	void EKeyPressed();
	void DodgeInput();

	/* Combat */
	void EquipWeapon(AMyWeapon* Weapon);
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual	bool CanAttack() override;
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisArm();
	bool CanArm();
	void Disarm();
	void Arm();
	virtual void DodgeEnd() override;
	virtual void Die_Implementation() override;
	bool IsOccupied();
	bool HasEnoughStamina();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

private:
	bool IsUnoccupied();
	void InitializeEchoOverlay();
	void SetHUDHealth();

	/* Character Component */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* MySpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AMyItem* OverlappingItem;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY()
	UEchoOverlay* EchoOverlay;
};
