// game_pawn_custo
// specific collision for my game 'badger'

#include "vdp.h"
#include "game_pawn_customove.h"

void GamePawnCusto_InitializePhysics(Game_Pawn_CustoMove* pawn, Game_PhysicsCB pcb, Game_CollisionCB ccb, u8 boundX, u8 boundY)
{
	boundX;
	boundY;
#if (GAMEPAWN_BOUND_X == GAMEPAWN_BOUND_CUSTOM)
	pawn->BoundX = boundX;
#endif
#if (GAMEPAWN_BOUND_Y == GAMEPAWN_BOUND_CUSTOM)
	pawn->BoundY = boundY;
#endif
	pawn->PhysicsCB = pcb; 
	pawn->CollisionCB = ccb;
}

void GamePawnCusto_SetMovement(Game_Pawn_CustoMove* g_PawnCusto, i8 dx, i8 dy) 
{ 
	g_PawnCusto->MoveX = dx;
	g_PawnCusto->MoveY = dy;
	(g_PawnCusto->Pawn).Update |= PAWN_UPDATE_COLLISION;
}


void GamePawnCusto_Update (Game_Pawn_CustoMove* g_PawnCusto)
//
// <<< à lancer APRES GamePawn_Update >>>
// 
{
	Game_Pawn *g_Pawn = &(g_PawnCusto->Pawn);
//#if (GAMEPAWN_USE_PHYSICS)
	if (g_Pawn->Update & PAWN_UPDATE_COLLISION)
	{
		u8 targetX = g_Pawn->PositionX + g_PawnCusto->MoveX;
		u8 targetY = g_Pawn->PositionY + g_PawnCusto->MoveY;

		//.....................................................................
		// Vertical movement - Go down
		if (g_PawnCusto->MoveY > 0)
		{
			#if ((GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_DOWN) || (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_DOWN))
			if (targetY + GET_BOUND_Y >= GAMEPAWN_BORDER_MAX_Y)
			{
				#if (GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_DOWN)
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_BORDER_DOWN, 0);
				#endif
				#if (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_DOWN)
					targetY = (u8)(GAMEPAWN_BORDER_MAX_Y - GET_BOUND_Y);
				#endif
			}
			else
			#endif
			{
				g_Game_CellY = (targetY + GET_BOUND_Y) / 8;
				g_Game_CellX = 0xFF;
				u8 lastCell = 0xFF;
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_0)
					g_Game_CellX = (targetX) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
						{
							g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_DOWN, tile);
							targetY = (g_Game_CellY * 8) - GET_BOUND_Y;
							goto skipVertival;
						}
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_25)
					g_Game_CellX = (targetX + (GET_BOUND_X / 4)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
						{
							g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_DOWN, tile);
							targetY = (g_Game_CellY * 8) - GET_BOUND_Y;
							goto skipVertival;
						}
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_50)
					g_Game_CellX = (targetX + (GET_BOUND_X / 2)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
						{
							g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_DOWN, tile);
							targetY = (g_Game_CellY * 8) - GET_BOUND_Y;
							goto skipVertival;
						}
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_75)
					g_Game_CellX = (targetX + GET_BOUND_X - (GET_BOUND_X / 4)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
						{
							g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_DOWN, tile);
							targetY = (g_Game_CellY * 8) - GET_BOUND_Y;
							goto skipVertival;
						}
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_100)
					g_Game_CellX = (targetX + GET_BOUND_X - 1) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
						{
							g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_DOWN, tile);
							targetY = (g_Game_CellY * 8) - GET_BOUND_Y;
							goto skipVertival;
						}
					}
				#endif
			}
		}
		//.....................................................................
		// Vertical movement - Go up
		else if (g_PawnCusto->MoveY < 0)
		{
			#if ((GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_UP) || (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_UP))
			#if (GAMEPAWN_BORDER_MIN_Y > 0)
			if ((targetY > g_Pawn->PositionY) || ((targetY < GAMEPAWN_BORDER_MIN_Y)))
			#else
			if (targetY > g_Pawn->PositionY)
			#endif
			{
				#if (GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_UP)
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_BORDER_UP, 0);
				#endif
				#if (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_UP)
					targetY = GAMEPAWN_BORDER_MIN_Y;
				#endif
			}
			else
			#endif
			{
				g_Game_CellX = (targetX + (GET_BOUND_X / 2)) / 8;
				g_Game_CellY = (targetY) / 8;
				u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
				if (g_PawnCusto->CollisionCB(tile))
				{
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_UP, tile);
					targetY = (g_Game_CellY * 8) + 8;
				}
			}
		}
		//.....................................................................
		// No vertical movement - Check floor
		else // if (g_PawnCusto->MoveY == 0)
		{
			#if (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_DOWN)
			if (targetY + GET_BOUND_Y < GAMEPAWN_BORDER_MAX_Y)
			#endif
			{
				u8 tile = 0;
				g_Game_CellY = (targetY + GET_BOUND_Y) / 8;
				g_Game_CellX = 0xFF;
				u8 lastCell = 0xFF;
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_0)
					g_Game_CellX = (targetX) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
							goto skipVertival;
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_25)
					g_Game_CellX = (targetX + (GET_BOUND_X / 4)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
							goto skipVertival;
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_50)
					g_Game_CellX = (targetX + (GET_BOUND_X / 2)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
							goto skipVertival;
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_75)
					g_Game_CellX = (targetX + GET_BOUND_X - (GET_BOUND_X / 4)) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
							goto skipVertival;
					}
				#endif
				#if (GAMEPAWN_COL_DOWN & GAMEPAWN_COL_100)
					g_Game_CellX = (targetX + GET_BOUND_X - 1) / 8;
					if (g_Game_CellX != lastCell)
					{
						lastCell = g_Game_CellX;
						tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
						if (g_PawnCusto->CollisionCB(tile))
							goto skipVertival;
					}
				#endif
				g_PawnCusto->PhysicsCB(PAWN_PHYSICS_FALL, tile);
			}
		}
