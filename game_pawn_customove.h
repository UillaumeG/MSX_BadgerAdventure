// Library GAME_PAWN_CUSTOMOVE
// because of the isometric render I need to adapt the hit boxes on sprite's moves  
//     > I want to keep the benefit of the game_pawn library but overide the physics
//     > hitbox with background tiles will refer to the feet of my sprite 
//     > then the head of the sprite may overlap some tiles of the background
// Uillaume 2025-06-07 creation

#pragma once
#include "core.h"
#include "game_pawn.h"


// GAMEPAWN_USE_PHYSICS
#if (GAMEPAWN_USE_PHYSICS == TRUE)
	#warning GAMEPAWN_USE_PHYSICS is set to TRUE in "msxgl_config.h"! Make your choice between physics from GAME_PAWN and GAME_PAWN_CUSTOMOVE
#endif

/* ----------------------------------------------------------------------------
 VARIABLES HONTEUSEMENT POMPEES DE LA LIBRARY GAME_PAWN
 POUR FAIRE TOURNER LE CODE LUI AUSSI COPIE (/me sifflotte)
---------------------------------------------------------------------------- */

//#if (GAMEPAWN_USE_PHYSICS)
// Current cell coordinate
extern u8 g_Game_CellX;
extern u8 g_Game_CellY;
//#endif

// UILL 2025-06-07, FORCE and/or OVERWRITE to fit in the code
#ifndef PAWN_UPDATE_COLLISION
	#define PAWN_UPDATE_COLLISION '0b00010000'
#endif
#ifndef PAWN_UPDATE_PHYSICS
	#define PAWN_UPDATE_PHYSICS   '0b00100000'
#endif

#ifdef GET_BOUND_Y
	#undef GET_BOUND_Y
	#if (GAMEPAWN_BOUND_Y == GAMEPAWN_BOUND_CUSTOM)
		#define GET_BOUND_Y		g_PawnCusto->BoundY
	#else
		#define GET_BOUND_Y		GAMEPAWN_BOUND_Y
	#endif
#endif
#ifdef GET_BOUND_X
	#undef GET_BOUND_X
	#if (GAMEPAWN_BOUND_X == GAMEPAWN_BOUND_CUSTOM)
		#define GET_BOUND_X		g_PawnCusto->BoundX
	#else
		#define GET_BOUND_X		GAMEPAWN_BOUND_X
	#endif
#endif

//#if (GAMEPAWN_USE_PHYSICS)
// Physics callback
typedef void (*Game_PhysicsCB)(u8 event, u8 tile);

// Collision callback
typedef bool (*Game_CollisionCB)(u8 tile);

// Pawn physics events
enum PAWN_PHYSICS_EVENT
{
	// Collision
	PAWN_PHYSICS_COL_DOWN  = 0,
	PAWN_PHYSICS_COL_UP,
	PAWN_PHYSICS_COL_RIGHT,
	PAWN_PHYSICS_COL_LEFT,
	// Physics
	PAWN_PHYSICS_FALL,
	// Border
	#if (GAMEPAWN_BORDER_EVENT || GAMEPAWN_BORDER_BLOCK)
		PAWN_PHYSICS_BORDER_DOWN,
		PAWN_PHYSICS_BORDER_UP,
		PAWN_PHYSICS_BORDER_RIGHT,
		PAWN_PHYSICS_BORDER_LEFT,
	#endif
};

// Pawn physics states
enum PAWN_PHYSICS_STATE
{
	PAWN_PHYSICS_INAIR     = 0b00000001, // Grounded overwise
};
//#endif

// Pawn structure
typedef struct
{
	Game_Pawn	Pawn;		// List of sprite layers
	// CONTENT of GAME_PAWN
	//	const Game_Sprite*	SpriteList;		// List of sprite layers
	//	u8					SpriteNum;		// Number of sprite layers
	//	const Game_Action*	ActionList;		// List of actions
	//	u8					PositionX;		// Pawn screen position
	//	u8					PositionY;
	//	u8					ActionId;		// Current action id
	//	u8					AnimFrame;		// Current animation frame id
	//	u8					AnimStep;		// Current step into the animation
	//	u8					AnimTimer;		// Animation timer (into the current step)
	//	u8					Update;			// Pawn update flags
	//	u8					Counter;		// Global update counter
	
	// ATTRIBUTES RECONDUCTED for CUSTOMIZED PHYSICS 
	i8					MoveX;			// Pawn movement offset
	i8					MoveY;
	u8					BoundX;			// Pawn collision bound
	u8					BoundY;
	u8					PhysicsState;
	Game_PhysicsCB		PhysicsCB;
	Game_CollisionCB	CollisionCB;
} Game_Pawn_CustoMove;


void GamePawnCusto_InitializePhysics(Game_Pawn_CustoMove* pawn, Game_PhysicsCB pcb, Game_CollisionCB ccb, u8 boundX, u8 boundY);

void GamePawnCusto_SetMovement(Game_Pawn_CustoMove* pawn, i8 dx, i8 dy);

void GamePawnCusto_Update (Game_Pawn_CustoMove* pawn);
//
// <<< à lancer APRES GamePawn_Update >>>
// 