// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacters/EchoAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyCharacters/MyCharacter.h"

void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	UE_LOG(LogTemp, Warning, TEXT("PROCESS"));

	EchoCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	if (EchoCharacter) {
		MyCharacterMovement = EchoCharacter->GetCharacterMovement();
	}
	
}

void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MyCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(MyCharacterMovement->Velocity);
		IsFalling = MyCharacterMovement->IsFalling();
		CharacterState = EchoCharacter->GetCharacterState();
		ActionState = EchoCharacter->GetActionState();
		DeathPose = EchoCharacter->GetDeathPose();
	}

}
