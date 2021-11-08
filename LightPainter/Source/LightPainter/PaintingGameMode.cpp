// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/StereoLayerFunctionLibrary.h"
#include "Saving/PainterSaveGame.h"

void APaintingGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	Super::InitGame(MapName, Options, ErrorMessage);

	SlotName = UGameplayStatics::ParseOption(Options, "SlotName");

	UE_LOG(LogTemp, Warning, TEXT("SlotName: %s"), *SlotName);
}

void APaintingGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto LoadGame = UPainterSaveGame::Load(SlotName);
	if (LoadGame)
	{
		LoadGame->DeserializeToWorld(GetWorld());
		UStereoLayerFunctionLibrary::HideSplashScreen();

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game slot not Found %s"), *SlotName);
	}
}
