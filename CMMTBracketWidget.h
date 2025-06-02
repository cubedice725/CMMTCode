// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CMMTGameMode.h"
#include "CMMTMainMenu.h"
#include "CMMTRound.h"
#include "CMMTTable.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

#include "Blueprint/UserWidget.h"
#include "CMMTBracketWidget.generated.h"

/**
 * 
 */
UCLASS()
class CMMT_API UCMMTBracketWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "BracketWidget")
	TSubclassOf<UUserWidget> RoundWidgetClass;

	UPROPERTY(EditAnywhere, Category = "BracketWidget")
	TSubclassOf<UUserWidget> TableWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TArray<UUserWidget*> RoundArray;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox;

public:
	bool CreateBracketUserWidget(TArray<TArray<TArray<int8>>> Bracket, TArray<FString> Participants, int8 TableValue);
};
