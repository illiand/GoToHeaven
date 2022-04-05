// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GOTOHEAVEN_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void changeWBackgroundAlpha(float a);

	UFUNCTION(BlueprintImplementableEvent)
	void changeBBackgroundAlpha(float a);

	UFUNCTION(BlueprintImplementableEvent)
	void changeTextAlpha(float a);
};
