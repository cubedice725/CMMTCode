// Fill out your copyright notice in the Description page of Project Settings.


#include "CMMTBracketWidget.h"

bool UCMMTBracketWidget::CreateBracketUserWidget(TArray<TArray<TArray<int8>>> Bracket, TArray<FString> Participants, int8 TableValue)
{
    ScrollBox->ClearChildren();
    float  FColor = 0.3;
    float  LColor = 0.3;
    float  FAlpha = 0.4;
    float  LAlpha = 0.8;


    for (int RoundIndex = 0; RoundIndex < Bracket.Num() ; ++RoundIndex)
    {
        RoundArray.Add(Cast<UCMMTRound>(CreateWidget<UUserWidget>(GetWorld(), RoundWidgetClass)));
        UCMMTRound* RoundWidget = Cast<UCMMTRound>(RoundArray.Top());
        RoundWidget->RoundNumText->SetText(FText::FromString(FString::Printf(TEXT("라운드%d"), RoundIndex + 1)));
        
        if (RoundIndex % 2 == 1) 
        { 
            RoundWidget->RoundImage->SetBrushTintColor(FLinearColor(FColor, FColor, FColor, FAlpha));
            RoundWidget->RoundNumText->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
        }
        else
        {
            RoundWidget->RoundImage->SetBrushTintColor(FLinearColor(LColor, LColor, LColor, LAlpha));
            RoundWidget->RoundNumText->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
        }


        for (int TableIndex = 0; TableIndex < Bracket[RoundIndex].Num(); ++TableIndex)
        {
            RoundWidget->TableArray.Add(CreateWidget<UUserWidget>(GetWorld(), TableWidgetClass));
            UCMMTTable* TableWidget = Cast<UCMMTTable>(RoundWidget->TableArray.Top());
            if (TableIndex < TableValue)
            {
                TableWidget->TableNumText->SetText(FText::FromString(FString::Printf(TEXT("지정 테이블%d"), TableIndex + 1)));
            }
            else
            {
                TableWidget->TableNumText->SetText(FText::FromString(FString::Printf(TEXT("테이블%d"), TableIndex + 1)));
            }

            if (RoundIndex % 2 == 1) 
            { 
                TableWidget->TableImage->SetBrushTintColor(FLinearColor(FColor, FColor, FColor, FAlpha));
                TableWidget->TableNumText->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
            }
            else
            {
                TableWidget->TableImage->SetBrushTintColor(FLinearColor(LColor, LColor, LColor, LAlpha));
                TableWidget->TableNumText->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
            }


            TableWidget->MemberText1->SetText(FText::FromString(Participants[Bracket[RoundIndex][TableIndex][0]]));
            TableWidget->MemberText2->SetText(FText::FromString(Participants[Bracket[RoundIndex][TableIndex][1]]));
            TableWidget->MemberText3->SetText(FText::FromString(Participants[Bracket[RoundIndex][TableIndex][2]]));
            TableWidget->MemberText4->SetText(FText::FromString(Participants[Bracket[RoundIndex][TableIndex][3]]));

            RoundWidget->GetVerticalBox()->AddChild(RoundWidget->TableArray.Top());
        }
        ScrollBox->AddChild(RoundWidget);
    }

    return true;
}