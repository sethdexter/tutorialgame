// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "tutorialgamePlayerHUD.generated.h"


UCLASS(Abstract)
class TUTORIALGAME_API UtutorialgamePlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetStamina(float Val, float Max);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
		class UProgressBar* StaminaBar;
};

