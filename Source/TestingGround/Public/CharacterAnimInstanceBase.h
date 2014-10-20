// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGGROUND_API UCharacterAnimInstanceBase : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

	/** The PawnOwner of this AnimInstance */
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ACharacterBase* Character;

	/** The speed of the character in centimeters/sec */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float Speed;

	/**
	* The direction of the character in degrees in range [-180, 180].
	* 0 -> Forward
	* -90 -> Left
	* 90 -> Right
	* +-180 -> Backward
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	float Direction;

	/** Indicates if the character is aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsAiming;

	/** Indicates if the character is firing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsFiring;

	/** Indicates if the character is reloading */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bIsReloading;

	virtual void BlueprintUpdateAnimation(float DeltaTimeX) override;

	virtual void BlueprintInitializeAnimation() override;	
};
