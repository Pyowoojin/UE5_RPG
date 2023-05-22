// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MyItem.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHESCRATCH_API ASoul : public AMyItem
{
	GENERATED_BODY()
protected :
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void BeginPlay() override;
private :
	
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;

	float DesiredZ;

	UPROPERTY(EditAnywhere)
	double DriftRate = -15.f;

public :
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
	virtual void Tick(float DeltaTime) override;

};
