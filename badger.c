///////////////////////////////////////////////////////////////////////////////////
// ▄▄          ▄                █        ▄▄     ▄                ▄
// █▄█ ▄▀██ ▄▀██ ▄▀██ ▄███ █▄▀▀  ▄█▀▀    █▄█ ▄▀██ █ ██ ▄███ █▄▀█ █▀▀ █ ██ █▄▀▀ ▄███
// █▄█ ▀▄██ ▀▄██ ▀▄██ ▀█▄▄ █     ▄▄█▀    █ █ ▀▄██  ██  ▀█▄▄ █  █ ▀▄▄ ▀▄█▀ █    ▀█▄▄
//                 ▄▀
// "Guide our hero in his trip"
// A project by UILLAUME based on MSXgl by AOINEKO
///////////////////////////////////////////////////////////////////////////////////


/* *** TODO ***

9-nov-2024 deplacement en diagonale du sprite-joueur (precedemment haut/bas ou gauche/droite)
dec-2024 Ecran de debut/titre/credits DONE
dec-2024 Ajout musique DONE
jan-2025 Ajout de niveaux DONE
jan-2025 Tuiles dans build_data DONE
jan-2025 Using states from Game ; FSM not used anymore
jan-2025 More states, better timing of VRAM updates
jan-2025 chargements tiles & sprites mis dans dataLoad (state #0)
jan-2025 écran fin de niveau
jan-2025 fix detection fin de niveau 

correction deplacements
finir ennemi rocher
creer ennemi aigle 
ajouter bruitages
créer un menu > jouer 
ajouter joystick
affichage depuis 10 bytes / compression pletter des niveaux
créer stages speciaux : tous les 10 niveaux décor féérique+password
ajouter un menu > pass 
ajouter un menu > langue

*/

//=============================================================================
// INCLUDES
//=============================================================================
// MSX GL "game library"
#include "msxgl.h"
#include "arkos/akg_player.h"
#include "debug.h"
#include "game.h"
#include "game_pawn.h"
#include "game_pawn_customove.h"
#include "input.h"
#include "compress/pletter.h"

//=============================================================================
// DEFINES
//=============================================================================

// Music address
#define MUSIC_ADDRESS 0xD000
//#define SFX_ADDRESS 0xE000

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Font
#include "font/font_mgl_sample8.h"
// Music
#include "content/sound/g_AKG_Test1.h"

// Tiles and Levels
#include "content/img/data_tile_background.h"
#include "content/data_level.h"

// Sprites
#include "content/img/data_sprite_badger.h"
#include "content/img/data_sprite_boulder.h"
//#include "content/img/data_sprite_cristal.h"
//#include "content/img/data_sprite_flower.h"

// Sign-of-life character animation data
const c8 g_ChrAnim[] = { '|', '\\', '-', '/' };

//=============================================================================
// Include GAME
//=============================================================================
// STATES
// Data >> Splash screen >> IntroLevel >> PlayLevel >> End
//                                       >> Pause >>

// Function prototypes
// Data
bool State_DataLoad();
// Title
bool State_SplashDisplay();
bool State_SplashWait();
// Game 
bool State_GameLevelInit();
//bool State_GameLevelStart();
bool State_GameLevelPlay();
	//bool State_GameLevelPause();
bool State_GameLevelEnd();

///////////////////////////////////////////////////////////////////////////////
// PLAYER sprites
// 

// Pawn sprite layers
const Game_Sprite g_PlayerSpriteLayers[] =
{//   X  Y  Pattern Color            Option
	{ 0, 0, 0,      COLOR_WHITE,     0 }, 
	{ 0, 0, 4,      COLOR_BLACK,     0 }
};

// Idle animation frames
const Game_Frame g_FramesPlayerIdle[] =
{ //  Pattern Frames Function
	{ 0*8,	  48,    NULL },
	{ 1*8,	  24,    NULL },
};

