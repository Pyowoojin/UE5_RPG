// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacters/MyCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/MyItem.h"
#include "HUD/EchoHUD.h"
#include "HUD/EchoOverlay.h"
#include "Component/AttributeComponent.h"
#include "Items/Weapon/MyWeapon.h"
#include "Animation/AnimMontage.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Fruits.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	this->bUseControllerRotationPitch = false;
	this->bUseControllerRotationYaw = false;
	this->bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	MySpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	MySpringArm->SetupAttachment(RootComponent);
	MySpringArm->TargetArmLength = 450.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	ViewCamera->SetupAttachment(MySpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

}

void AMyCharacter::Tick(float DeltaTime)
{
	if (Attributes && EchoOverlay) {
		Attributes->RegenStamina(DeltaTime);
		EchoOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(MyCharContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitializeEchoOverlay();
}

void AMyCharacter::InitializeEchoOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		AEchoHUD* EchoHUD = Cast<AEchoHUD>(PlayerController->GetHUD());
		if (EchoHUD) {
			EchoOverlay = EchoHUD->GetEchoOverlay();
			if (EchoOverlay && Attributes) {
				EchoOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				EchoOverlay->SetStaminaBarPercent(1.f);
				EchoOverlay->SetGold(0);
				EchoOverlay->SetSouls(0);
			}
		}
	}
}

void AMyCharacter::CharMove(const FInputActionValue& Value)
{
	if ((ActionState != EActionState::EAS_Unoccupied) || ActionState == EActionState::EAS_Dead) return;

	const FRotator ConRot = GetControlRotation();
	const FRotator YawRot(0.f, ConRot.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	const FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(Direction, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMyCharacter::CharLook(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

// E Key를 눌렀을 때 실행되는 함수
void AMyCharacter::EKeyPressed()
{
	// 오버랩된 무기를 가져옴
	AMyWeapon* OverlappingWeapon = Cast<AMyWeapon>(OverlappingItem);

	// 오버랩된 무기가 있다면
	if (OverlappingWeapon) {
		// 장착한다.
		if (EquippedWeapon) {
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else {
		if (CanDisArm()) {
			Disarm();
		}
		else if (CanArm()) {
			Arm();
		}
	}
}

void AMyCharacter::EquipWeapon(AMyWeapon* Weapon)
{
	// 오른손 소켓에 장착함
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	// 캐릭터 상태를 한 손 무기 장착 상태로 바꿈
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	// overlapping Item 초기화
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

// 공격 함수. 애니메이션 몽타주를 실행하고 현재 상태를 변경
void AMyCharacter::Attack()
{
	Super::Attack();
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

// 애니메이션 몽타주 재생 함수. 해당 섹션으로 점프 뜀
void AMyCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

// 공격이 끝날때 호출하는 함수 -> 상태를 바꿔줌
void AMyCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

// 공격 가능 ? 다른 모션 X, 무기가 있다면,
bool AMyCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

// 무장 해제 가능? 다른 모션 X, 무장 상태라면,
bool AMyCharacter::CanDisArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

// 무장 가능 ? 다른 모션 X, 무장 해제 상태라면, 장착 중인 무기가 있다면,
bool AMyCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void AMyCharacter::Disarm()
{
	PlayEquipMontage(FName("UnEquip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMyCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMyCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();

}

// 무장 해제 함수. 장착중인 무기가 있으면, 등 뒤 소켓에 붙인다.
void AMyCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

// 재무장 함수, 무기가 있다면, 손에 붙인다.
void AMyCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AMyCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(CharMovementAction, ETriggerEvent::Triggered, this, &AMyCharacter::CharMove);
		EnhancedInputComponent->BindAction(CharLookAction, ETriggerEvent::Triggered, this, &AMyCharacter::CharLook);
		EnhancedInputComponent->BindAction(CharJumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(CharEquip, ETriggerEvent::Started, this, &AMyCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(CharAttack, ETriggerEvent::Triggered, this, &AMyCharacter::Attack);
		EnhancedInputComponent->BindAction(Dodge, ETriggerEvent::Triggered, this, &AMyCharacter::DodgeInput);
	}
}

void AMyCharacter::DodgeInput()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && EchoOverlay) {
		Attributes->UseStamina(Attributes->GetDodgeCost());
		EchoOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

bool AMyCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AMyCharacter::Jump()
{
	if (IsUnoccupied())
		Super::Jump();
}

void AMyCharacter::SetOverlappingItem(AMyItem* Item)
{
	OverlappingItem = Item;
}

void AMyCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && EchoOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		EchoOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AMyCharacter::AddGold(ATreasure* Gold)
{
	if (Attributes && EchoOverlay) {
		Attributes->AddGold(Gold->GetGold());
		EchoOverlay->SetGold(Attributes->GetGold());
	}
}

void AMyCharacter::EatFood(AFruits* Fruits)
{
	if (Attributes && EchoOverlay) {
		UE_LOG(LogTemp, Warning, TEXT("GRRR"));
		Attributes->ReceiveDamage(-(Fruits->GetFruitsEffect()));
		SetHUDHealth();
		if (!IsAlive()) {
			ActionState = EActionState::EAS_Dead;
		}
	}
}

bool AMyCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AMyCharacter::SetWeaponCollisionDisable()
{

}

void AMyCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f) {
		ActionState = EActionState::EAS_HitReaction;
	}
}

bool AMyCharacter::HasEnoughStamina()
{
	return Attributes && (Attributes->GetStamina() > Attributes->GetDodgeCost());
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void AMyCharacter::SetHUDHealth()
{
	if (EchoOverlay && Attributes) {
		EchoOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}
