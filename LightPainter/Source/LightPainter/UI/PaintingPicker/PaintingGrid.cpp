// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingGrid.h"

#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"

void UPaintingGrid::AddPainting(int32 PaintingIndex, FString PaintingName)
{
	if (!PaintingGrid) { return; }

	auto NewWidget = CreateWidget<UPaintingGridCard>(GetWorld(), GridCardClass);
	if (!NewWidget) { return; }

	NewWidget->SetPaintingName(PaintingName);

	auto CardCointener = Cast<USizeBox>(PaintingGrid->GetChildAt(PaintingIndex));
	if (!CardCointener) {return;}
	
	CardCointener->AddChild(NewWidget);
}

void UPaintingGrid::ClearPaintings()
{
	for (int32 i = 0; i < PaintingGrid->GetChildrenCount(); ++i)
	{
		auto CardCointener = Cast<USizeBox>(PaintingGrid->GetChildAt(i));
		if (!CardCointener) continue;

		CardCointener->ClearChildren();
	}
}

void UPaintingGrid::ClearPaginationDots()
{
	if (!PaginationDots) return;

	PaginationDots->ClearChildren();
}

void UPaintingGrid::AddPaginationDot(bool Active)
{
	if (!PaginationDots) return;

	auto Dot = CreateWidget<UPaginationDot>(GetWorld(), PaginationDotClass);
	if (!Dot) return;

	Dot->SetActive(Active);
	UHorizontalBoxSlot* tmpSlot = PaginationDots->AddChildToHorizontalBox(Dot);
	tmpSlot->SetPadding(FMargin(PaginationDotPadding, 0));
}

int32 UPaintingGrid::GetNumberOfSlots() const
{
	return PaintingGrid->GetChildrenCount();
}
