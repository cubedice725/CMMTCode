// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Blueprint/UserWidget.h"
#include "CMMTTable.generated.h"

UCLASS()
class CMMT_API UCMMTTable : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MemberText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MemberText2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MemberText3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MemberText4;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TableNumText;

	UPROPERTY(meta = (BindWidget))
	UImage* TableImage;
};