// Move animation frames
const Game_Frame g_FramesPlayerMoveRight[] =
{
	{ 2*8,	6,	NULL },
	{ 3*8,	6,	NULL },
};
const Game_Frame g_FramesPlayerMoveLeft[] =
{
	{ 4*8,	6,	NULL },
	{ 5*8,	6,	NULL },
};
const Game_Frame g_FramesPlayerMoveUp[] =
{
	{ 8*8,	6,	NULL },
	{ 9*8,	6,	NULL },
};
const Game_Frame g_FramesPlayerMoveDown[] =
{
	{10*8,	6,	NULL },
	{11*8,	6,	NULL },
};
const Game_Frame g_FramesPlayerClimb[] =
{
	{ 6*8,	8,	NULL },
	{ 7*8,	8,	NULL },
};
const Game_Frame g_FramesPlayerHang[] =
{
	{ 6*8,	99,	NULL },
};
const Game_Frame g_FramesPlayerPain[] =
{
	{ 12*8,	30,	NULL },
	{ 13*8,	30,	NULL },
	{ 12*8,	30,	NULL },
	{ 13*8,	30,	NULL },
	{ 12*8,	30,	NULL },
	{ 13*8,	30,	NULL },
};
const Game_Frame g_FramesPlayerDance[] =
{
	{ 14*8,	40,	NULL },
	{ 15*8,	40,	NULL },
};

// Actions id
enum ANIM_PLAYER_ACTION_ID
{
	PLAYER_ACTION_IDLE        = 0,
	PLAYER_ACTION_MOVE_RIGHT  ,
	PLAYER_ACTION_MOVE_LEFT   ,
	PLAYER_ACTION_MOVE_UP     ,
	PLAYER_ACTION_MOVE_DOWN   ,
	PLAYER_ACTION_CLIMB       ,
	PLAYER_ACTION_HANG        ,
	PLAYER_ACTION_PAIN        ,
	PLAYER_ACTION_DANCE
};

// List of all player actions
const Game_Action g_AnimActions[] =
{ //  Frames             Number                       				Loop?	Interrupt?
	{ g_FramesPlayerIdle,      numberof(g_FramesPlayerIdle),     	TRUE,	TRUE },
	{ g_FramesPlayerMoveRight, numberof(g_FramesPlayerMoveRight),	TRUE,	TRUE },
	{ g_FramesPlayerMoveLeft,  numberof(g_FramesPlayerMoveLeft), 	TRUE,	TRUE },
	{ g_FramesPlayerMoveUp,    numberof(g_FramesPlayerMoveUp),   	TRUE,	TRUE },
	{ g_FramesPlayerMoveDown,  numberof(g_FramesPlayerMoveDown), 	TRUE,	TRUE },
	{ g_FramesPlayerClimb,     numberof(g_FramesPlayerClimb),    	TRUE,	TRUE },
	{ g_FramesPlayerHang,      numberof(g_FramesPlayerHang),     	TRUE,	TRUE },
	{ g_FramesPlayerPain,      numberof(g_FramesPlayerPain),     	FALSE,	FALSE },
	{ g_FramesPlayerDance,     numberof(g_FramesPlayerDance),     	TRUE,	FALSE },
};

///////////////////////////////////////////////////////////////////////////////
// BOULDER sprites
//
// Pawn sprite layers

