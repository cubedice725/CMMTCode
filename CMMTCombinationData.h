// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CMMTCombinationData.generated.h"

struct CMMT_API FCMMTCombinationData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint64 Index;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Case;
};
