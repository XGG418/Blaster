// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void MenuSetup();
	void MenuTearDown();

protected:
	virtual bool Initialize() override;

	//回调函数
	UFUNCTION()
	void OnDestroySession(bool bWasSuucessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:
	UPROPERTY(meta=(BindWidget))
	class UButton* ReturnButton;

	//将绑定到一个动态委托
	UFUNCTION()
	void ReturnButtonClicked();

	UPROPERTY()
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	class APlayerController* PlayerController;
};
