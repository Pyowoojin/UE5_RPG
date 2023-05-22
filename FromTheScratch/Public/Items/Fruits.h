// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/MyItem.h"
#include "Fruits.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHESCRATCH_API AFruits : public AMyItem
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private :
	UPROPERTY(EditAnywhere, Category = Fruits)
	float Fluctuation = 0;

public :
	FORCEINLINE float GetFruitsEffect() const { return Fluctuation; }
};
