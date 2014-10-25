// Fill out your copyright notice in the Description page of Project Settings.

#include "TestingGround.h"
#include "EnemyAIController.h"
#include "MainCharacter.h"


AEnemyAIController::AEnemyAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	this->FieldOfView = 150.0f; // The FOV of the enemy character in degrees
	this->Target = NULL;
	this->TargetLocation = FVector::ZeroVector;
	this->bShouldMoveToHomeLocation = true;
	this->DelayBeforeGoingToHomeLocation = 2.5f; // in seconds
	this->DelayBeforeGoingToHomeLocationCounter = 0.0f; // in seconds
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledPawn = this->GetPawn();
	if (ControlledPawn != NULL)
	{
		this->ControlledCharacter = Cast<AEnemyCharacter>(ControlledPawn);

		if (this->ControlledCharacter != NULL)
		{
			this->HomeLocation = this->ControlledCharacter->GetActorLocation();

			this->ControlledCharacter->AggroTrigger->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAIController::OnAggroTriggerBeginOverlap);
			this->ControlledCharacter->AggroTrigger->OnComponentEndOverlap.AddDynamic(this, &AEnemyAIController::OnAggroTriggerEndOverlap);
		}
	}
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->Target != NULL)
	{
		bool bIsTargetInLineOfSight = this->IsTargetInLineOfSight(this->Target);
		bool bIsTargetCloseEnough = this->IsTargetCloseEnough(this->ControlledCharacter->AggroTrigger->GetUnscaledSphereRadius() * 0.7f);

		if (bIsTargetInLineOfSight && bIsTargetCloseEnough)
		{
			if (!this->ControlledCharacter->bIsFiring)
			{
				this->ControlledCharacter->AimStart();
				this->ControlledCharacter->FireStart();
			}
		}
		else
		{
			this->ControlledCharacter->FireStop();
			this->ControlledCharacter->AimStop();
		}

		if (this->ControlledCharacter->AmmoInClip == 0)
		{
			this->ControlledCharacter->ReloadStart();
		}

		this->ChaseTarget(this->Target, 200.0f, bIsTargetInLineOfSight, DeltaTime);
	}
	else
	{
		this->MoveToLocation(this->HomeLocation);
	}
}

void AEnemyAIController::ChaseTarget(AActor* Target, float AcceptanceRadius, bool bIsTargetInLineOfSight, float DeltaTime)
{
	if (bIsTargetInLineOfSight)
	{
		this->bShouldMoveToHomeLocation = false;
		this->TargetLocation = Target->GetActorLocation();
		this->MoveToLocation(this->TargetLocation, AcceptanceRadius);
	}
	else
	{
		if (this->bShouldMoveToHomeLocation)
		{
			this->MoveToLocation(this->HomeLocation);
		}
		else
		{
			EPathFollowingRequestResult::Type PathRequestResult = this->MoveToLocation(this->TargetLocation, AcceptanceRadius);
			if (PathRequestResult == EPathFollowingRequestResult::AlreadyAtGoal)
			{
				this->DelayBeforeGoingToHomeLocationCounter += DeltaTime;
				if (this->DelayBeforeGoingToHomeLocationCounter >= this->DelayBeforeGoingToHomeLocation)
				{
					this->bShouldMoveToHomeLocation = true;
					this->DelayBeforeGoingToHomeLocationCounter = 0.0f;
				}
			}
		}
	}
}

bool AEnemyAIController::IsTargetInLineOfSight(AActor* Target) const
{
	bool bIsInLineOfSight = false;

	if (Target != NULL)
	{
		UWorld* World = this->GetWorld();
		if (World != NULL)
		{
			// Make a raycast to the Target to see if the it is visible
			const FVector RayStart = this->ControlledCharacter->GetActorLocation() + FVector(0.0f, 0.0f, this->ControlledCharacter->BaseEyeHeight);
			const FVector RayEnd = Target->GetActorLocation();
			FCollisionQueryParams QueryParams(FName(TEXT("TargetVisibility")), false, this);
			QueryParams.AddIgnoredActor(this->ControlledCharacter);
			FHitResult HitResult;

			if (World->LineTraceSingle(HitResult, RayStart, RayEnd, ECollisionChannel::ECC_Camera, QueryParams))
			{
				if (HitResult.Actor == Target)
				{
					// The target is visible, but we need to check if the target is in the line of sight of the enemy character
					FVector EnemyForwardVector = this->ControlledCharacter->GetActorForwardVector();
					FVector DirectionToTarget = Target->GetActorLocation() - this->ControlledCharacter->GetActorLocation();

					EnemyForwardVector.Normalize();
					DirectionToTarget.Normalize();

					float AngleBetween = FMath::Acos(FVector::DotProduct(EnemyForwardVector, DirectionToTarget));
					AngleBetween = FMath::RadiansToDegrees(AngleBetween);

					if (AngleBetween <= this->FieldOfView * 0.5f)
					{
						bIsInLineOfSight = true;
					}
				}
			}
		}
	}

	return bIsInLineOfSight;
}

bool AEnemyAIController::IsTargetCloseEnough(float AcceptanceRadius) const
{
	bool bIsCloseEnough = false;

	if ((this->Target->GetActorLocation() -  this->ControlledCharacter->GetActorLocation()).Size() <= AcceptanceRadius)
	{
		bIsCloseEnough = true;
	}

	return bIsCloseEnough;
}

AActor* AEnemyAIController::GetTarget() const
{
	return this->Target;
}

FVector AEnemyAIController::GetTargetLastKnownLocation() const
{
	return this->TargetLocation;
}

FVector AEnemyAIController::GetHomeLocation() const
{
	return this->HomeLocation;
}

void AEnemyAIController::OnAggroTriggerBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (MainCharacter != NULL)
	{
		this->Target = MainCharacter;
	}
}

void AEnemyAIController::OnAggroTriggerEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (MainCharacter != NULL)
	{
		this->Target = NULL;
	}
}