skipVertival:
		//.....................................................................
		// Horizontal movement - Go right
		if (g_PawnCusto->MoveX > 0)
		{
			#if ((GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_RIGHT) || (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_RIGHT))
			if ((u8)(targetX + GET_BOUND_X) < g_Pawn->PositionX)
			{
				#if (GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_RIGHT)
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_BORDER_RIGHT, 0);
				#endif
				#if (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_RIGHT)
					targetX = (u8)(0 - GET_BOUND_X);
				#endif
			}
			else
			#endif
			{
				g_Game_CellX = (targetX + GET_BOUND_X) / 8;
				g_Game_CellY = (targetY + (GET_BOUND_Y / 2)) / 8;
				u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
				if (g_PawnCusto->CollisionCB(tile))
				{
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_RIGHT, tile);
					targetX = (g_Game_CellX * 8) - GET_BOUND_X;
				}
			}
		}
		//.....................................................................
		// Horizontal movement - Go left
		else if (g_PawnCusto->MoveX < 0)
		{
			#if ((GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_LEFT) || (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_LEFT))
			if (targetX > g_Pawn->PositionX)
			{
				#if (GAMEPAWN_BORDER_EVENT & GAMEPAWN_BORDER_LEFT)
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_BORDER_LEFT, 0);
				#endif
				#if (GAMEPAWN_BORDER_BLOCK & GAMEPAWN_BORDER_LEFT)
					targetX = 0;
				#endif
			}
			else
			#endif
			{
				g_Game_CellX = (targetX) / 8;
				g_Game_CellY = (targetY + (GET_BOUND_Y / 2)) / 8;
				u8 tile = GAMEPAWN_GET_TILE(g_Game_CellX, g_Game_CellY);
				if (g_PawnCusto->CollisionCB(tile))
				{
					g_PawnCusto->PhysicsCB(PAWN_PHYSICS_COL_LEFT, tile);
					targetX = (g_Game_CellX * 8) + 8;
				}
			}
		}

		g_Pawn->PositionX = targetX;
		g_Pawn->PositionY = targetY;
		g_Pawn->Update |= PAWN_UPDATE_POSITION;
		g_Pawn->Update &= ~PAWN_UPDATE_COLLISION;
	}
//#endif
} // GamePawnCusto