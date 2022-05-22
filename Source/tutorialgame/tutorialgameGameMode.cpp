// Copyright Epic Games, Inc. All Rights Reserved.

#include "tutorialgameGameMode.h"
#include "tutorialgameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AtutorialgameGameMode::AtutorialgameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/NewBlueprint"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
