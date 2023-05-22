// Copyright Epic Games, Inc. All Rights Reserved.

#include "FromTheScratchGameMode.h"
#include "FromTheScratchCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFromTheScratchGameMode::AFromTheScratchGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
