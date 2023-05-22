// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHESCRATCH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;
private :
	// ���� ü��
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	// �ƽø�
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	// ���� ���¹̳�
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stamina;
	// �ƽø�
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 3.f;

public :
	// ó�� �ִ� ü�°� ���� ü���� ����
	void SetInitHealth(float Value);
	// ü�� Getter Func
	FORCEINLINE float GetHealth() { return Health; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();
};
