// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingPicker.h"

#include "ActionBar.h"
#include "PaintingGrid.h"
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

	RefreshSlots();

}

void APaintingPicker::RefreshSlots()
{
	auto PaintingGridWidget = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
	if (!PaintingGridWidget) return;

	PaintingGridWidget->AddPaginationDot(true);
	PaintingGridWidget->AddPaginationDot(false);
	PaintingGridWidget->AddPaginationDot(false);
	PaintingGridWidget->ClearPaintings();

	int32 Index = 0;
	for (auto& SlotNamse : UPainterSaveGameIndex::Load()->GetSlotNames())
	{
		PaintingGridWidget->AddPainting(Index, SlotNamse);
		Index++;
	}
}

void APaintingPicker::AddPainting()
{
	UPainterSaveGame::Create();

	RefreshSlots();
}

void APaintingPicker::ToggleDeleteMode()
{
	auto PaintingGridWidget = Cast<UPaintingGrid>(PaintingGrid->GetUserWidgetObject());
	if (!PaintingGridWidget) return;

	PaintingGridWidget->ClearPaintings();
}

