/**
 * @file attack.c
 * This file includes all functions used to filter general moves.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

// Formula: attacked_square - attacking_square + 77 = pieces able to attack
const u8 AttackArray[155] = {
	5,0,0,0,0,0,0,2,0,0,0,5,0,0,5,
	0,0,2,0,0,0,0,5,5,0,0,0,2,0,0,
	0,0,5,5,0,0,0,2,0,0,0,5,0,0,5,
	0,0,2,0,0,5,0,0,0,0,5,6,2,6,5,
	0,0,0,0,0,6,3,1,3,6,2,2,2,2,2,
	2,1,0,1,2,2,2,2,2,2,6,4,1,4,6,
	0,0,0,0,0,5,6,2,6,5,0,0,0,0,5,
	0,0,2,0,0,5,0,0,5,0,0,0,2,0,0,
	0,5,5,0,0,0,0,2,0,0,0,5,5,0,0,
	0,0,2,0,0,5,0,0,5,0,0,0,2,0,0,
	0,0,0,0,5
};

static const u8 LoopSidePce[] = {wP, wN, wB, wR, wQ, wK, 0, bP, bN, bB, bR, bQ, bK, 0};
static const u8 LoopSideIndex[2] = {0, 7};

// These arrays contain all direction that pieces can make on board
const s8 KnDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};                            // Knights
const s8 RkDir[4] = {-1, -10, 1, 10};                                              // Roooks
const s8 BiDir[4] = {-9, -11, 9, 11};                                              // Bishops
const s8 KiDir[8] = {-1, -10, 1, 10, -9, -11, 9, 11};                              // Kings

/**
 * A fuction to take a specified square on the board and check if it is in attack by
 * a specified side.
 * 
 * @param sq The square on the board to check.
 * @param side The opponent side.
 * @param pos The position's pointer to check.
 * @return TRUE if the spuare is attacked, FALSE if not. 
 */
u8 IsSqAttacked(const u8 sq, const u8 side, const S_BOARD *pos){
	u8 pce, index, t_sq, dir;
	
	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid);
	ASSERT(CheckBoard(pos));
	
	// pawns
	if(side == WHITE){
		if(pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) return TRUE;
	}else{
		if(pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) return TRUE;
	}
	
	// knights
	for(index = 0;index < 8; index++){
		pce = pos->pieces[sq + KnDir[index]];
		if(pce == OFF_BOARD || pce == EMPTY) continue;
		if(PieceCol[pce] == side && IsKn(pce)) return TRUE;
	}
	
	// rooks and quuens
	for(index = 0; index < 4; index++){
		dir = RkDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];
		while(pce != OFF_BOARD){
			if(pce != EMPTY){
				if(PieceCol[pce] == side && IsRQ(pce)) return TRUE;
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}
	
	// bishops and quuens
	for(index = 0; index < 4; index++){
		dir = BiDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];
		while(pce != OFF_BOARD){
			if(pce != EMPTY){
				if(PieceCol[pce] == side && IsBQ(pce)) return TRUE;
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}
	
	// kings
	for(index = 0;index < 8; index++){
		pce = pos->pieces[sq + KiDir[index]];
		if(pce == OFF_BOARD || pce == EMPTY) continue;
		if(PieceCol[pce] == side && IsKi(pce)) return TRUE;
	}
	
	// Return false if there is no attack
	return FALSE;
	
}






