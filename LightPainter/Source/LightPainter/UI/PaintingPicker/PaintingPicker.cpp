// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingPicker.h"

#include "ActionBar.h"
#include "LightPainter/Saving/PainterSaveGameIndex.h"

// Sets default values
APaintingPicker::APaintingPicker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PaintingGrid = CreateDefaultSubobject<UWidgetComponent>(TEXT("PaintingGrid"));
	PaintingGrid->SetupAttachment(GetRootComponent());

	ActionBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("ActionBar"));
	ActionBar->SetupAttachment(GetRootComponent());
}


// Called when the game starts or when spawned
void APaintingPicker::BeginPlay()
{
	Super::BeginPlay();


	auto PaintingActionBar = Cast<UActionBar>(ActionBar->GetUserWidgetObject());
	if (PaintingActionBar)
	{
		PaintingActionBar->SetParentPicker(this);
	}

	Refresh();

}

void APaintingPicker::RefreshSlots()
{
	UE_LOG(LogTemp, Warning, TEXT("NumberOfPages, %d"), GetNumberOfPages());

	auto PaintingGridWidget = GetPaintingGrid();
	if (!PaintingGridWidget) return;

	PaintingGridWidget->ClearPaintings();

	int32 Index = 0;
	for (auto& SlotNamse : UPainterSaveGameIndex::Load()->GetSlotNames())
	{
		PaintingGridWidget->AddPainting(Index, SlotNamse);
		Index++;
	}
}

void APaintingPicker::RefreshDots()
{
	if (!GetPaintingGrid()) return;

	GetPaintingGrid()->ClearPaginationDots();

	for (int32 i = 0; i < GetNumberOfPages(); ++i)
	{
		GetPaintingGrid()->AddPaginationDot(CurrentPage == i);
	}
}

int32 APaintingPicker::GetNumberOfPages() const
{
	if (!GetPaintingGrid()) return 0;

	int32 TotalNumberOfSlot = UPainterSaveGameIndex::Load()->GetSlotNames().Num();
	int32 SlotsPerPage = GetPaintingGrid()->GetNumberOfSlots();

	return FMath::CeilToInt((float)TotalNumberOfSlot / SlotsPerPage);
}

void APaintingPicker::AddPainting()
{
	UPainterSaveGame::Create();

	Refresh();
}

void APaintingPicker::ToggleDeleteMode()
{
	if (!GetPaintingGrid()) return;

	GetPaintingGrid()->ClearPaintings();
}

