// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "tutorialgamePlayerHUD.h"
#include "Components/ProgressBar.h"

void UtutorialgamePlayerHUD::SetStamina(float Val, float Max) {

	if (StaminaBar)
	{
		StaminaBar->SetPercent(Val/Max);
	}
};