const Game_Sprite g_BoulderSpriteLayers[] =
{//   X  Y  		Pattern Color		Option
	{ 0, 0, 0,      COLOR_DARK_RED,		0 }, 
	{ 0, 0, 4,      COLOR_MEDIUM_RED,	0 }
};
const Game_Frame g_FramesBoulderRollLeft[] =
{
	{16*8,	4,	NULL },
	{17*8,	4,	NULL },
	{18*8,	4,	NULL },
	{19*8,	4,	NULL },
};
// Actions id
enum ANIM_BOULDER_ACTION_ID
{
	BOULDER_ACTION_FALL_RIGHT  = 0,
	BOULDER_ACTION_FALL_LEFT   ,
	BOULDER_ACTION_ROLL_RIGHT  ,
	BOULDER_ACTION_ROLL_LEFT   ,
};
const Game_Action g_AnimActions_Boulder[] =
{ //  Frames             			Number                       			Loop? Interrupt?
	{ g_FramesBoulderRollLeft,      numberof(g_FramesBoulderRollLeft),      TRUE, TRUE },
	{ g_FramesBoulderRollLeft,      numberof(g_FramesBoulderRollLeft),      TRUE, TRUE },
	{ g_FramesBoulderRollLeft,      numberof(g_FramesBoulderRollLeft),      TRUE, TRUE },
	{ g_FramesBoulderRollLeft,      numberof(g_FramesBoulderRollLeft),      TRUE, TRUE },
};

// ///////////////////////////////////////////////////////////////////////////////
// // FLOWER sprites
// //
// // Pawn sprite layers
// 
// const Game_Sprite g_FlowerSpriteLayers[] =
// {//   X  Y  		Pattern Color		Option
// 	{ 0, 0, 0,      COLOR_WHITE,		0 }, 
// };
// const Game_Frame g_FramesFlowerJiggle[] = // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX A METTRE A JOUR XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// {
// 	{20*8,		4,	NULL },
// 	{20*8+4,	4,	NULL },
// 	{20*8+4*2,	4,	NULL },
// 	{20*8+4*3,	4,	NULL },
// };
// // Actions id
// enum ANIM_FLOWER_ACTION_ID
// {
// 	FLOWER_ACTION_JIGGLE  = 0
// };
// const Game_Action g_AnimActions_Flower[] =
// { //  Frames             			Number                       			Loop? Interrupt?
// 	{ g_FramesFlowerJiggle,      numberof(g_FramesFlowerJiggle),      TRUE, TRUE },
// 	{ g_FramesFlowerJiggle,      numberof(g_FramesFlowerJiggle),      TRUE, TRUE },
// 	{ g_FramesFlowerJiggle,      numberof(g_FramesFlowerJiggle),      TRUE, TRUE },
// 	{ g_FramesFlowerJiggle,      numberof(g_FramesFlowerJiggle),      TRUE, TRUE },
// };

//=============================================================================
// MEMORY DATA
//=============================================================================

Game_Pawn_CustoMove g_PlayerPawn;
Game_Pawn_CustoMove g_BoulderPawn;
Game_Pawn_CustoMove g_FlowerPawn;

//bool g_bFlicker = TRUE;
//bool g_bJumping = FALSE;
//i8   g_VelocityY;

// related to PLAYER
bool g_bPlayerFlagEvent_Moving = FALSE; // TRUE when sprite-player in action
u8   g_bPlayer_Direction;               // where is the sprite-player going? KEY_RIGHT,KEY_LEFT,KEY_UP,KEY_DOWN
bool g_bPlayerFlagEvent_Ladder = FALSE; // TRUE when sprite-player on ladder = climbing
i8   g_DX = 0;
i8   g_DY = 0;
u8   g_CatchBoulderEvent = 0;
// related to GAME LOGIC
u8   g_PrevRow8 = 0xFF;
u16  g_Level = 1; // Start at level 1
// related to ennemies
bool g_BoulderHidden;
u8   g_Boulder_NewActionId;
u8   g_Flower_NewActionId;
// related to GOAL-tile
u8   g_Goal1_PositionX, g_Goal1_PositionY; // goal coordinates on level

