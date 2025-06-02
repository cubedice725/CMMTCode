// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CMMTTable.h"
#include "Components/VerticalBox.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "CMMTRound.generated.h"

/**
 * 
 */
UCLASS()
class CMMT_API UCMMTRound : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* TableVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TArray<UUserWidget*> TableArray;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundNumText;

	UPROPERTY(meta = (BindWidget))
	UImage* RoundImage;

	UVerticalBox* GetVerticalBox() { return TableVerticalBox; }
};
