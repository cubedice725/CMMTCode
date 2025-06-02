// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CMMTPData.generated.h"

USTRUCT(BlueprintType)
struct CMMT_API FCMMTPData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Affiliation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText PName;
};
