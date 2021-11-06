// Fill out your copyright notice in the Description page of Project Settings.


#include "PainterSaveGame.h"
#include "Kismet/GameplayStatics.h"

UPainterSaveGame* UPainterSaveGame::Create()
{
	USaveGame* NewSaveGame = UGameplayStatics::CreateSaveGameObject(StaticClass());
	return Cast<UPainterSaveGame>(NewSaveGame);
}

bool UPainterSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, "Test", 0);
}

UPainterSaveGame* UPainterSaveGame::Load()
{
	auto LoadSloteGame = UGameplayStatics::LoadGameFromSlot("Test", 0);
	return Cast<UPainterSaveGame>(LoadSloteGame);
}
