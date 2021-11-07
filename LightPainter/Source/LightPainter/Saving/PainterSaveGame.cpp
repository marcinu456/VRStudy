// Fill out your copyright notice in the Description page of Project Settings.


#include "PainterSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "LightPainter/Stroke.h"
#include "Misc/Guid.h"

UPainterSaveGame* UPainterSaveGame::Create()
{
	UPainterSaveGame* NewSaveGame = Cast<UPainterSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
	NewSaveGame->SlotName = FGuid::NewGuid().ToString();
	return NewSaveGame;
}

bool UPainterSaveGame::Save()
{
	return UGameplayStatics::SaveGameToSlot(this, SlotName, 0);
}

UPainterSaveGame* UPainterSaveGame::Load(FString LoadSlot)
{
	return Cast<UPainterSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadSlot, 0));
}

void UPainterSaveGame::SerializeFromWorld(UWorld* World)
{
	Strokes.Empty();
	for (TActorIterator<AStroke> StrokeItr(World); StrokeItr; ++StrokeItr)
	{
		Strokes.Add(StrokeItr->SerializeToStruct());
	}
}

void UPainterSaveGame::DeserializeToWorld(UWorld* World)
{
	ClearWorld(World);
	for (auto& StrokeState : Strokes)
	{
		AStroke::SpawnAndDeserializeFromStruct(World, StrokeState);
	}
}

void UPainterSaveGame::ClearWorld(UWorld* World)
{
	for (TActorIterator<AStroke> StrokeItr(World); StrokeItr; ++StrokeItr)
	{
		StrokeItr->Destroy();
	}
}