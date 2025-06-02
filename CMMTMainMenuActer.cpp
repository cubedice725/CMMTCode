// Fill out your copyright notice in the Description page of Project Settings.


#include "CMMTMainMenuActer.h"

#include "CMMTMainMenu.h"
#include "Blueprint/UserWidget.h"

//ACMMTMainMenuActer::ACMMTMainMenuActer()
//{
// 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = false;
//
//	static ConstructorHelpers::FClassFinder<UCMMTMainMenu> HUD(TEXT("WidgetBlueprint'/Game/Blueprints/WB_MainMenu.WB_MainMenu_C'"));
//	if (HUD.Succeeded())
//	{
//		HUDClass = HUD.Class;
//	}
//
//}
//
//void ACMMTMainMenuActer::BeginPlay()
//{
//	Super::BeginPlay();
//	/*if (HUDClass)
//	{
//		UE_LOG(LogTemp, Error, TEXT("%s"), *HUDClass->GetName());
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("HUDClass is nullptr"));
//	}*/
//	CreateHUD();
//}
//
//void ACMMTMainMenuActer::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}
//
//void ACMMTMainMenuActer::CreateHUD()
//{
//	// GetWorld()->GetFirstPlayerController()은 첫번째 컨트롤러만 반환하므로 멀티에 적합하지 않음
//	if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
//	{
//		HUDWidget = CreateWidget<UUserWidget>(playerController, HUDClass);
//		if (HUDWidget)
//		{
//			HUDWidget->AddToViewport();
//
//			playerController->SetInputMode(FInputModeGameOnly());
//			playerController->bShowMouseCursor = true;
//		}
//	}
//}
//