//=============================================================================
// STATE >> DATA LOAD
//=============================================================================
bool State_DataLoad()
{
	// Initialize display
	VDP_EnableDisplay(FALSE);
	VDP_SetMode(VDP_MODE_GRAPHIC1); // Initialize screen grpahic 1
	VDP_ClearVRAM();
    // Fill screen with spaces
	VDP_FillVRAM_16K ((u8)' ', g_ScreenLayoutLow, 32*24);
	
	// Init Text
	Print_Initialize();
//	Print_SetTextFont(PRINT_DEFAULT_FONT, 1); // Initialize font (use BIOS font)
	Print_SetTextFont(g_Font_MGL_Sample8, 32);

	// Load background tiles (over first characters of font)
	VDP_WriteVRAM (g_DataTileBackground_Patterns, g_ScreenPatternLow, 0, sizeof(g_DataTileBackground_Patterns));
	VDP_WriteVRAM (g_DataTileBackground_Colors, g_ScreenColorLow, 0, sizeof(g_DataTileBackground_Colors));

	// Load sprites
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	VDP_LoadSpritePattern(g_DataSprtLayer, 0, 16*4*2); // Player = Badger
	VDP_LoadSpritePattern(g_DataBoulderLayer, 0+16*4*2, 8*4*2);	// Boulder
// 	VDP_LoadSpritePattern(g_DataFlowerLayer, 0+16*4*2+8*4*2, 4*4*1); // Flower
	
	// Music
	Pletter_UnpackToRAM(g_AKG_Test1, (void*)MUSIC_ADDRESS);
	AKG_Init((const void*)MUSIC_ADDRESS, 0);

	// Go to next state
	Game_SetState(State_SplashDisplay);
	return TRUE; // Frame finished
} // State_DataLoad


//=============================================================================
// STATE = SPLASH SCREEN
//=============================================================================
bool State_SplashDisplay()
{
	// Video mode
	VDP_EnableDisplay(FALSE);
	VDP_SetColor2 (COLOR_DARK_GREEN, COLOR_WHITE); // green background, white text

	// Display
	Print_DrawTextAt (0, 1, "       BADGER'S ADVENTURE");
//	Print_DrawLineH  (0, 2, 32);
	Print_DrawTextAt (0, 3, "   Guide our hero in his trip");
	Print_DrawTextAt (0, 5, "         <press SPACE>");
	
	// Credits
	Print_DrawTextAt (0, 14, "   !!! WORK IN PROGRESS !!!");
	Print_DrawTextAt (0, 20, "   code UILLAUME, music CRIPS");
	Print_DrawTextAt (0, 21, "  thanks AOINEKO, JULIEN NEVO,");
	Print_DrawTextAt (0, 22, "     MSXVILLAGE, ASSO MO5");
	
	VDP_EnableDisplay(TRUE);
	
	// Init done --> next state SplashUpdate
	Game_SetState (State_SplashWait);

	return TRUE; // Frame finished
} // State_SplashDisplay
bool State_SplashWait()
{
	// Text
	Print_DrawCharAt (31, 0, g_ChrAnim[g_GameFrame & 0x03]);
	
	// Music
	AKG_Decode ();

	// Keyboard
	if (Keyboard_IsKeyPressed(KEY_SPACE)) Game_SetState(State_GameLevelInit);

	return TRUE; // Frame finished
} // State_SplashWait

///////////////////////////////////////////////////////////////////////////////
// PLAYER physics

// PLAYER, Physics callback
void PhysicsEvent_Player(u8 event, u8 tile)
{
	tile;
	event;
}

