// Fill out your copyright notice in the Description page of Project Settings.


#include "CMMTBracketManager.h"
#include "CMMTPEntryData.h"
#include "CMMTBracketWidget.h"

void UCMMTBracketManager::SpawnNormalTableActors(TSoftObjectPtr<UWorld> World)
{
    ClearNormalTableActors();

    if (!ListViewCount) { return; }

    int32 TotalTables = ListViewCount / 4;

    // 메시 로드
    UStaticMesh* NormalTableMesh = LoadObject<UStaticMesh>(
        nullptr,
        TEXT("/Game/Mahjong/NormalTable.NormalTable")
    );
    if (!NormalTableMesh) return;

    // 기준점 설정
    const FVector StartPoint = FVector(-70.0f, 1938.0f, 160.0f);
    const FVector EndPoint = FVector(-2710.0f, 4780.0f, 160.0f);

    // 최적의 행/열 계산 (최대한 정사각형에 가깝게)
    int32 Columns = FMath::FloorToInt(FMath::Sqrt(static_cast<float>(TotalTables)));
    while (Columns > 0 && TotalTables % Columns != 0) {
        Columns--;
    }
    if (Columns == 0) Columns = 1;
    const int32 Rows = TotalTables / Columns;

    // 좌표 생성
    TArray<FVector> Locations;
    for (int32 Col = 0; Col < Columns; ++Col)
    {
        const float AlphaX = (Columns > 1) ? (float)Col / (Columns - 1) : 0.0f;
        const float X = FMath::Lerp(StartPoint.X, EndPoint.X, AlphaX);

        for (int32 Row = 0; Row < Rows; ++Row)
        {
            const float AlphaY = (Rows > 1) ? (float)Row / (Rows - 1) : 0.0f;
            const float Y = FMath::Lerp(StartPoint.Y, EndPoint.Y, AlphaY);

            Locations.Add(FVector(X, Y, StartPoint.Z));
        }
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    // 액터 생성
    for (int32 Index = 0; Index < Locations.Num(); ++Index)
    {
        if (TableActors.Num() == Index)
        {
            TableActors.Add(World->SpawnActor<AStaticMeshActor>(Locations[Index], FRotator::ZeroRotator, Params));
            TableActors.Top()->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
            TableActors.Top()->GetStaticMeshComponent()->SetStaticMesh(NormalTableMesh);
        }
        else
        {
            TableActors[Index]->SetActorLocation(Locations[Index]);
        }
    }
}

void UCMMTBracketManager::ClearNormalTableActors()
{
    for (AStaticMeshActor* Table : TableActors)
    {
        Table->SetActorLocation(FVector(0, 0, -1000));
    }
}

void UCMMTBracketManager::ListViewToMap(const UListView* ListView)
{
    ListViewCount = ListView->GetNumItems();
    TMap<FString, TArray<FString>> Map = PDatasToMap(ListViewToPDatas(ListView));
    PTMap = Map;
}

void UCMMTBracketManager::CreateBracketThread()
{
    /*UE_LOG(LogTemp, Warning, TEXT("테스트 값 넣음"));
    PTMap.Empty();
    PTMap.Add(TEXT("Affiliation1"), { TEXT("Name0"), TEXT("Name1"), TEXT("Name2"), TEXT("Name3") });
    PTMap.Add(TEXT("Affiliation2"), { TEXT("Name4"), TEXT("Name5"), TEXT("Name6"), TEXT("Name7") });
    PTMap.Add(TEXT("Affiliation3"), { TEXT("Name8"), TEXT("Name9"), TEXT("Name10"), TEXT("Name11") });
    PTMap.Add(TEXT("Affiliation4"), { TEXT("Name12"), TEXT("Name13"),TEXT("Name14"), TEXT("Name15")});
    PTMap.Add(TEXT("Affiliation5"), { TEXT("Name16"), TEXT("Name17"), TEXT("Name18") ,TEXT("Name19") });
    PTMap.Add(TEXT("Affiliation6"), {});

    PTMap.Add(TEXT("Affiliation7"), {  });
    PTMap.Add(TEXT("Affiliation8"), {  });

    PTMap.Add(TEXT("Affiliation9"), {  });
    PTMap.Add(TEXT("Affiliation10"), {  });
    
    PTMap.Add(TEXT("Affiliation11"), { TEXT("Name21"), TEXT("Name22") });
    PTMap.Add(TEXT("Affiliation12"), { TEXT("Name23"), TEXT("Name24") }); 
    PData = 3;
    AData = 4;
    TData = 0;
    RoundData = 6;
    PDupCheckData = true;*/
    
    Worker = new CMMTBracketThread(PTMap, PData, AData, TData, RoundData, PDupCheckData, MapToFString(PTMap));
    IsBracketThreadStart = true;
}

void UCMMTBracketManager::StopBracketThread()
{
    if (Worker != NULL)
    {
        delete Worker;
    }
    IsFailedCreateBracket = false;
    IsBracketThreadStart = false;
}
void UCMMTBracketManager::CreateBracketUserWidget(UCMMTBracketWidget* BracketUserWidget)
{
    if (IsBracketThreadStart && Worker->ThreadEndCheck())
    {
        IsBracketThreadStart = false;
        IsCreateBracketUserWidgetEnd = true;

        Bracket = Worker->GetBracket();
        if (Bracket.Num() < RoundData)
        {
            IsFailedCreateBracket = true;
        }
        BracketUserWidget->CreateBracketUserWidget(Bracket, MapToFString(PTMap), TData);
    }
}

FString UCMMTBracketManager::InputInformation(int64 MaxValue, int64 MinValue, int64 PValue, int64 AValue, int64 TableValue, int64 RoundValue, bool PDupCheckValue)
{
    if (PDupCheckValue && (PValue > MaxValue || PValue < MinValue)) { return TEXT("참가자 중복 값이 올바르지 않습니다."); }
    if (AValue > 4 || AValue < 1) { return TEXT("소속 중복 값이 올바르지 않습니다."); }
    if (TableValue > ListViewCount / 4 || TableValue < 0) { return TEXT("지정 테이블 값이 올바르지 않습니다.."); }
    if (RoundValue > MaxValue || RoundValue < MinValue) { return TEXT("라운드 값이 올바르지 않습니다.."); }
    if (ListViewCount % 4 != 0) { return FString::Printf(TEXT("참가자가 %d명 더 필요합니다."), 4 - ListViewCount % 4); }
    
    PData = PValue;
    AData = AValue;
    TData = TableValue;
    RoundData = RoundValue;
    PDupCheckData = PDupCheckValue;
    
    return TEXT("");
}

TArray<FString> UCMMTBracketManager::MapToFString(const TMap<FString, TArray<FString>> Map)
{
    TArray<FString> Participants;
    for (const TPair<FString, TArray<FString>>& Elem : Map)
    {
        for (const FString str : Elem.Value)
        {
            Participants.Add(Elem.Key +" "+ str);
        }
    }

    return Participants;
}

TArray<FCMMTPData> UCMMTBracketManager::ListViewToPDatas(const UListView* ListView)
{
    TArray<UObject*> ListItems;
    TArray<FCMMTPData> PDatas;

    ListItems = ListView->GetListItems();
    for (UObject* ListItem : ListItems)
    {
        UCMMTPEntryData* ItemData = Cast<UCMMTPEntryData>(ListItem);
        PDatas.Add(ItemData->GetEntryInfo());
    }

    return PDatas;
}

TMap<FString, TArray<FString>> UCMMTBracketManager::PDatasToMap(const TArray<FCMMTPData>& PDatas)
{
    TMap<FString, TArray<FString>> Map;

    for (const FCMMTPData& Data : PDatas)
    {
        FString FSPData = Data.Affiliation.ToString();
        if (Map.Contains(FSPData))
        {
            Map[FSPData].Add(Data.PName.ToString());
        }
        else
        {
            Map.Add(FSPData, { Data.PName.ToString() });
        }
    }

    return Map;
}