// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

class CMMT_API CMMTBracketThread : public FRunnable
{
public:
	CMMTBracketThread(const TMap<FString, TArray<FString>>& Map, int64 PValue, int64 AValue, int64 TableValue, int64 RoundValue, bool PDupCheckValue, TArray<FString> Name);
	~CMMTBracketThread();
	virtual bool Init() override;
	virtual uint32 Run()  override;
	virtual void Stop()  override;
	virtual void Exit()  override;
	void EnsureCompletion();
	bool ThreadEndCheck() { return StopTaskCounter.GetValue() ? IsThreadEnd : false; }
	TArray<TArray<TArray<int8>>> GetBracket() { return StopTaskCounter.GetValue() ? Bracket : TArray<TArray<TArray<int8>>>(); }
	int32 GetBracketNum() { return BracketCount; }

private:
	enum Temps {
		// TSwap
		TSwapTemp0,
		TSwapTemp1,
		TSwapTemp2,
		TSwapTemp3,
		TSwapTemp4,
		// AMix
		AMixTemp,
		// IsDup
		IsDupForIndex,
		TableMatchIndex,
		// Skip
		SkipForIndexI,
		SkipForIndexII,
		// LineInsertArray
		LATemp,
		// PAndTableMemo
		PTMemoForIndex,
		// MemberTargetDupCheck
		MTDCForIndex,
		MTDCTemp,
		// CreateMemoTable
		ColIndex,
		RowIndex,
		MemoTableTemp,
		// RemoveInsert
		RITemp,
		End
	};
	TArray<int8> Temps; // Temps
	TArray<int8> ConfirmAArr; // [맴버] = 소속
	
	int8 PSize = 0; // 참가자 수
	int32 BracketCount = 0;

	TArray<TArray<TArray<int8>>> Bracket; // [라운드][테이블][멤버]
	TMap<FString, TArray<FString>> PMap; // 참가자
	TArray<TArray<int8>> DistributeAArr; // 적합한 소속 확인을 위한 배열
	TSet<FString> MembersHistory;

	TArray<TArray<int8>> PMemo; // [맴버][대상 멤버] = 만난 횟수
	TArray<int8>TableMemo; // [맴버] = 만난 횟수
	TArray<FString> PName;
	FRunnableThread* Thread = nullptr;
	FThreadSafeCounter StopTaskCounter;

	bool IsThreadEnd = false;
	int32 TableAssignment;

	bool CanPDup; // 중복 휫수 무한 
	int8 PDNum; // 참가자 중복 수
	int8 ADNum; // 최소 1 설정해야 함 최대는 4 그이상 무의미
	int8 TDNum; // 0이면 무의미 인원수 대비 테이블이 넘치면 값이 안나옴(TableIsDup)
	int8 RoundNum; // 라운드 설정
private:
	bool Skip(TArray<TArray<int8>>& Round, TArray<int8> Members, FString& Line);

	// 중복 확인
	bool MemberTargetDupCheck(const int8& MemberIndex, const int8& TargetIndex, const TArray<int8>& Members, FString& Line, TArray<int8>& MemberTemps);
	void PAndTableMemo(const TArray<TArray<int8>>& Round);
	bool PIsDup(const int8& Member, const int8& Target);
	bool TableIsDup(const int8& Member);
	bool AIsDup(TArray<int8>& Table);
	// FString Line
	void MDInit(FString& Line);
	void MDPMemoTable(FString& Line);
	void MDTableMemoTable(FString& Line);
	void CSVInsertRound(FString& Line, const TArray<TArray<int8>>& Round);
	void LineInsertArray(FString& Line, const FString& Title, const TArray<int8>& Array);
	void SaveLine(const FString& Line, const FString& Title);

	// 알고리즘
	bool AreAllElementsEqual(const TArray<int8>& Arr);
	void CustomShuffle(TArray<int8>& TargetArray);
	void DistributeAArrCorrection(TArray<TArray<int8>>& Arrs, const int8& Num);
	bool DistributeGroups(TArray<TArray<int8>>& Groups);
	void RemoveInsert(const int8& InsertIndex, const int8& TargetIndex, TArray<int8>& Arr);
	void TSwap(TArray<int8>& TSwapAs, TArray<int8>& TSwapBs);
	void TSwap(int8& TSwapA, int8& TSwapB);
};
