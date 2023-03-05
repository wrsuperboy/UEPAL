// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"

#ifdef UE_BUILD_DEBUG
#define PAL_DebugMsg(Message) \
GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.f, FColor::White, Message)
#else
#define PAL_DebugMsg(Message)
#endif

#define SQRT_3 1.732050807569

#define PIXEL_TO_UNIT 4.

constexpr float UI_PIXEL_TO_UNIT = 3.;

constexpr float FRAME_TIME = 0.1f;

enum class EGameDistribution : uint8
{
	DOS,
	Win95
};

struct FPALPosition2d
{
	double X;
	double Y;

	FPALPosition2d(double InX, double InY) : X(InX), Y(InY) {}
};

struct FPALPosition3d
{
	double X;
	double Y;
	double Z;

	FPALPosition3d() : X(0), Y(0), Z(0) {}

	FPALPosition3d(double InX, double InY, double InZ) : X(InX), Y(InY), Z(InZ) {}

	FPALPosition2d to2d() const
	{
		return FPALPosition2d(X, Y / 2. - Z * SQRT_3 / 2.);
	}

	FVector3d toLocation() const
	{
		return FVector3d(X * PIXEL_TO_UNIT, Y * PIXEL_TO_UNIT, Z * PIXEL_TO_UNIT);
	}
};

// maximum items in a store
#define MAX_STORE_ITEM 9

// maximum number of enemies in a team
#define MAX_ENEMIES_IN_TEAM 5

// total number of possible player roles
#define MAX_PLAYER_ROLES 6

// maximum number of equipments for a player
#define MAX_PLAYER_EQUIPMENTS 6

// total number of magic attributes
#define NUM_MAGIC_ELEMENTAL 5

// maximum number of magics for a player
#define MAX_PLAYER_MAGICS 32

// maximum number of scenes
#define MAX_SCENES 300

// maximum number of objects
#define MAX_OBJECTS 600

// totally number of playable player roles
#define MAX_PLAYABLE_PLAYER_ROLES 5

// maximum number of level
#define MAX_LEVELS 99

// maximum number of effective poisons to players
#define MAX_POISONS 16

// maximum number of event objects (should be somewhat more than the original,
// as there are some modified versions which has more)
#define MAX_EVENT_OBJECTS  5500

// maximum entries of inventory
#define MAX_INVENTORY 256