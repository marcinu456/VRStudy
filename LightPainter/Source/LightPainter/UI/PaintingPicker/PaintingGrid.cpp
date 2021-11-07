// Fill out your copyright notice in the Description page of Project Settings.


#include "PaintingGrid.h"
#include "Components/SizeBox.h"

void UPaintingGrid::AddPainting()
{
	if (!PaintingGrid) { return; }

	auto NewWidget = CreateWidget<UUserWidget>(GetWorld(), GridCardClass);
	if (!NewWidget) { return; }

	auto CardCointener = Cast<USizeBox>(PaintingGrid->GetChildAt(0));
	if (!CardCointener) {return;}
	
	CardCointener->AddChild(NewWidget);
}
