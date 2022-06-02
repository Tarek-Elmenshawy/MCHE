/**
 * @file makemove.c
 * This file includes all functions that make changes on board.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

/* These macros are to add or remove a specified key fron the general hash key. */
#define HASH_PCE(pce, sq) (pos->posKey ^= (PieceKeys[(pce)][SQ64(sq)])) ///< Hask the piece key on a specified square on board from the general hash key.             
#define HASH_WP(sq) (pos->pawnKey ^= (PieceKeys[wP][SQ64(sq)]))			///< Hask the white pawn key on a specified square on board from the general hash key.  
#define HASH_BP(sq) (pos->pawnKey ^= (PieceKeys[bP][SQ64(sq)]))			///< Hask the black pawn key on a specified square on board from the general hash key.  
#define HASH_CA (pos->posKey ^= CastleKeys[pos->castlePerm])			///< Hask the castling key -depends on castle pernission flag- from the general hash key.  
#define HASH_SIDE (pos->posKey ^= SideKey)								///< Hask the side key -depends on current turn- from the general hash key.  
#define HASH_EP (pos->posKey ^= PieceKeys[EMPTY][SQ64(pos->enPass)])	///< Hask tthe square where en passent occurs on board from the general hash key. 

/// This array retuns a mask that used to udate castlePerm depends on the squarethat the piece maves from
/// to track any chenge which leads to not be able to castle.
///
/// Ex. when the black king move from e8 
///
/// Before move:
/// castlePer = 1111 in Binary
///
/// After move:
/// castlePer &= CastlePerm[e8] => 1111 & 0011 = 0011
/// So the black king castling permission is removed is 
static const u8 CastlePerm[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7, 15, 15, 15,  3, 15, 15, 11
};

/**
 * This method to remove the piece on the given squre on the board
 * then updates board information.
 * 
 * @param sq The square where the piece -needed to be cleard- is on.
 * @param pos The position's pointer.
 */	
static void ClearPiece(const u8 sq, S_BOARD *pos){
	ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);
	
	ASSERT(SqOnBoard(sq));
	ASSERT(CheckBoard(pos));
	
	u8 pce = pos->pieces[sq];
	ASSERT(PieceValid(pce));
	
	u8 col = PieceCol[pce];
	ASSERT(SideValid(col));
	
	u8 index;
	s8 t_pceNum = -1;
	
	if(pce == wP) HASH_WP(sq);
	else if(pce == bP) HASH_BP(sq);
	
	HASH_PCE(pce, sq);
	
	pos->pieces[sq] = EMPTY;
	pos->material[col] -= PieceVal[pce];
	
	if(PieceBig[pce]){
		pos->bigPce[col]--;
		if(PieceMaj[pce]) pos->majPce[col]--;
		else pos->minPce[col]--;
	}else{
		CLRBIT(pos->pawns[col], SQ64(sq));
		CLRBIT(pos->pawns[BOTH], SQ64(sq));
	}
	
	for(index = 0; index < pos->pceNum[pce]; index++){
		if(pos->pList[pce][index] == sq){
			t_pceNum = index;
			break;
		}
	}
	
	ASSERT(t_pceNum != -1);
	ASSERT(t_pceNum >=0 && t_pceNum < 10);
	
	pos->pceNum[pce]--;
	pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pceNum[pce]];  //Put the last piece's place of the pList in the deleted ppiece's place                                               
	ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);
}

/**
 * This method to put a given piece on a given squre on the board
 * then updates board information.
 * 
 * @param sq The square where the piece needed to be added.
 * @param pce The piece type neede to be added.
 * @param pos The position's pointer.
 */	
static void AddPiece(const u8 sq, const u8 pce, S_BOARD *pos){
	ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);
	
	ASSERT(SqOnBoard(sq));
	ASSERT(CheckBoard(pos));
	ASSERT(PieceValid(pce));
	
	u8 col = PieceCol[pce];
	ASSERT(SideValid(col));
	
	if(pce == wP) HASH_WP(sq);
	else if(pce == bP) HASH_BP(sq);
	
	HASH_PCE(pce,sq);
	
	pos->pieces[sq] = pce;
	pos->material[col] += PieceVal[pce];
	
	if(PieceBig[pce]){
		pos->bigPce[col]++;
		if(PieceMaj[pce]) pos->majPce[col]++;
		else pos->minPce[col]++;
	}else{
		SETBIT(pos->pawns[col], SQ64(sq));
		SETBIT(pos->pawns[BOTH], SQ64(sq));
	}
	
	pos->pList[pce][pos->pceNum[pce]++] = sq;
	ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);
	
}

/**
 * This method to move a piece from a given squre to a given square on the board
 * then updates board information.
 * 
 * @param from The square where the piece needed to be moved from.
 * @param to The square where the piece needed to be moved to.
 * @param pos The position's pointer.
 */	
