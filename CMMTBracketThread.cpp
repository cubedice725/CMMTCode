// Fill out your copyright notice in the Description page of Project Settings.


#include "CMMTBracketThread.h"

CMMTBracketThread::CMMTBracketThread(const TMap<FString, TArray<FString>>& Map, int64 PValue, int64 AValue, int64 TableValue, int64 RoundValue, bool PDupCheckValue, TArray<FString> Name)
    : PMap(Map), PDNum(PValue), ADNum(AValue), TDNum(TableValue), RoundNum(RoundValue), CanPDup(PDupCheckValue), PName(Name)
{
    Thread = FRunnableThread::Create(this, TEXT("CombinationThread"));
}
CMMTBracketThread::~CMMTBracketThread()
{
    EnsureCompletion();
    delete Thread;
    Thread = nullptr;
}
bool CMMTBracketThread::Init()
{
    Temps.Init(0, End);

    int8 Index = 0;
    for (const TPair<FString, TArray<FString>>& Elem : PMap)
    {
        for (const FString& Name : Elem.Value)
        {
            ConfirmAArr.Add(Index);
        }
        ++Index;
        PSize = ConfirmAArr.Num();  // 총 인원 수
    }
    
    // Memo 초기화
    PMemo.Init(TArray<int8>(), PSize);
    for (TArray<int8>& Memo : PMemo) { Memo.Init(0, PSize); }
    TableMemo.Init(0, PSize);

    TableAssignment = (RoundNum / (PSize / 4)) * TDNum;

    return true;
}
uint32 CMMTBracketThread::Run()
{
    // 실제 스레드에서 실행될 작업
    if (StopTaskCounter.GetValue() == 0)
    {
        FString CombinedString;
        FString CSVLine; // 대진표 문자열로 변환된 값 저장될 변수
        FString MarkDownLine; // 대진표 작성 알고리즘의 로그를 기록할 변수
        TArray<TArray<int8>> Round; // 순열에 사용될 Round
        TArray<int8> Members;
        int32 Index = 0;
        bool IsSuccess = false;
        FString ParticipantLine;
        FString EndLine;
        while (true)
        {
            
            if (ADNum < 4 && !DistributeGroups(DistributeAArr))
            {
                MarkDownLine += TEXT("# 소속사 분배 불가능");
                SaveLine(MarkDownLine, "Log.md");
                return 0;
            }

            MDInit(MarkDownLine);
            Round.SetNum(PSize / 4); // 테이블 개수만큼 준비
            for (TArray<int8>& Table : Round) { Table.Init(0, 4); }

            // 멤버 열생성
            Members.Empty();
            for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < PSize; ++Temps[SkipForIndexI]) { Members.Add(Temps[SkipForIndexI]); }
            CustomShuffle(Members);
            for (int8 Member : Members)
            {
                CombinedString += FString::FromInt(Member);
            }
            if (!MembersHistory.Contains(CombinedString))
            {
                MembersHistory.Add(CombinedString);
            }
            BracketCount = 0;

            while (true)
            {
                IsSuccess = Skip(Round, Members, MarkDownLine);
                if (IsSuccess)
                {
                    Bracket.Add(Round);
                    PAndTableMemo(Round);
                    CSVInsertRound(CSVLine, Round);

                    ++BracketCount;
                    if (StopTaskCounter.GetValue() != 0) { break; }
                    if (BracketCount >= RoundNum) { MarkDownLine += TEXT("# 대진표 생성 성공");  break; } // 필요한 라운드만큼 생성시 종료

                    for (Index = 0; Index < Members.Num(); ++Index) { Members[Index] = (Members[Index] + 1) % PSize; }
                }
                else
                {
                    
                    if (TDNum != 0 && AreAllElementsEqual(TableMemo)) 
                    { 
                        ++TableAssignment; 
                        MarkDownLine += FString::Printf(TEXT(" 테이블 %d회 중복 증가"), TableAssignment) + LINE_TERMINATOR;
                        continue; 
                    }
                    SaveLine(MarkDownLine, "Log.md");
                    
                    // Memo 초기화
                    for (TArray<int8>& Memo : PMemo) { Memo.Init(0, PSize); }
                    TableMemo.Init(0, PSize);

                    Bracket.Empty();
                    CSVLine = "";
                    MarkDownLine = "";
                    break;
                }
            }

            if (IsSuccess) 
            {

                for (const TArray<TArray<int8>>& ARound : Bracket)
                {
                    for (const TArray<int8>& ATable : ARound)
                    {
                        for (const int8& AMember : ATable)
                        {
                            ParticipantLine += FString::Printf(TEXT("%s"), *PName[AMember]) + TEXT(",");
                        }
                        ParticipantLine = ParticipantLine.LeftChop(1);
                        ParticipantLine += LINE_TERMINATOR;
                    }
                    ParticipantLine += LINE_TERMINATOR;
                }

                ParticipantLine += LINE_TERMINATOR;
                MDPMemoTable(EndLine); // 표 생성됨
                MDTableMemoTable(EndLine); // 표 생성됨 
                SaveLine(EndLine, "Memo.md");
                SaveLine(MarkDownLine, "Log.md");
                SaveLine(CSVLine, "Breaket.csv");
                SaveLine(ParticipantLine, "ParticipantBreaket.csv");

                return 0; 
            }
        }
    }
    return 0;
}
void CMMTBracketThread::Stop()
{
    StopTaskCounter.Increment();
}
void CMMTBracketThread::Exit()
{
    StopTaskCounter.Increment();
	IsThreadEnd = true;
}
void CMMTBracketThread::EnsureCompletion()
{
    Stop();
    if (Thread)
    {
        Thread->WaitForCompletion();
    }
}
bool CMMTBracketThread::Skip(TArray<TArray<int8>>& Round, TArray<int8> Members, FString& Line)
{
    // 메모 확인
    MDPMemoTable(Line);
    MDTableMemoTable(Line);

    if (TDNum != 0)
    {
        Line += TEXT("## 첫번째 기준 참가자 ");

        // 첫번째 기준 참가자 테이블 중복 확인
        for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < PSize; ++Temps[SkipForIndexI])
        {
            if (!TableIsDup(Members[Temps[SkipForIndexI]]))
            {
                Line += FString::Printf(TEXT("%d"), Members[Temps[SkipForIndexI]]);
                RemoveInsert(0, Temps[SkipForIndexI], Members);
                break;
            }

            if (Temps[SkipForIndexI] >= PSize - 1) { Line += TEXT("존재 하지 않음"); return false; }
        }
        Line += LINE_TERMINATOR LINE_TERMINATOR;
    }
    if (ADNum < 4)
    {
        DistributeAArrCorrection(DistributeAArr, ConfirmAArr[Members[0]]);
        Line += TEXT("## 소속 분배: ");
        for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < PSize; ++Temps[SkipForIndexI])
        {
            if (Temps[SkipForIndexI] % 4 == 3)
            {
                Line += FString::Printf(TEXT("%d "), DistributeAArr[Temps[SkipForIndexI] / 4][Temps[SkipForIndexI] % 4]);
            }
            else
            {
                Line += FString::Printf(TEXT("%d,"), DistributeAArr[Temps[SkipForIndexI] / 4][Temps[SkipForIndexI] % 4]);
            }
        }
        Line = Line.LeftChop(1);

        Line += LINE_TERMINATOR LINE_TERMINATOR;

        Line += FString::Printf(TEXT("## 현재 소속: "));

        for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < PSize; ++Temps[SkipForIndexI])
        {
            if (Temps[SkipForIndexI] % 4 == 3)
            {
                Line += FString::Printf(TEXT("%d "), ConfirmAArr[Temps[SkipForIndexI]]);
            }
            else
            {
                Line += FString::Printf(TEXT("%d,"), ConfirmAArr[Temps[SkipForIndexI]]);
            }
        }
        Line = Line.LeftChop(1);

        Line += LINE_TERMINATOR LINE_TERMINATOR;
    }

    LineInsertArray(Line, TEXT("시작 행렬 "), Members);

    TArray<int8> MemberTemps;

    // 기준 참가자
    for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < PSize - 1; ++Temps[SkipForIndexI])
    {
        Line += FString::Printf(TEXT("### 참가자 %d"), Members[Temps[SkipForIndexI]]);
        Line += LINE_TERMINATOR LINE_TERMINATOR;

        // 대상 참가자
        for (Temps[SkipForIndexII] = Temps[SkipForIndexI] + 1; Temps[SkipForIndexII] < PSize; ++Temps[SkipForIndexII])
        {
            // 실제 중복 확인
            if (MemberTargetDupCheck(Temps[SkipForIndexI], Temps[SkipForIndexII], Members, Line, MemberTemps))
            { 
                if (Temps[SkipForIndexII] >= PSize - 1) { Line += TEXT("# 대진표 생성 실패"); return false; } // 멤버 끝까지 가도 중복이면 중단
                continue; 
            }
            else
            {
                Line += FString::Printf(TEXT("참가자%d 중복 없음"), Members[Temps[SkipForIndexII]]);
            }
            Line += LINE_TERMINATOR LINE_TERMINATOR;

            // 기준 참가자가 대상 참가자가 아닌 경우
            if (Temps[SkipForIndexII] != Temps[SkipForIndexI] + 1)
            {
                Line += FString::Printf(TEXT("참가자%d "), Members[Temps[SkipForIndexII]]);
                
                RemoveInsert(Temps[SkipForIndexI] + 1, Temps[SkipForIndexII], Members);

                Line += FString::Printf(TEXT("인덱스%d에 넣음"), Temps[SkipForIndexI] + 1);
                Line += LINE_TERMINATOR LINE_TERMINATOR;

                LineInsertArray(Line, TEXT("현재 행렬"), Members);
            }

            break;
        }
    }

    // Run()에 사용될 배열에 넣어줌
    for (Temps[SkipForIndexI] = 0; Temps[SkipForIndexI] < Members.Num(); ++Temps[SkipForIndexI])
    {
        Round[Temps[SkipForIndexI] / 4][Temps[SkipForIndexI] % 4] = Members[Temps[SkipForIndexI]];
    }

    return true;
}

