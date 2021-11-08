// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingGrid.h"
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