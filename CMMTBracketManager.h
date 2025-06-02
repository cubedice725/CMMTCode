// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CMMTBracketThread.h"
#include "CMMTPData.h"
#include "Components/ListView.h"
#include "Blueprint/UserWidget.h"
#include "UObject/NoExportTypes.h"
#include "CMMTBracketManager.generated.h"

UCLASS()
class CMMT_API UCMMTBracketManager : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void SpawnNormalTableActors(TSoftObjectPtr<UWorld> World);

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void ClearNormalTableActors();

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void ListViewToMap(const UListView* ListView);

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void CreateBracketThread();
	
	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void StopBracketThread();
	
	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void CreateBracketUserWidget(UCMMTBracketWidget* BracketUserWidget);

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	FString InputInformation(int64 MaxValue, int64 MinValue, int64 PValue, int64 AValue, int64 TableValue, int64 RoundValue, bool PDupCheckValue);
	
	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void SetIsCreateBracketUserWidgetEnd(bool Condition) { IsCreateBracketUserWidgetEnd = Condition; };

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void SetIsStartSequenceEnd(bool Condition) { IsStartSequenceEnd = Condition; };

	UFUNCTION(BlueprintCallable, Category = "BracketManager")
	void SetIsFailedCreateBracket(bool Condition) { IsFailedCreateBracket = Condition; };

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsStartSequenceEnd = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsCreateBracketUserWidgetEnd = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsBracketThreadStart = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsFailedCreateBracket = false;
private:
	
	TArray<AStaticMeshActor*> TableActors;
	TArray<TArray<TArray<int8>>> Bracket; // [라운드][테이블][멤버]
	TMap<FString, TArray<FString>> PTMap; // [소속][이름]
	CMMTBracketThread* Worker = nullptr;

	TArray<FString> MapToFString(const TMap<FString, TArray<FString>> Map);
	TArray<FCMMTPData> ListViewToPDatas(const UListView* ListView);
	TMap<FString, TArray<FString>> PDatasToMap(const TArray<FCMMTPData>& Datas);
	

	int32 ListViewCount;
	int8 PData = 0;
	int8 AData = 0;
	int8 TData = 0;
	int8 RoundData = 0;
	bool PDupCheckData = false;
};