// 중복 확인
bool CMMTBracketThread::MemberTargetDupCheck(const int8& MemberIndex, const int8& TargetIndex, const TArray<int8>& Members, FString& Line, TArray<int8>& MemberTemps)
{
    // MemberIndex번 참가자 지정 테이블 확인
    if (TDNum != 0 && MemberIndex / 4 < TDNum && MemberIndex != TDNum * 4 - 1)
    {
        if (TableIsDup(Members[TargetIndex]))
        {
            Line += FString::Printf(TEXT("[%d] 참가자%d 지정 테이블 %d회 중복"), TargetIndex, Members[TargetIndex],TableMemo[Members[TargetIndex]]);
            Line += LINE_TERMINATOR LINE_TERMINATOR;
            return true;
        }
    }
    // 참가자 중복 확인
    for (Temps[MTDCForIndex] = 0; Temps[MTDCForIndex] <= MemberIndex % 4; ++Temps[MTDCForIndex])
    {
        if (CanPDup && PIsDup(Members[MemberIndex - Temps[MTDCForIndex]], Members[TargetIndex]))
        {
            Line += FString::Printf(TEXT("[%d] 참가자%d, 참가자%d 중복"), TargetIndex, Members[MemberIndex - Temps[MTDCForIndex]], Members[TargetIndex]);
            Line += LINE_TERMINATOR LINE_TERMINATOR;
            return true;
        }
    }

    // 참가자의 소속 중복 확인 
    if (ADNum < 4)
    {
        Temps[MTDCTemp] = MemberIndex + 1;
        if (DistributeAArr[Temps[MTDCTemp] / 4][Temps[MTDCTemp] % 4] != ConfirmAArr[Members[TargetIndex]])
        {
            Line += FString::Printf(TEXT("[%d] 참가자%d, 소속%d 원하는 소속%d와 불일치"), 
                TargetIndex,
                Members[TargetIndex],
                ConfirmAArr[Members[TargetIndex]],
                DistributeAArr[Temps[MTDCTemp] / 4][Temps[MTDCTemp] % 4]
            );
            Line += LINE_TERMINATOR LINE_TERMINATOR;
            return true;
        }
    }
    return false;
}
void CMMTBracketThread::PAndTableMemo(const TArray<TArray<int8>>& Round)
{
    if (!CanPDup && TDNum == 0) { return; }

    for (Temps[PTMemoForIndex] = 0; Temps[PTMemoForIndex] < Round.Num(); ++Temps[PTMemoForIndex])
    {
        if (CanPDup)
        {
            // 현재 테이블에서 만난 사람 저장
            PMemo[Round[Temps[PTMemoForIndex]][0]][Round[Temps[PTMemoForIndex]][1]]++;
            PMemo[Round[Temps[PTMemoForIndex]][0]][Round[Temps[PTMemoForIndex]][2]]++;
            PMemo[Round[Temps[PTMemoForIndex]][0]][Round[Temps[PTMemoForIndex]][3]]++;

            PMemo[Round[Temps[PTMemoForIndex]][1]][Round[Temps[PTMemoForIndex]][0]]++;
            PMemo[Round[Temps[PTMemoForIndex]][1]][Round[Temps[PTMemoForIndex]][2]]++;
            PMemo[Round[Temps[PTMemoForIndex]][1]][Round[Temps[PTMemoForIndex]][3]]++;

            PMemo[Round[Temps[PTMemoForIndex]][2]][Round[Temps[PTMemoForIndex]][0]]++;
            PMemo[Round[Temps[PTMemoForIndex]][2]][Round[Temps[PTMemoForIndex]][1]]++;
            PMemo[Round[Temps[PTMemoForIndex]][2]][Round[Temps[PTMemoForIndex]][3]]++;

            PMemo[Round[Temps[PTMemoForIndex]][3]][Round[Temps[PTMemoForIndex]][0]]++;
            PMemo[Round[Temps[PTMemoForIndex]][3]][Round[Temps[PTMemoForIndex]][1]]++;
            PMemo[Round[Temps[PTMemoForIndex]][3]][Round[Temps[PTMemoForIndex]][2]]++;
        }
        
        if (TDNum != 0 && Temps[PTMemoForIndex] < TDNum)
        {
            // 지정 테이블 저장
            TableMemo[Round[Temps[PTMemoForIndex]][0]]++;
            TableMemo[Round[Temps[PTMemoForIndex]][1]]++;
            TableMemo[Round[Temps[PTMemoForIndex]][2]]++;
            TableMemo[Round[Temps[PTMemoForIndex]][3]]++;
        }
    }
}
bool CMMTBracketThread::PIsDup(const int8& Member, const int8& Target)
{
    return PMemo[Member][Target] >= PDNum;
}
bool CMMTBracketThread::TableIsDup(const int8& Member)
{
    return TableMemo[Member] >= TableAssignment; // 자동적으로 몇번 매치 시킬지 설정
}
bool CMMTBracketThread::AIsDup(TArray<int8>& Table)
{
    Table.Sort(); // 앞에서 정렬을 안하는 경우 사용해야 함
    // 최대 중복 카운트 계산
    if (Table[0] == Table[3]) { return 4 > ADNum; } // 모든 요소 동일 (4)
    else if ((Table[0] == Table[2]) || (Table[1] == Table[3])) { return 3 > ADNum; } // 앞 3개 동일 (3), 뒤 3개 동일 (3)
    else if ((Table[0] == Table[1]) || (Table[1] == Table[2]) || (Table[2] == Table[3])) { return 2 > ADNum; } // 첫 쌍 (2), 중간 쌍 (2), 끝 쌍 (2)
    else { return 1 > ADNum; } // 모두 다름 (1)
}

