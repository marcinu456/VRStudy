// Fill out your copyright notice in the Description page of Project Settings.


#include "HandController.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);
	MotionController->bDisplayDeviceModel = true;
	MotionController->SetCollisionProfileName("OverlapAllDynamic");
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();
	//MotionController->OnComponentBeginOverlap.AddDynamic(this, &AHandController::ComponentBeginOverlap);
	//MotionController->OnComponentEndOverlap.AddDynamic(this, &AHandController::ComponentEndOverlap);
	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsClimbing)
	{
		FVector HandControllerDelta = GetActorLocation() - ClimbingStartLocation;

		GetAttachParentActor()->AddActorWorldOffset(-HandControllerDelta);
	}
}

//void AHandController::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	bool bNewCanClimb = CanClimb();
//	if (!bCanClimb && bNewCanClimb)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Can Climb!"));
//	}
//	bCanClimb = bNewCanClimb;
//
//	UE_LOG(LogTemp, Warning, TEXT("ComponentBeginOverlap"));
//}
//
//void AHandController::ComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	bCanClimb = CanClimb();
//}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bool bNewCanClimb = CanClimb();
	if (!bCanClimb && bNewCanClimb)
	{
		APawn* Pawn = Cast<APawn>(GetAttachParentActor());
		if (Pawn)
		{
			APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
			if (Controller)
			{
				Controller->PlayHapticEffect(HapticEffect, MotionController->GetTrackingSource());
			}
		}
	}
	bCanClimb = bNewCanClimb;
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
}

bool AHandController::CanClimb() const
{

	TArray<AActor*> OverlappingActors;
	//MotionController->GetOverlappingActors(OverlappingActors);
	GetOverlappingActors(OverlappingActors);

	for (auto& OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->ActorHasTag(TEXT("Climbable")))
		{
			return true;
		}
	}

	return false;
}

void AHandController::Grip()
{
	if (!bCanClimb) return;

	if (!bIsClimbing)
	{
		bIsClimbing = true;
		ClimbingStartLocation = GetActorLocation();
	}
}

void AHandController::Release()
{
	bIsClimbing = false;
}