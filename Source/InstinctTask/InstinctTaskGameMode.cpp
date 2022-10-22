// Copyright Epic Games, Inc. All Rights Reserved.

#include "InstinctTaskGameMode.h"
#include "InstinctTaskBall.h"

AInstinctTaskGameMode::AInstinctTaskGameMode()
{
	// set default pawn class to our ball
	DefaultPawnClass = AInstinctTaskBall::StaticClass();
}
