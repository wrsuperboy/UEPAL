// Copyright (C) 2022 Meizhouxuanhan.


#include "PALDialogDataTable.h"

UPALDialogDataTable::UPALDialogDataTable()
{
	RowStruct = FRichTextStyleRow::StaticStruct();
}

void UPALDialogDataTable::Init(UPALCommon* Common)
{
	FRichTextStyleRow RowCyan;
	RowCyan.TextStyle.SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
	RowCyan.TextStyle.SetColorAndOpacity(Common->GetDefaultPalette()[141]);
	RowCyan.TextStyle.SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RowCyan.TextStyle.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	AddRow(TEXT("Cyan"), RowCyan);

	FRichTextStyleRow RowRed;
	RowRed.TextStyle.SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
	RowRed.TextStyle.SetColorAndOpacity(Common->GetDefaultPalette()[26]);
	RowRed.TextStyle.SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RowRed.TextStyle.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	AddRow(TEXT("Red"), RowRed);

	FRichTextStyleRow RowRedAlt;
	RowRedAlt.TextStyle.SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
	RowRedAlt.TextStyle.SetColorAndOpacity(Common->GetDefaultPalette()[23]);
	RowRedAlt.TextStyle.SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RowRedAlt.TextStyle.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	AddRow(TEXT("RedAlt"), RowRedAlt);

	FRichTextStyleRow RowYellow;
	RowYellow.TextStyle.SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
	RowYellow.TextStyle.SetColorAndOpacity(Common->GetDefaultPalette()[45]);
	RowYellow.TextStyle.SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RowYellow.TextStyle.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	AddRow(TEXT("Yellow"), RowYellow);

	FRichTextStyleRow RowNum;
	RowNum.TextStyle.SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
	RowNum.TextStyle.SetColorAndOpacity(FColor(212, 200, 168));
	RowNum.TextStyle.SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RowNum.TextStyle.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	AddRow(TEXT("Num"), RowNum);
}