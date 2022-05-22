// Copyright Epic Games, Inc. All Rights Reserved.

#include "tutorialgameCharacter.h"
#include "tutorialgameProjectile.h"
#include "tutorialgamePlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/Pawn.h"
#include "Misc/App.h"


//////////////////////////////////////////////////////////////////////////
// AtutorialgameCharacter

AtutorialgameCharacter::AtutorialgameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 40.f;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	
	Mesh1P->SetupAttachment(GetCapsuleComponent());
	Mesh1P->bCastDynamicShadow = true;
	Mesh1P->CastShadow = true; 
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(Mesh1P, "head");
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	//FirstPersonCameraComponent->AttachToComponent(Mesh1P,)


	//HUD
	PlayerHUDClass = nullptr;

	//sets max stats
	PlayerHUD = nullptr;

	//default state
	IsJumping = false;
	MaxStamina = 150.f;
	
	// set stats base values
	HealthPoints = 100.f;
	StaminaPoints = MaxStamina;
	MovementInput = 0.f;	

	//debug stuff
	InfiniteStamina = true;
	// enable ticking
	PrimaryActorTick.bCanEverTick = true;
	
	//sets deltatime to mroe usable call -- 
	ADeltaTime = FApp::GetDeltaTime();
}

void AtutorialgameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//HUD
	
	// init HUD object StaminaBar
	if (IsLocallyControlled() && PlayerHUDClass) {
		APlayerController* playerChar = GetController<APlayerController>();
		check(playerChar);
		PlayerHUD = CreateWidget<UtutorialgamePlayerHUD>(playerChar, PlayerHUDClass);
		check(PlayerHUD)
		PlayerHUD->AddToPlayerScreen();
		PlayerHUD->SetStamina(StaminaPoints, MaxStamina);
	}
	
	// sets up a timer that handles passive regenration
	GetWorldTimerManager().SetTimer(RegenTimerHandle, this, &AtutorialgameCharacter::PassiveRegen, 0.01f, true, 0.01f);

	// initializes player speed
	GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed*0.7);
}

void AtutorialgameCharacter::Tick(float DeltaTime) {
	
	Super::Tick(DeltaTime);

	UpdateStaminaBar();

	//PlayerHUD->SetStamina(StaminaPoints, MaxStamina);

}


//////////////////////////////////////////////////////////////////////////// Input

void AtutorialgameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AtutorialgameCharacter::CharacterJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AtutorialgameCharacter::CharacterStopJump);

	// Bind sprint functions
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AtutorialgameCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AtutorialgameCharacter::SprintStop);

	//Bind walk functions
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AtutorialgameCharacter::WalkStart);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AtutorialgameCharacter::WalkStop);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AtutorialgameCharacter::OnPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AtutorialgameCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AtutorialgameCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AtutorialgameCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AtutorialgameCharacter::LookUpAtRate);
}

// Misc functions



bool AtutorialgameCharacter::Switch(bool Val) {
	return !(Val);
}

void  AtutorialgameCharacter::JumpSwitch() { IsJumping = false; }

// Movement functions

bool  AtutorialgameCharacter::AIsFalling() {
	if (GetCharacterMovement()->IsFalling()) { return true; }
	else { return false; }
}

void AtutorialgameCharacter::MultiplyMaxSpeed(float Val) {
	if (Val > 0) {
		GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed * Val);}
}

void AtutorialgameCharacter::DivideMaxSpeed(float Val) {
	if (Val > 0) {
		GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed / Val);
	}
}

bool AtutorialgameCharacter::AIsWalking() {
	if (IsWalking) { return true; }
	else { return false; }
}

bool AtutorialgameCharacter::AIsRunning() {
	if (IsRunning) { return true; }
	else { return false; }
}


bool AtutorialgameCharacter::AIsSprinting() {
	if (IsSprinting) { return true; }
	else { return false; }
}


bool AtutorialgameCharacter::AIsSideStepping() {
	if (IsSideStepping) { return true; }
	else { return false; }
}

bool AtutorialgameCharacter::AIsMoving() {	
	if (MovementInput != 0.f) { return true; }
	else { return false; };		
}

bool AtutorialgameCharacter::AIsJumping() {
	if (IsJumping) { return true; }
	else { return false; }
}

void AtutorialgameCharacter::StaminaUpdate(float Value, float Max) {
	
	if (StaminaPoints <= Max) { StaminaPoints -= fabsf(Value); }
	else if (StaminaPoints > Max) { StaminaPoints = Max; }
}

void AtutorialgameCharacter::UpdateStaminaBar() {

	PlayerHUD->SetStamina(StaminaPoints, MaxStamina);

}

//passive regenration handles stamina regen based on movement state (running, walking or sprinting)
void AtutorialgameCharacter::PassiveRegen() {
	
	if (InfiniteStamina == true) { StaminaPoints = MaxStamina; }

	//Running regen state
	if (MovementInput != 0.f && IsRunning) {
		StaminaUpdate(0.2f, MaxStamina);HealthPoints += 0.1f;}

	else if (MovementInput != 0.f && IsSideStepping && IsRunning) {
		StaminaUpdate(0.2f, MaxStamina);HealthPoints += 0.1f;}

	//Sprinting regen state
	if (MovementInput != 0.f && IsSprinting) {
		StaminaUpdate(0.2f, MaxStamina);HealthPoints += 0.1f;}

	else if (MovementInput != 0.f && IsSideStepping && IsSprinting) {
		StaminaUpdate(0.2f, MaxStamina);HealthPoints += 0.1f;}
	
	//Walking reen state
	if (MovementInput != 0.f && IsWalking) { 
		StaminaPoints += (0.5f);HealthPoints += 0.2f; }

	else if (MovementInput != 0.f && IsSideStepping && IsWalking) {
		StaminaPoints += 0.6f;HealthPoints += 0.2f;}


	if (MovementInput == 0.f && StaminaPoints < MaxStamina) {
		StaminaPoints += 0.75f;HealthPoints += 0.1f; }

}

void AtutorialgameCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AtutorialgameCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AtutorialgameCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AtutorialgameCharacter::MoveForward(float Value)
{
	if (Value != 0.0f) {

		if (Value < 0.0) {
			MovingBackward;
			// add movement in that direction
			if (!IsSideStepping) { AddMovementInput(GetActorForwardVector(), (Value * 0.55f)); }
			else if (IsSideStepping) { AddMovementInput(GetActorForwardVector(), (Value * 0.35f)); }
			// update stamina points
			StaminaUpdate((Value * 0.3) * ADeltaTime, MaxStamina);

			MovementInput = Value;
			IsRunning = true;
		}
		else if (Value > 0.33) {
			// add movement in that direction
			if (!IsSideStepping) { AddMovementInput(GetActorForwardVector(), (Value * 0.76f)); }
			else if (IsSideStepping) { AddMovementInput(GetActorForwardVector(), (Value * 0.56f)); }
			// update stamina points
			StaminaUpdate((Value * 0.3) * ADeltaTime, MaxStamina);

			MovementInput = Value;
			IsRunning = true;
		}
	}
	else { IsRunning = false; MovementInput = 0.0; }
}

void AtutorialgameCharacter::MoveRight(float Value)
{
	if (Value != 0.0f) {
		MovementInput = Value;
		IsSideStepping = true;
		if (StaminaPoints > 0.f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), (Value * 0.7f));
			// update stamina points
			StaminaUpdate(Value * ADeltaTime,MaxStamina);
		}
		else
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), (Value / 3));
			// update stamina points
			StaminaUpdate(Value * ADeltaTime,MaxStamina);
		}
	} 
	else { IsSideStepping = false; MovementInput = 0.0; }
}

void AtutorialgameCharacter::CharacterJump() {

	if (!IsJumping) {
		
		if (!IsRunning && !IsSideStepping && !IsSprinting) {

			IsJumping = true;
			//ACharacter::Jump();			
						
			float TFloat = 0.05;
			JumpSpeedDelayDelegate.BindUFunction(this, FName("DivideMaxSpeed"), TFloat);

			MultiplyMaxSpeed(0.05f);
			GetWorldTimerManager().SetTimer(JumpDelayHandle, this, &AtutorialgameCharacter::ACharacter::Jump, 0.0f, false, 1.2f);
			//DivideMaxSpeed(0.05f);
			GetWorldTimerManager().SetTimer(JumpSpeedDelayHandle, JumpSpeedDelayDelegate, 0.01f, false, 1.3f);
			
			WhileJumpingDelegate.BindUFunction(this, FName("JumpSwitch"));
			GetWorldTimerManager().SetTimer(WhileJumpingHandle, WhileJumpingDelegate, 0.1f, false);
			StaminaUpdate(15.f, MaxStamina);
			//IsJumping = false;

		}
		else if (IsRunning && !IsJumping)
		{
			IsJumping = true;
			ACharacter::Jump();
			StaminaUpdate(15.f, MaxStamina);
			WhileJumpingDelegate.BindUFunction(this, FName("JumpSwitch"));
			GetWorldTimerManager().SetTimer(WhileJumpingHandle, WhileJumpingDelegate, 0.1f, false);
			//IsJumping = false;
			
		}

		StaminaUpdate(15.f, MaxStamina);
	}
	else { IsJumping = false; }
}

void AtutorialgameCharacter::CharacterStopJump() {

	IsJumping = false;

	//GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed / 0.05);
	//ACharacter::Jump();
	//ACharacter::StopJumping();

	//StaminaUpdate(15.f, MaxStamina);
}

void AtutorialgameCharacter::SprintStart() {

	if (!IsWalking) {
		GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed * 1.4);
	}

	IsSprinting = true;
	IsWalking = false;
}

void AtutorialgameCharacter::SprintStop() {

	if (!IsWalking) {
		GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed / 1.4);
	}
	IsSprinting = false;
}

void AtutorialgameCharacter::WalkStart() {

	IsWalking = true;
	if (!IsSprinting) {
		if (IsSideStepping) {
			GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed * 0.2);
			SideStepWalk = true;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed * 0.4);
			IsSideStepping = false;
		}
	}	
	IsRunning = false;
	IsSprinting = false;
}

void AtutorialgameCharacter::WalkStop() {

	IsWalking = false;
	if (!IsSprinting) {
		if (SideStepWalk) {
			GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed / 0.2);
			//reset sidestepwalk switch
			SideStepWalk = false;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = (GetCharacterMovement()->MaxWalkSpeed / 0.4);
			IsSideStepping = false;
		}
	}

	IsWalking = false;
}

void AtutorialgameCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AtutorialgameCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

bool AtutorialgameCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AtutorialgameCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AtutorialgameCharacter::EndTouch);

		return true;
	}
	
	return false;
}

//double AtutorialgameCharacter::CDeltaTime() {


