// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingGridCard.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UPaintingGridCard::SetPaintingName(FString _PaintingName)
{
	PaintingName = _PaintingName;

	SlotName->SetText(FText::FromString(PaintingName));

	CardButton->OnClicked.AddDynamic(this, &UPaintingGridCard::CardButtonClicked);
}

void UPaintingGridCard::CardButtonClicked()
{
	UStereoLayerFunctionLibrary::ShowSplashScreen();
	UGameplayStatics::OpenLevel(GetWorld(), "Canvas", true,"SlotName="+ PaintingName);
}
