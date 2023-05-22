// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EchoOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEchoOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar) {
		HealthProgressBar->SetPercent(Percent);
	}
}

void UEchoOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar) {
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UEchoOverlay::SetGold(int32 Gold)
{
	if (GoldCountText) {
		FText FTextGold = FText::FromString(FString::Printf(TEXT("%d"), Gold));
		GoldCountText->SetText(FTextGold);
	}
}

void UEchoOverlay::SetSouls(int32 Souls)
{
	if (SoulCountText) {
		FText FTextSouls = FText::FromString(FString::Printf(TEXT("%d"), Souls));
		SoulCountText->SetText(FTextSouls);
	}
}
