// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CMMTBracketManager.h"
#include "CMMTMainMenu.h"
#include "CMMTGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CMMT_API ACMMTGameMode : public AGameMode
{
	GENERATED_BODY()

	ACMMTGameMode();
public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "CFun")
	void SetMainMenu(UCMMTMainMenu* InMainMenu) { MainMenu = InMainMenu; };
	
	UFUNCTION(BlueprintCallable, Category = "CFun")
	UCMMTMainMenu* GetMainMenu() { return MainMenu; };
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCMMTBracketManager* BracketManager = NULL;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCMMTMainMenu* MainMenu = NULL;
};