// FSting Line
void CMMTBracketThread::MDInit(FString& Line)
{
    Line += TEXT("# 알고리즘 경위서") LINE_TERMINATOR;
    Line += TEXT("## 알고리즘 설정 목록") LINE_TERMINATOR;
    Line += FString::Printf(TEXT("- 참가자 %d명"), PSize) + LINE_TERMINATOR;

    if (CanPDup) { Line += FString::Printf(TEXT("- 참가자 %d번 중복"), PDNum) + LINE_TERMINATOR; }
    if (ADNum < 4) { Line += FString::Printf(TEXT("- 소속 %d명 중복"), ADNum) + LINE_TERMINATOR; }
    if (TDNum != 0) { Line += FString::Printf(TEXT("- 테이블 %d회 중복"), TableAssignment) + LINE_TERMINATOR; }
    if (!CanPDup && ADNum >= 4 && TDNum == 0) { Line += TEXT("- 해당사항 없음") LINE_TERMINATOR; }
    Line += TEXT("- - -") LINE_TERMINATOR;
}
void CMMTBracketThread::MDPMemoTable(FString& Line)
{
    if (CanPDup)
    {
        Line += TEXT("## 참가자 만남") LINE_TERMINATOR;
        Temps[MemoTableTemp] = PMemo[0].Num();

        // 헤더 작성
        Line += FString::Printf(TEXT("| %s "), TEXT("참가자"));
        for (Temps[ColIndex] = 0; Temps[ColIndex] < Temps[MemoTableTemp]; ++Temps[ColIndex])
        {
            Line += FString::Printf(TEXT("| %d "), Temps[ColIndex]);
        }
        Line += TEXT("|") LINE_TERMINATOR;

        // 구분선 작성
        Line += TEXT("|---");
        for (Temps[ColIndex] = 0; Temps[ColIndex] < Temps[MemoTableTemp]; ++Temps[ColIndex])
        {
            Line += TEXT("|---");
        }
        Line += TEXT("|") LINE_TERMINATOR;

        // 데이터 행 작성
        for (int8 IndexI = 0; IndexI < PMemo.Num(); ++IndexI)
        {
            Line += FString::Printf(TEXT("| %d "), IndexI);
            for (int8 IndexII = 0; IndexII < Temps[MemoTableTemp]; ++IndexII)
            {
                Line += FString::Printf(TEXT("| %d "), PMemo[IndexI][IndexII]);
            }
            Line += TEXT("|") LINE_TERMINATOR;
        }
        Line += LINE_TERMINATOR LINE_TERMINATOR;
    }
    
}
void CMMTBracketThread::MDTableMemoTable(FString& Line)
{
    if (TDNum != 0)
    {
        Line += TEXT("## 테이블 지정") LINE_TERMINATOR;
        Temps[MemoTableTemp] = TableMemo.Num();

        // 헤더 작성
        Line += FString::Printf(TEXT("| %s "), TEXT("참가자"));
        for (Temps[ColIndex] = 0; Temps[ColIndex] < Temps[MemoTableTemp]; ++Temps[ColIndex])
        {
            Line += FString::Printf(TEXT("| %d "), Temps[ColIndex]);
        }
        Line += TEXT("|") LINE_TERMINATOR;

        // 구분선 작성
        Line += TEXT("|---");
        for (Temps[ColIndex] = 0; Temps[ColIndex] < Temps[MemoTableTemp]; ++Temps[ColIndex])
        {
            Line += TEXT("|---");
        }
        Line += TEXT("|") LINE_TERMINATOR;

        // 데이터 행 작성
        Line += FString::Printf(TEXT("| %s "), TEXT("지정된 횟수"));
        for (int8 Member : TableMemo)
        {
            Line += FString::Printf(TEXT("| %d "), Member);
        }
        Line += TEXT("|") LINE_TERMINATOR;
        Line += LINE_TERMINATOR;
    }
}
void CMMTBracketThread::CSVInsertRound(FString& Line, const TArray<TArray<int8>>& Round)
{
    Line += TEXT("\"");

    for (const TArray<int8>& Table : Round)
    {
        for (const int8& Member : Table)
        {
            Line += FString::FromInt(Member) + TEXT(",");
        }
    }

    Line = Line.LeftChop(1);
    Line += TEXT("\"");
    Line += LINE_TERMINATOR;
}
void CMMTBracketThread::LineInsertArray(FString& Line, const FString& Title, const TArray<int8>& Array)
{
    Line += FString::Printf(TEXT("## %d 라운드 %s: "), BracketCount + 1, *Title);
    for (Temps[LATemp] = 0; Temps[LATemp] < PSize; ++Temps[LATemp])
    {
        if (Temps[LATemp] % 4 == 3)
        {
            Line += FString::Printf(TEXT("%d "), Array[Temps[LATemp]]);
        }
        else
        {
            Line += FString::Printf(TEXT("%d,"), Array[Temps[LATemp]]);
        }
    }
    Line = Line.LeftChop(1);

    Line += LINE_TERMINATOR LINE_TERMINATOR;
}
void CMMTBracketThread::SaveLine(const FString& Line, const FString& Title)
{
    /*FString SavedDir = FPaths::ProjectSavedDir();
    FString FullPath = SavedDir / Title;
    if (FFileHelper::SaveStringToFile(Line, *FullPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("CSV 저장 성공! 경로: %s"), *FullPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CSV 저장 실패!"));
    }*/
    FString FullPath = FPaths::Combine(FPlatformProcess::BaseDir(), TEXT("Documents/"), *Title);
    FString BaseDir = FPlatformProcess::BaseDir();
    FString DocumentsDir = FPaths::Combine(BaseDir, TEXT("Documents"));
    if (!IFileManager::Get().DirectoryExists(*DocumentsDir))
    {
        IFileManager::Get().MakeDirectory(*DocumentsDir);
    }
    // 파일 저장 (한 번에 전체 데이터 저장)
    if (FFileHelper::SaveStringToFile(Line, *FullPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("CSV 저장 성공! 경로: %s"), *FullPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CSV 저장 실패!"));
    }
}

// 알고리즘
bool CMMTBracketThread::AreAllElementsEqual(const TArray<int8>& Arr)
{
    if (TableMemo.Num() == 0)
    {
        // 빈 배열의 경우 true를 반환할지 false를 반환할지는 상황에 따라 다를 수 있습니다.
        // 여기서는 빈 배열도 모두 같다고 간주해 true를 반환합니다.
        return true;
    }

    int8 FirstValue = TableMemo[0];
    for (int32 i = 1; i < TableMemo.Num(); ++i)
    {
        if (TableMemo[i] != FirstValue)
        {
            return false;
        }
    }
    return true;
}
void CMMTBracketThread::CustomShuffle(TArray<int8>& TargetArray)
{
    FRandomStream RandomStream(FDateTime::Now().GetTicks());

    int32 LastIndex = TargetArray.Num() - 1;
    for (int32 i = 0; i <= LastIndex; ++i)
    {
        int32 SwapIndex = RandomStream.RandRange(i, LastIndex);
        TargetArray.Swap(i, SwapIndex);
    }
}
void CMMTBracketThread::DistributeAArrCorrection(TArray<TArray<int8>>& Arrs, const int8& Num)
{
    const int32 Correction = Num - Arrs[0][0]; // 보정값 방향 변경

    for (TArray<int8>& Arr : Arrs) {
        for (int8& Element : Arr) {
            Element = (Element + Correction + (PSize / 4)) % (PSize / 4);
        }
    }
}
bool CMMTBracketThread::DistributeGroups(TArray<TArray<int8>>& Groups)
{
    // 값의 빈도수 계산
    TMap<int8, int32> ValueCount;
    for (int8 Value : ConfirmAArr)
    {
        ValueCount.FindOrAdd(Value)++;
    }
    int8 MaxCount = PSize / 4;

    // DistributeAArr 초기화
    DistributeAArr.Empty();
    DistributeAArr.SetNum(MaxCount);
    int32 Num = 0;

    // 순환 방식으로 값 분배
    for (const auto& Elem : ValueCount)
    {
        int8 Value = Elem.Key;
        int32 Count = Elem.Value;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            DistributeAArr[Num % MaxCount].Add(Value);
            ++Num;
        }
    }
    for (TArray<int8>& Arr : DistributeAArr)
    {
        if (AIsDup(Arr)) { return false; }
    }
    return true;
}
void CMMTBracketThread::RemoveInsert(const int8& InsertIndex, const int8& TargetIndex, TArray<int8>& Arr)
{
    Temps[RITemp] = Arr[TargetIndex];
    Arr.RemoveAt(TargetIndex);
    Arr.Insert(Temps[RITemp], InsertIndex);
}
void CMMTBracketThread::TSwap(TArray<int8>& TSwapAs, TArray<int8>& TSwapBs)
{
    Temps[TSwapTemp1] = TSwapAs[0];
    Temps[TSwapTemp2] = TSwapAs[1];
    Temps[TSwapTemp3] = TSwapAs[2];
    Temps[TSwapTemp4] = TSwapAs[3];

    TSwapAs = TSwapBs;

    TSwapBs[0] = Temps[TSwapTemp1];
    TSwapBs[1] = Temps[TSwapTemp2];
    TSwapBs[2] = Temps[TSwapTemp3];
    TSwapBs[3] = Temps[TSwapTemp4];
}
void CMMTBracketThread::TSwap(int8& TSwapA, int8& TSwapB)
{
    Temps[TSwapTemp0] = TSwapA;
    TSwapA = TSwapB;
    TSwapB = Temps[TSwapTemp0];
}
