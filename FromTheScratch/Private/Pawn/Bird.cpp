// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
// #include "InputMappingContext.h
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MyCapsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(Capsule);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
	ViewCamera->SetRelativeRotation(FRotator(0, -20, 0));

}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::MoveBird(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();

	if (Controller && (DirectionValue != 0.f)) {
		UE_LOG(LogTemp, Warning, TEXT("%f"), DirectionValue);
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue);
	}

}

void ABird::LookBird(const FInputActionValue& Value)
{
	const FVector2D TANDR = Value.Get<FVector2D>();
	if (GetController()) {
		AddControllerYawInput(TANDR.X);
		AddControllerPitchInput(TANDR.Y);
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// PlayerInputComponent->BindAction(FName("MoveBird"), this, &ABird::MoveBird);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::MoveBird);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::LookBird);
	}
}