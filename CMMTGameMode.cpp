// Fill out your copyright notice in the Description page of Project Settings.


#include "CMMTGameMode.h"

#include "CMMTNullPlayerController.h"
#include "CMMTMainCamera.h"
#include "UObject/ConstructorHelpers.h"

ACMMTGameMode::ACMMTGameMode() {
	PlayerControllerClass = ACMMTNullPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_MainCamera"));
	if (PlayerPawnBPClass.Class != nullptr) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ACMMTGameMode::BeginPlay()
{
	BracketManager = NewObject<UCMMTBracketManager>(this, UCMMTBracketManager::StaticClass());
}
