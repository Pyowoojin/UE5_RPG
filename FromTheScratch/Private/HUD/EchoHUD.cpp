// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/EchoHUD.h"
#include "HUD/EchoOverlay.h"

void AEchoHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && EchoOverlayClass) {
			EchoOverlay = CreateWidget<UEchoOverlay>(Controller, EchoOverlayClass);
			EchoOverlay->AddToViewport();
		}
	}
}
