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

// E Key�� ������ �� ����Ǵ� �Լ�
void AMyCharacter::EKeyPressed()
{
	// �������� ���⸦ ������
	AMyWeapon* OverlappingWeapon = Cast<AMyWeapon>(OverlappingItem);

	// �������� ���Ⱑ �ִٸ�
	if (OverlappingWeapon) {
		// �����Ѵ�.
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
	// ������ ���Ͽ� ������
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	// ĳ���� ���¸� �� �� ���� ���� ���·� �ٲ�
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	// overlapping Item �ʱ�ȭ
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

// ���� �Լ�. �ִϸ��̼� ��Ÿ�ָ� �����ϰ� ���� ���¸� ����
void AMyCharacter::Attack()
{
	Super::Attack();
	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

// �ִϸ��̼� ��Ÿ�� ��� �Լ�. �ش� �������� ���� ��
void AMyCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

// ������ ������ ȣ���ϴ� �Լ� -> ���¸� �ٲ���
void AMyCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

// ���� ���� ? �ٸ� ��� X, ���Ⱑ �ִٸ�,
bool AMyCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

// ���� ���� ����? �ٸ� ��� X, ���� ���¶��,
bool AMyCharacter::CanDisArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

// ���� ���� ? �ٸ� ��� X, ���� ���� ���¶��, ���� ���� ���Ⱑ �ִٸ�,
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

// ���� ���� �Լ�. �������� ���Ⱑ ������, �� �� ���Ͽ� ���δ�.
void AMyCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

// �繫�� �Լ�, ���Ⱑ �ִٸ�, �տ� ���δ�.
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
