// Fill out your copyright notice in the Description page of Project Settings.


#include "UIPointerHandController.h"


AUIPointerHandController::AUIPointerHandController()
{
	Pointer = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Pointer"));
	Pointer->SetupAttachment(GetRootComponent());
}

void AUIPointerHandController::TriggerPressed()
{
	Super::TriggerPressed();
	Pointer->PressPointerKey(EKeys::LeftMouseButton);
}

void AUIPointerHandController::TriggerReleased()
{
	Super::TriggerReleased();
	Pointer->ReleasePointerKey(EKeys::LeftMouseButton);

}