// PLAYER, Collision callback
bool PhysicsCollision_Player(u8 tile)
{
	switch (tile)
	{
		// Walls
		case 7:  return true; // wall
		case 15: return true; // wall
		// Corners on platform
		case 0 : return true;  
		case 5 : return true; 
		case 10: return true; 
		case 11: return true; 
		// ladders
		case 6:  g_bPlayerFlagEvent_Ladder=true; return false;
		case 14: g_bPlayerFlagEvent_Ladder=true; return false;
	}
	
	// Left screen border
	if (tile >=64) return true;
	
	// all other cases
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// BOULDER physics

//// BOULDER hide
void BoulderHide ()
{
	g_BoulderHidden = true;
	GamePawn_SetPosition(&(g_BoulderPawn.Pawn), 0, 0); // avoid collision with player-sprite
	GamePawn_Disable (&(g_BoulderPawn.Pawn));
} // BoulderHide

//// BOULDER show
void BoulderShow (int posx, int posy)
{
	g_BoulderHidden = false;
	GamePawn_SetPosition(&(g_BoulderPawn.Pawn), posx, posy);
} // BoulderShow

//// BOULDER, Physics callback
void PhysicsEvent_Boulder(u8 event, u8 tile)
{
	tile;
	event;
	switch(event)
	{
//		// catch event when a border is reached
		case PAWN_PHYSICS_BORDER_DOWN: BoulderHide (); break;
//		case PAWN_PHYSICS_BORDER_RIGHT:
//		case PAWN_PHYSICS_BORDER_LEFT:
//			g_CatchBoulderEvent = event;
	};
}

// BOULDER, Collision callback
bool PhysicsCollision_Boulder(u8 tile)
{
	switch (tile) // roll in grass / fall along walls
	{
		case 16: // grass 
		case 17: // grass 
			if (g_BoulderPawn.MoveX>0) g_Boulder_NewActionId=BOULDER_ACTION_ROLL_RIGHT;
			else g_Boulder_NewActionId = BOULDER_ACTION_ROLL_LEFT;
			return false;
		case  0: // front grass limit
		case  1: // front grass limit
		case  2: // front grass limit
		case  8: // front grass limit
		case  9: // front grass limit
		case 10: // front grass limit
			if (g_BoulderPawn.MoveX>0) g_Boulder_NewActionId=BOULDER_ACTION_FALL_RIGHT;
 			else g_Boulder_NewActionId = BOULDER_ACTION_FALL_LEFT;
			return false;
		case  3: // back grass limit
		case  4: // back grass limit
		case  5: // back grass limit
			g_Boulder_NewActionId = BOULDER_ACTION_FALL_LEFT;
		case 11: // back grass limit
		case 12: // back grass limit
		case 13: // back grass limit
			g_Boulder_NewActionId = BOULDER_ACTION_FALL_RIGHT;
		default: return false;
	}
}

//-----------------------------------------------------------------------------
//
void DrawLevel(u8 levelnum) 
{
	// Read level from g_DataLevel[] from data_level.h
	VDP_WriteVRAM_16K (&g_DataLevel[levelnum*32*24], g_ScreenLayoutLow, 32*24);
	
	// Initialize pattern
//	Print_SetTextFont(g_Font_MGL_Sample8, 32);
//	Print_SetColor(0xF, 0x1);
	VDP_SetColor(COLOR_BLACK);  
	VDP_WriteVRAM (g_DataTileBackground_Colors, g_ScreenColorLow, 0, sizeof(g_DataTileBackground_Colors));

	// Title
	Print_DrawTextAtV ( 0,  0, "BADGER ADVENTURE");
	Print_DrawTextAt  ( 0, 18, "Lv");
//	levelnum += (int) '0';
	Print_SetPosition ( 0, 19);
	Print_DrawChar    ( levelnum/10 +(int) '0' );
	Print_SetPosition ( 1, 19);
	Print_DrawChar    ( levelnum%10 +(int) '0' );
	
	// Locate goal-tile
	for (int i=0; i<32; i++)
		for (int j=0; j<24; j++)
			if (g_DataLevel[levelnum*32*24+i+j*32]>23 && g_DataLevel[levelnum*32*24+i+j*32]<32)
			{
				g_Goal1_PositionX = i;
				g_Goal1_PositionY = j;
				return;
			}
} // void DrawLevel

//-----------------------------------------------------------------------------
//
bool State_GameLevelInit()
{
	// Initialize display
	VDP_EnableDisplay(FALSE);

	// Initialize layout
	DrawLevel(g_Level);

	// Initialize color
    // 00 0x0 transparent
    // 01 0x1 black
    // 02 0x2 medium green
    // 03 0x3 light green
    // 04 0x4 dark blue
    // 05 0x5 light blue
    // 06 0x6 dark red
    // 07 0x7 cyan
    // 08 0x8 medium red
    // 09 0x9 light red
    // 10 0xA dark yellow
    // 11 0xB light yellow
    // 12 0xC dark green
    // 13 0xD magenta
    // 14 0xE gray
    // 15 0xF white

	//////////////
	// PLAYER
	//

	// Init player pawn
	GamePawn_Initialize(&(g_PlayerPawn.Pawn), g_PlayerSpriteLayers, numberof(g_PlayerSpriteLayers), 0, g_AnimActions);
	GamePawn_SetPosition(&(g_PlayerPawn.Pawn), 20, 164);
//	GamePawn_InitializePhysics(&g_PlayerPawn, PhysicsEvent_Player, PhysicsCollision_Player, 16, 16);
	GamePawnCusto_InitializePhysics(&g_PlayerPawn, PhysicsEvent_Player, PhysicsCollision_Player, 16, 16);

	//////////////
	// BOULDER
	//

	// BOULDER, Init pawn
	GamePawn_Initialize(&(g_BoulderPawn.Pawn), g_BoulderSpriteLayers, numberof(g_BoulderSpriteLayers), 3, g_AnimActions_Boulder);
	BoulderShow (24,0); // position and declare 'shown'
//	GamePawn_InitializePhysics(&g_BoulderPawn, PhysicsEvent_Boulder, PhysicsCollision_Boulder, 16, 16);
	GamePawnCusto_InitializePhysics(&g_BoulderPawn, PhysicsEvent_Boulder, PhysicsCollision_Boulder, 16, 16);
	g_Boulder_NewActionId = BOULDER_ACTION_FALL_RIGHT;

	//////////////
	// FLOWER
	//

// 	// FLOWER, Init pawn
// 	GamePawn_Initialize(&g_FlowerPawn, g_FlowerSpriteLayers, numberof(g_FlowerSpriteLayers), 3, g_AnimActions_Flower);
// 	GamePawn_SetPosition(&g_FlowerPawn, 20, 20);
// //	GamePawn_InitializePhysics(&g_FlowerPawn, null, null, 16, 16);
// 	g_Flower_NewActionId = FLOWER_ACTION_JIGGLE;

	VDP_EnableDisplay(TRUE);

	Game_SetState(State_GameLevelPlay); // Go to next State
	
	return TRUE; // Frame finished
}

//-----------------------------------------------------------------------------
//
bool State_GameLevelPlay()
{
	u8 act_player  = PLAYER_ACTION_IDLE;
	u8 row8 = Keyboard_Read(8);

	// TEXT animation
	Print_SetPosition(31, 0);
	Print_DrawChar(g_ChrAnim[g_GameFrame & 0x03]);
	
	// MUSIC is playing
	AKG_Decode ();

	// PLAYER Update animation & physics
	
	// Goal-tile reached?
	if ( (g_PlayerPawn.Pawn.PositionX+16  >g_Goal1_PositionX*8) 
		&& (g_PlayerPawn.Pawn.PositionY+16>g_Goal1_PositionY*8)
		&& (g_PlayerPawn.Pawn.PositionX   <g_Goal1_PositionX*8+8) 
		&& (g_PlayerPawn.Pawn.PositionY   <g_Goal1_PositionY*8+8) )
	{
		Game_SetState (State_GameLevelEnd);
		return TRUE; // Frame finished

	}
	
	// Manage actions
	if (g_bPlayerFlagEvent_Ladder)  
		act_player = PLAYER_ACTION_HANG;
	if (g_bPlayerFlagEvent_Moving)
	{
		if (g_bPlayerFlagEvent_Ladder)  
			act_player = PLAYER_ACTION_CLIMB;
		else switch (g_bPlayer_Direction)
		{
			case KEY_RIGHT : act_player = PLAYER_ACTION_MOVE_RIGHT; break;
			case KEY_LEFT  : act_player = PLAYER_ACTION_MOVE_LEFT;  break;
			case KEY_UP    : act_player = PLAYER_ACTION_MOVE_UP;    break;
			case KEY_DOWN  : act_player = PLAYER_ACTION_MOVE_DOWN;  break;
		}
	}
	if (act_player != PLAYER_ACTION_IDLE) g_bPlayerFlagEvent_Ladder = false;
	
	// Collision PLAYER/BOULDER
	if ( (g_PlayerPawn.Pawn.PositionX+16  >g_BoulderPawn.Pawn.PositionX) 
		&& (g_PlayerPawn.Pawn.PositionY+16>g_BoulderPawn.Pawn.PositionY)
		&& (g_PlayerPawn.Pawn.PositionX   <g_BoulderPawn.Pawn.PositionX+16) 
		&& (g_PlayerPawn.Pawn.PositionY   <g_BoulderPawn.Pawn.PositionY+16) )
		act_player = PLAYER_ACTION_PAIN;
	
	GamePawn_SetAction  	(&(g_PlayerPawn.Pawn), act_player);
//	GamePawn_SetMovement	(&g_PlayerPawn, g_DX, g_DY);
	GamePawn_Update     	(&(g_PlayerPawn.Pawn));
	GamePawnCusto_SetMovement(&g_PlayerPawn, g_DX, g_DY);
	GamePawnCusto_Update	(&g_PlayerPawn);
	GamePawn_Draw       	(&(g_PlayerPawn.Pawn));

//	// Handle collision events
//	switch(g_CatchBoulderEvent)
//	{
//	case PAWN_PHYSICS_BORDER_DOWN:
//		break;
//	case PAWN_PHYSICS_BORDER_RIGHT:
//		break;
//	}

	// Update PLAYER movement
	g_DX = 0;
	g_DY = 0;
	g_bPlayerFlagEvent_Moving = FALSE;
	if(IS_KEY_PRESSED(row8, KEY_UP))
	{
		g_DY--;
		g_bPlayerFlagEvent_Moving = TRUE;
		g_bPlayer_Direction = KEY_UP;
	}
	if(IS_KEY_PRESSED(row8, KEY_DOWN))
	{
		g_DY++;
		g_bPlayerFlagEvent_Moving = TRUE;
		g_bPlayer_Direction = KEY_DOWN;
	}
	if(IS_KEY_PRESSED(row8, KEY_RIGHT))
	{
		g_DX++;
		g_bPlayerFlagEvent_Moving = TRUE;
		g_bPlayer_Direction = KEY_RIGHT;
	}
	if(IS_KEY_PRESSED(row8, KEY_LEFT))
	{
		g_DX--;
		g_bPlayerFlagEvent_Moving = TRUE;
		g_bPlayer_Direction = KEY_LEFT;
	}

// SPECIAL KEYS (DEBUG)

	// Reach Level 0
	if(Keyboard_IsKeyPressed(KEY_0)) // Level 0
	{
		g_Level = 0 ; // force level 0
		Game_SetState (State_GameLevelInit);
	}
	// Reach next Level = CTRL+RETURN
	if(Keyboard_IsKeyPressed(KEY_CTRL) && Keyboard_IsKeyPressed(KEY_RETURN))
	{
		g_Level++; // increase
		if (g_Level == sizeof(g_DataLevel)/(24*32)) g_Level=1; // all levels but 0
		Game_SetState (State_GameLevelInit);
	}
	// Reach previous Level : CTRL+BACKSPACE
	if(Keyboard_IsKeyPressed(KEY_CTRL) && Keyboard_IsKeyPressed(KEY_BS))
	{
		g_Level--; // decrease
		if (g_Level==0)  g_Level = sizeof(g_DataLevel)/(24*32) -1; // all levels but 0
		Game_SetState (State_GameLevelInit);
	}

	if(Keyboard_IsKeyPressed(KEY_ESC))
		Game_Exit();
	
	// Update BOULDER
	
	// Reach a border?
//	switch (g_CatchBoulderEvent)
//	{
//		case PAWN_PHYSICS_BORDER_DOWN:
//		case PAWN_PHYSICS_BORDER_RIGHT:
//		case PAWN_PHYSICS_BORDER_LEFT:
//			g_CatchBoulderEvent = 0;
//			BoulderHide ();
//			GamePawn_Update(&g_BoulderPawn);
//			GamePawn_Draw(&g_BoulderPawn);
//	}
	
	// Update when on-screen (not hidden)
	if (!g_BoulderHidden)
	{
		GamePawn_SetAction(&(g_BoulderPawn.Pawn), g_Boulder_NewActionId); 
		GamePawn_Update(&(g_BoulderPawn.Pawn));
		switch (g_Boulder_NewActionId) {
//			case BOULDER_ACTION_ROLL_LEFT : GamePawn_SetMovement(&g_BoulderPawn, -3, 1); break;
//			case BOULDER_ACTION_ROLL_RIGHT: GamePawn_SetMovement(&g_BoulderPawn,  3, 1); break;
//			case BOULDER_ACTION_FALL_LEFT : GamePawn_SetMovement(&g_BoulderPawn, -1, 3); break;
//			case BOULDER_ACTION_FALL_RIGHT: GamePawn_SetMovement(&g_BoulderPawn,  1, 3); break;
			case BOULDER_ACTION_ROLL_LEFT : GamePawnCusto_SetMovement(&g_BoulderPawn, -3, 1); break;
			case BOULDER_ACTION_ROLL_RIGHT: GamePawnCusto_SetMovement(&g_BoulderPawn,  3, 1); break;
			case BOULDER_ACTION_FALL_LEFT : GamePawnCusto_SetMovement(&g_BoulderPawn, -1, 3); break;
			case BOULDER_ACTION_FALL_RIGHT: GamePawnCusto_SetMovement(&g_BoulderPawn,  1, 3); break;
		} // switch NewActionId
		GamePawnCusto_Update(&g_PlayerPawn);
		GamePawn_Draw(&(g_BoulderPawn.Pawn));
	} // if !gBoulderHidden
	
//	// Handle collision events
//	switch(g_CatchBoulderEvent)
//	{
//	case PAWN_PHYSICS_BORDER_DOWN:
//		break;
//	case PAWN_PHYSICS_BORDER_RIGHT:
//		break;
//	}

	return TRUE; // Frame finished
}

//-----------------------------------------------------------------------------
//
//bool State_GameLevelPause()
//{
//	return TRUE; // Frame finished
//}

bool State_GameLevelEnd()
{
	// Text
	Print_DrawCharAt (31,  0, g_ChrAnim[g_GameFrame & 0x03]);
	Print_DrawTextAt (10,  9, "  SUCCESS!  ");
//	Print_DrawTextAt (10, 10, "<press SPACE>");
	
	// Music
	AKG_Decode ();
	
	// Anim Player
	GamePawn_SetAction  (&(g_PlayerPawn.Pawn), PLAYER_ACTION_DANCE);
	GamePawn_Update     (&(g_PlayerPawn.Pawn));
	GamePawnCusto_Update(&g_PlayerPawn);
	GamePawn_Draw       (&(g_PlayerPawn.Pawn));

	// Wait SPACE
	if(Keyboard_IsKeyPressed(KEY_SPACE)) // Next level
	{
		g_Level++; // increase
		if (g_Level == sizeof(g_DataLevel)/(24*32)) g_Level=1; // all levels but 0
		Game_SetState (State_GameLevelInit);
	}

	return TRUE; // Frame finished
}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Programme entry point
void main()
{

	Game_SetState (State_DataLoad); // load data before 1st display
	Game_MainLoop (VDP_MODE_GRAPHIC1);

//	Bios_Exit(0);

}