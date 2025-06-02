// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CMMTPData.h"
#include "Blueprint/UserWidget.h"
#include "CMMTPEntryData.generated.h"

/**
 * 
 */
UCLASS()
class CMMT_API UCMMTPEntryData : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    FCMMTPData PData;

public:
    UFUNCTION(BlueprintCallable, Category = "CFun")
    void Init(const FCMMTPData& EntryData) { PData = EntryData; }

    UFUNCTION(BlueprintCallable, Category = "CFun")
    FCMMTPData GetEntryInfo() const { return PData; }
};
