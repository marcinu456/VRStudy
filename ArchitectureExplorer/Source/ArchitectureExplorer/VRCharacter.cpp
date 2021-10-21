// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "UObject/Object.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->SetTrackingSource(EControllerHand::Left);
	LeftController->bDisplayDeviceModel = true;

	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VRRoot);
	RightController->SetTrackingSource(EControllerHand::Right);
	RightController->bDisplayDeviceModel = true;

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(RightController);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());
	DestinationMarker->SetCollisionProfileName("NoCollision");

	PostProccesComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProccesComponent"));
	PostProccesComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	DestinationMarker->SetVisibility(false);

	if (BlinkerMaterialBase)
	{
		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		PostProccesComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);

		BlinkerMaterialInstance->SetScalarParameterValue("RadiusSize", .6);
	}
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	UpdateDestinationMarker();
	UpdateBlinkers();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Move_Y"),this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Move_X"),this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Teleport"),IE_Released,this, &AVRCharacter::BeginTeleport);
}

bool AVRCharacter::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation)
{
	
	FVector Start = RightController->GetComponentLocation();

	FVector Look = RightController->GetForwardVector();


	FPredictProjectilePathParams Params(TeleportProjectileRadius,Start,Look*TeleportProjectileSpeed,
		TeleportSimulationTime,ECC_Visibility,this);

	Params.bTraceComplex = true; //You can disable if you have better collision on scene

	FPredictProjectilePathResult Result;

	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

	if(!bHit)
	{
		return false;
	}

	
	//OutPath.Empty();
	for(auto& PointData: Result.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	FNavLocation NavLocation;
	bool bOnNavMesh = UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(Result.HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh)
	{
		return false;
	}

	OutLocation = NavLocation.Location;

	return true;
}

void AVRCharacter::UpdateDestinationMarker()
{
	TArray<FVector> Path;
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Path, Location);

	if(bHasDestination)
	{
		DestinationMarker->SetWorldLocation(Location);
		DestinationMarker->SetVisibility(true);

		DrawTeleporthPath(Path);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
		DrawTeleporthPath(TArray<FVector>());

	}

}

void AVRCharacter::UpdateBlinkers()
{
	if (!RadiusVsVelocity)
	{
		return;
	}
	float Speed = GetVelocity().Size();
	float Radius = RadiusVsVelocity->GetFloatValue(Speed);
	BlinkerMaterialInstance->SetScalarParameterValue("RadiusSize", Radius);

	FVector2D Centre = GetBlinkerCentre();

	BlinkerMaterialInstance->SetVectorParameterValue("Centre", FLinearColor(Centre.X, Centre.Y, 0));

}

void AVRCharacter::DrawTeleporthPath(const TArray<FVector>& Path)
{
	UpdateSpline(Path);

	for(auto SplineMesh: TeleportPathMeshPool)
	{
		SplineMesh->SetVisibility(false);
	}

	int32 SegmentNum = Path.Num() - 1;
	for (int32 i = 0; i < SegmentNum; ++i)
	{
		if (TeleportPathMeshPool.Num() <= i)
		{
			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMesh->SetStaticMesh(TeleportArchMesh);
			SplineMesh->SetMaterial(0, TeleportArchMaterial);
			SplineMesh->RegisterComponent();

			TeleportPathMeshPool.Add(SplineMesh);
		}

		USplineMeshComponent* SplineMesh = TeleportPathMeshPool[i];
		SplineMesh->SetVisibility(true);

		FVector StarPos, StartTangent, EndPos, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StarPos, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i+1, EndPos, EndTangent);
		SplineMesh->SetStartAndEnd(StarPos, StartTangent, EndPos, EndTangent);
	}


}

void AVRCharacter::UpdateSpline(const TArray<FVector> &Path)
{
	TeleportPath->ClearSplinePoints(false);
	int32 index = 0;
	for(auto& Point: Path)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Point);
		TeleportPath->AddPoint(FSplinePoint(index, LocalPosition, ESplinePointType::Curve),false);
		index++;
	}

	TeleportPath->UpdateSpline();
}

FVector2D AVRCharacter::GetBlinkerCentre()
{
	return FVector2D(.5, .5);
	/* Maybe Sometimes
	FVector MovementDirection = GetVelocity().GetSafeNormal();
	if(MovementDirection.IsNearlyZero())
	{
		return FVector2D(.5, .5);
	}
	FVector WorldStationaryLaction;

	if(FVector::DotProduct(Camera->GetForwardVector(),MovementDirection)>0)
	{
		WorldStationaryLaction = Camera->GetComponentLocation() + MovementDirection * 100;
	}
	else
	{
		WorldStationaryLaction = Camera->GetComponentLocation() - MovementDirection * 100;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return FVector2D(.5, .5);
	}

	FVector2D ScreenStationaryLocation;
	PC->ProjectWorldLocationToScreen(WorldStationaryLaction, ScreenStationaryLocation);

	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);
	ScreenStationaryLocation.X /= SizeX;
	ScreenStationaryLocation.Y /= SizeY;
	return ScreenStationaryLocation;
	*/
}

void AVRCharacter::MoveForward(float throttle)
{
	AddMovementInput(throttle * Camera->GetForwardVector());
}

void AVRCharacter::MoveRight(float throttle)
{
	AddMovementInput(throttle * Camera->GetRightVector());
}


void AVRCharacter::BeginTeleport()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	StartFade(0, 1);
	FTimerHandle Handle;

	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, TeleportFadeTime, false);
}

void AVRCharacter::FinishTeleport()
{
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
	SetActorLocation(Destination);

	StartFade(1, 0);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeTime, FLinearColor::Black);
	}
}