static void MovePiece(const u8 from, const u8 to, S_BOARD *pos){
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	
	u8 pce = pos->pieces[from];
	ASSERT(PieceValid(pce));
	
	u8 col = PieceCol[pce];
	ASSERT(SideValid(col));
	
	if(pce == wP) HASH_WP(from);
	else if(pce == bP) HASH_BP(from);
	
	HASH_PCE(pce,from);
	pos->pieces[from] = EMPTY;
	
	if(pce == wP) HASH_WP(to);
	else if(pce == bP) HASH_BP(to);
	
	HASH_PCE(pce,to);
	pos->pieces[to] = pce;
	
	if(!PieceBig[pce]){
		SETBIT(pos->pawns[col], SQ64(to));
		SETBIT(pos->pawns[BOTH], SQ64(to));
		CLRBIT(pos->pawns[col], SQ64(from));
		CLRBIT(pos->pawns[BOTH], SQ64(from));
	}
	
	u8 index;
	#ifdef DEBUG
		u8 f_pce = FALSE;
	#endif

	for(index = 0; index < pos->pceNum[pce]; index++){
		if(pos->pList[pce][index] == from){
			pos->pList[pce][index] = to;
			#ifdef DEBUG
				f_pce = TRUE;;
			#endif
			break;
		}
	}
	
	ASSERT(f_pce);
}

/**
 * This method performs some changes on the board to make a move on boud
 * then updates board information.
 * 
 * @param pos The position's pointer.
 * @param move The move to be made on the board.
 * @return TRUE if the move is made correctly or FALSE if not.
 */	
u8 MakeMove(S_BOARD *pos, s32 move){
	
	ASSERT(MoveValid(move, pos));
	ASSERT(CheckBoard(pos));
	
	u8 from = FROMSQ(move);
	ASSERT(SqOnBoard(from));
	
	u8 to = TOSQ(move);
	ASSERT(SqOnBoard(to));
	
	u8 side = pos->side;
	ASSERT(SideValid(side));
	
	ASSERT(PieceValid(pos->pieces[from]));
	
	ASSERT(pos->hisply >= 0 && pos->hisply < MAXGAMEMOVES);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	//Save all data in history structure before making any change
	pos->history[pos->hisply].move = move;
	pos->history[pos->hisply].castlePerm = pos->castlePerm;
	pos->history[pos->hisply].enPass = pos->enPass;
	pos->history[pos->hisply].fiftyMove = pos->fiftyMove;
	pos->history[pos->hisply].posKey = pos->posKey;
	pos->history[pos->hisply].pawnKey = pos->pawnKey;
	
	pos->ply++;
	pos->hisply++;
	ASSERT(pos->hisply >= 0 && pos->hisply < MAXGAMEMOVES);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if(move & MFLAGEP){
		if(side == WHITE) ClearPiece(to - 10, pos);
		else ClearPiece(to + 10, pos);
	}else if(move & MFLAGCA){
		
		#ifdef DEBUG
			switch(to){
				case C1: ASSERT(PieceValid(pos->pieces[A1])); break;
				case C8: ASSERT(PieceValid(pos->pieces[A8])); break;
				case G1: ASSERT(PieceValid(pos->pieces[H1])); break;
				case G8: ASSERT(PieceValid(pos->pieces[H8])); break;
				default: ASSERT(FALSE);
			}
		#endif
		
		switch(to){
			case C1: MovePiece(A1, D1, pos); break;
			case C8: MovePiece(A8, D8, pos); break;
			case G1: MovePiece(H1, F1, pos); break;
			case G8: MovePiece(H8, F8, pos); break;
			default: ASSERT(FALSE);
		}
	}
	
	HASH_CA;
	pos->castlePerm &= CastlePerm[SQ64(from)];
	pos->castlePerm &= CastlePerm[SQ64(to)];
	HASH_CA;
	
	if(pos->enPass != NO_SQ){
		HASH_EP;
		pos->enPass = NO_SQ;
	}
	
	pos->fiftyMove++;
	
	if(CAPTURED(move) != EMPTY){
		ASSERT(PieceValid(CAPTURED(move)));
		ClearPiece(to, pos);
		pos->fiftyMove = 0;
	}
	
	//If the moved piece is a pawn and moved 2 squares, set the enPass square
	if(!(PieceBig[pos->pieces[from]])){
		pos->fiftyMove = 0;
		if(move & MFLAGPS){
			if(side == WHITE){
				pos->enPass = from + 10;
				ASSERT(RanksBrd[SQ64(pos->enPass)] == RANK_3);
			}else{
				pos->enPass = from - 10;
				ASSERT(RanksBrd[SQ64(pos->enPass)] == RANK_6);
			}
			HASH_EP;
		}
	}
	
	ASSERT(PieceValid(pos->pieces[from]));
	MovePiece(from, to, pos);
	
	if(IsKi(pos->pieces[to])) pos->kingSq[side] = to;
	
	u8 proPce = PROMOTED(move);
	if(proPce != EMPTY){
		ASSERT(PieceValid(proPce) && PieceBig[proPce] && !PieceKing[proPce]);
		ClearPiece(to, pos);
		AddPiece(to, proPce, pos);
	}
	
	pos->side ^= 1;
	HASH_SIDE;
	
	ASSERT(CheckBoard(pos));
	
	if(IsSqAttacked(pos->kingSq[side], pos->side, pos)){
		UnMakeMove(pos);
		return FALSE;
	}
	
	return TRUE;
}

