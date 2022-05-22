// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "tutorialgameCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UCharacterMovementComponent;
//class UtutorialPlayerHUD;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class AtutorialgameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;


public:
	AtutorialgameCharacter();

	// HUD stuff 
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UtutorialgamePlayerHUD> PlayerHUDClass;

	UPROPERTY()
		class UtutorialgamePlayerHUD* PlayerHUD;

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;
protected:
	
	/** Fires a projectile. */
	void OnPrimaryAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

public:
	//misc functions
	UFUNCTION()
		bool Switch(bool Val);
	UFUNCTION()
		void JumpSwitch();

	UFUNCTION()
		void MultiplyMaxSpeed(float Val);
	UFUNCTION()
		void DivideMaxSpeed(float Val);

	/*variables for player stats*/
	float HealthPoints;
	float StaminaPoints;
	/*max stats variables*/
	float MaxStamina;
	/*updates StaminaPoints*/

	UFUNCTION()
		void CharacterJump();

	UFUNCTION()
		void CharacterStopJump();

	void StaminaUpdate(float Val, float Max);
	//void StaminaRegen(float Val, float Max);
	/*passive stat regeneration*/
	void PassiveRegen();
	//movement funvtions
	void ChangeSpeed(float Val);
	void SprintStart();
	void SprintStop();
	void WalkStart();
	void WalkStop();
	// HUD functions
	void UpdateStaminaBar();

	// movement check data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MovementInput;
	UFUNCTION(BlueprintCallable)
		bool AIsMoving();		

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovementVariables")
		bool IsRunning;
	UFUNCTION(BlueprintCallable)
		bool AIsRunning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovementVariables")
		bool IsSprinting;
	UFUNCTION(BlueprintCallable)
		bool AIsSprinting();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovementVariables")
		bool IsWalking;
	UFUNCTION(BlueprintCallable)
		bool AIsWalking();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovementVariables")
		bool IsSideStepping;
	UFUNCTION(BlueprintCallable)
		bool AIsSideStepping();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovementVariables")
		bool IsJumping;
	UFUNCTION(BlueprintCallable)
		bool AIsJumping();

	UFUNCTION(BlueprintCallable)
		bool AIsFalling();
	//UFUNCTION(BlueprintCallable)
		//bool JumpCheck();

	bool SideStepWalk;
	bool MovingBackward;
	//debug stuff

	bool InfiniteStamina;
	//delta time
	double ADeltaTime;
	//double CDeltaTime();

	//timer handles / delegates
	FTimerHandle RegenTimerHandle;
	FTimerHandle JumpDelayHandle;
	FTimerHandle JumpSpeedDelayHandle;
	FTimerDelegate JumpSpeedDelayDelegate;
	FTimerHandle WhileJumpingHandle;
	FTimerDelegate WhileJumpingDelegate;

};

