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
	// 현재 체력
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	// 맥시멈
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	// 현재 스태미나
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stamina;
	// 맥시멈
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
	// 처음 최대 체력과 현재 체력을 정함
	void SetInitHealth(float Value);
	// 체력 Getter Func
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
