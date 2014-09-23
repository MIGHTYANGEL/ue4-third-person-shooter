

#include "TestingGround.h"
#include "MainCharacterAnimInstance.h"
#include "MainCharacter.h"


UMainCharacterAnimInstance::UMainCharacterAnimInstance(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	this->Speed = 0.0f;
	this->Direction = 0.0f;
	this->bIsAiming = false;
}

void UMainCharacterAnimInstance::BlueprintUpdateAnimation(float DeltaTimeX)
{
	Super::BlueprintUpdateAnimation(DeltaTimeX);

	APawn* PawnOwner = this->TryGetPawnOwner();

	if (PawnOwner != NULL)
	{
		const FVector CharacterVelocity = PawnOwner->GetVelocity();
		const FRotator CharacterRotation = PawnOwner->GetActorRotation();

		this->Speed = CharacterVelocity.Size();
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString(TEXT("Speed: ")) + FString::SanitizeFloat(this->Speed));

		this->Direction = this->CalculateDirection(CharacterVelocity, CharacterRotation);
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Green, FString(TEXT("Direction: ")) + FString::SanitizeFloat(this->Direction));

		AMainCharacter* MainCharacter = Cast<AMainCharacter>(PawnOwner);
		if (MainCharacter != NULL)
		{
			this->bIsAiming = MainCharacter->bIsAiming;

			GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, MainCharacter->bIsAiming ? FString(TEXT("IsAiming: True")) : FString(TEXT("IsAiming: False")));
			GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Yellow, MainCharacter->bIsSprinting ? FString(TEXT("IsSprinting: True")) : FString(TEXT("IsSprinting: False")));
		}
	}
}