/**
 * Undo the last move and return to the previos move using history structure.
 * 
 * @param pos The position's pointer.
 */	
void UnMakeMove(S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	
	pos->ply--;
	pos->hisply--;
	ASSERT(pos->hisply >= 0 && pos->hisply < MAXGAMEMOVES);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	s32 move = pos->history[pos->hisply].move;
	
	u8 from = FROMSQ(move);
	ASSERT(SqOnBoard(from));
	
	u8 to = TOSQ(move);
	ASSERT(SqOnBoard(to));
	
	pos->side ^= 1;
	HASH_SIDE;
	
	if(pos->enPass != NO_SQ) HASH_EP;
	HASH_CA;
	
    pos->castlePerm = pos->history[pos->hisply].castlePerm;
	pos->enPass = pos->history[pos->hisply].enPass;
	pos->fiftyMove = pos->history[pos->hisply].fiftyMove;
	
	if(pos->enPass != NO_SQ) HASH_EP;
	HASH_CA;
	
	if(move & MFLAGEP){
		if(pos->side == WHITE) AddPiece(to - 10, bP, pos);
		else AddPiece(to + 10, wP, pos);
	}else if(move & MFLAGCA){
		switch(to){
			case C1: MovePiece(D1, A1, pos); break;
			case C8: MovePiece(D8, A8, pos); break;
			case G1: MovePiece(F1, H1, pos); break;
			case G8: MovePiece(F8, H8, pos); break;
			default: ASSERT(FALSE);
		}
	}
	
	MovePiece(to, from, pos);
	
	if(IsKi(pos->pieces[from])) pos->kingSq[pos->side] = from;
	
	if(CAPTURED(move) != EMPTY){
		ASSERT(PieceValid(CAPTURED(move)));
		AddPiece(to,CAPTURED(move), pos);
	}
	
	u8 proPce = PROMOTED(move);
	if(proPce != EMPTY){
		ASSERT(PieceValid(proPce) && PieceBig[proPce] && !PieceKing[proPce]);
		ClearPiece(from, pos);
		AddPiece(from, (pos->side == WHITE? wP : bP), pos);
	}
	
	ASSERT(CheckBoard(pos));
	
}

/**
 * This method performs some changes on the board to make a null-move on boud
 * 
 * @param pos The position's pointer.
 */	
void MakeNullMove(S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	ASSERT(!IsSqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos));
	
	//Save all data in history structure before making any change
	pos->history[pos->hisply].move = NOMOVE;
	pos->history[pos->hisply].castlePerm = pos->castlePerm;
	pos->history[pos->hisply].enPass = pos->enPass;
	pos->history[pos->hisply].fiftyMove = pos->fiftyMove;
	pos->history[pos->hisply].posKey = pos->posKey;
	pos->history[pos->hisply].pawnKey = pos->pawnKey;
	
	pos->ply++;
	pos->hisply++;
	ASSERT(pos->hisply >= 0 && pos->hisply < MAXGAMEMOVES);
	if(!(pos->ply >= 0 && pos->ply < MAXDEPTH)) printf("MakeNull():  ply = %d\n", pos->ply);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if(pos->enPass != NO_SQ){
		HASH_EP;
		pos->enPass = NO_SQ;
	}
	
	pos->side ^= 1;
	HASH_SIDE;
	ASSERT(CheckBoard(pos));
	
}

/**
 * Undo the last null-move and return to the previos move using history structure.
 * 
 * @param pos The position's pointer.
 */	
void UnMakeNullMove(S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	pos->ply--;
	pos->hisply--;
	ASSERT(pos->hisply >= 0 && pos->hisply < MAXGAMEMOVES);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	pos->side ^= 1;
	
    pos->castlePerm = pos->history[pos->hisply].castlePerm;
	pos->enPass = pos->history[pos->hisply].enPass;
	pos->fiftyMove = pos->history[pos->hisply].fiftyMove;
	pos->posKey = pos->history[pos->hisply].posKey;
	pos->pawnKey = pos->history[pos->hisply].pawnKey;
	
	
	ASSERT(CheckBoard(pos));
	
}













