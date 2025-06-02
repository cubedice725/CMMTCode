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
	TArray<int8> ConfirmAArr; // [�ɹ�] = �Ҽ�
	
	int8 PSize = 0; // ������ ��
	int32 BracketCount = 0;

	TArray<TArray<TArray<int8>>> Bracket; // [����][���̺�][���]
	TMap<FString, TArray<FString>> PMap; // ������
	TArray<TArray<int8>> DistributeAArr; // ������ �Ҽ� Ȯ���� ���� �迭
	TSet<FString> MembersHistory;

	TArray<TArray<int8>> PMemo; // [�ɹ�][��� ���] = ���� Ƚ��
	TArray<int8>TableMemo; // [�ɹ�] = ���� Ƚ��
	TArray<FString> PName;
	FRunnableThread* Thread = nullptr;
	FThreadSafeCounter StopTaskCounter;

	bool IsThreadEnd = false;
	int32 TableAssignment;

	bool CanPDup; // �ߺ� �ܼ� ���� 
	int8 PDNum; // ������ �ߺ� ��
	int8 ADNum; // �ּ� 1 �����ؾ� �� �ִ�� 4 ���̻� ���ǹ�
	int8 TDNum; // 0�̸� ���ǹ� �ο��� ��� ���̺��� ��ġ�� ���� �ȳ���(TableIsDup)
	int8 RoundNum; // ���� ����
private:
	bool Skip(TArray<TArray<int8>>& Round, TArray<int8> Members, FString& Line);

	// �ߺ� Ȯ��
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

	// �˰���
	bool AreAllElementsEqual(const TArray<int8>& Arr);
	void CustomShuffle(TArray<int8>& TargetArray);
	void DistributeAArrCorrection(TArray<TArray<int8>>& Arrs, const int8& Num);
	bool DistributeGroups(TArray<TArray<int8>>& Groups);
	void RemoveInsert(const int8& InsertIndex, const int8& TargetIndex, TArray<int8>& Arr);
	void TSwap(TArray<int8>& TSwapAs, TArray<int8>& TSwapBs);
	void TSwap(int8& TSwapA, int8& TSwapB);
};
