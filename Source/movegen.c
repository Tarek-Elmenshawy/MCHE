/**
 * @file movegen.c
 * This file includes all methods that deel with generating and checking moves.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include "defs.h"

#define MOVE(f, t, cap, pro, fl) ((f) | (t << 7) | (cap << 14) | (pro << 20) | (fl))  ///< Combine move elements to get an integer move.

static const u8 PieceSlides[13] = {FALSE, FALSE, FALSE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, TRUE, FALSE, FALSE}; ///< Returns true if the piece passed as an index is a silde piece.                             

static const u8 LoopSlidePce[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};
static const u8 LoopSlideIndex[2] = {0, 4};

static const u8 LoopNonSlidePce[6] = {wN, wK, 0, bN, bK, 0};
static const u8 LoopNonSlideIndex[2] = {0, 3};

static const s8 PceDir[13][8]= {
	{0, 0, 0, 0, 0, 0, 0, 0},                                           // EMPTY
	{0, 0, 0, 0, 0, 0, 0, 0},											// wP
	{-8, -19, -21, -12, 8, 19, 21, 12},                                 // wN
	{-9, -11, 11, 9, 0, 0, 0, 0},										// wB
	{-1, -10, 1, 10, 0, 0, 0, 0},										// wR
	{-1, -10, 1, 10, -9, -11, 11, 9},									// wQ
	{-1, -10, 1, 10, -9, -11, 11, 9},									// wK
	{0, 0, 0, 0, 0, 0, 0, 0},											// bP
	{-8, -19, -21, -12, 8, 19, 21, 12},                                 // bN
	{-9, -11, 11, 9, 0, 0, 0, 0},										// bB
	{-1, -10, 1, 10, 0, 0, 0, 0},										// bR
	{-1, -10, 1, 10, -9, -11, 11, 9},									// bQ
	{-1, -10, 1, 10, -9, -11, 11, 9} 									// bK
};

static const u8 NumDir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

/* Move Ordering:
 * PV move
 * Captures -> MvvLva
 * Killers -> Beta cut off
 * HistoryScores -> Beating alpha
 */

const u8 VictimScores[13] = {0, 10, 20, 30, 40, 50, 60, 10, 20, 30, 40, 50, 60};    ///< Socres of piece types.

/**
 * This function to check if the given move exists on the current board or not.
 *
 * @param pos The position's pointer.
 * @param move
 */
u8 MoveExists(S_BOARD *pos, const s32 move){
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	
	u8 moveNum;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		if(!MakeMove(pos, list->moves[moveNum].move)) continue;
		UnMakeMove(pos);
		if(list->moves[moveNum].move == move) return TRUE;
		
	}
	
	return FALSE;
	
}

static void AddQuietMove(const S_BOARD *pos, s32 move, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(CheckBoard(pos));
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	list->moves[list->count].move = move;
	list->moves[list->count].score = 0;
	
	#ifdef USE_KILLERS
	if(pos->searchKillers[0][pos->ply] == move) list->moves[list->count].score = 900000;
	else if(pos->searchKillers[1][pos->ply] == move) list->moves[list->count].score = 800000;
	#endif
	#ifdef USE_HISTORY
	else list->moves[list->count].score = pos->searchHistory[pos->pieces[FROMSQ(move)]][SQ64(TOSQ(move))];
	#endif
	
	#ifdef USE_EVALMOVE
	if(list->moves[list->count].score == 0)
		list->moves[list->count].score = EvalMove(pos, FROMSQ(move), TOSQ(move));
	#endif
	
	list->count++;
}

static void AddCaptureMove(const S_BOARD *pos, s32 move, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(PieceValid(CAPTURED(move)));
	ASSERT(PieceValid(pos->pieces[FROMSQ(move)]));
	ASSERT(CheckBoard(pos));
	
	list->moves[list->count].move = move;
	#ifdef USE_SEE
	list->moves[list->count].score = SEE(pos, move) + 1000000;
	#endif
	list->count++;
}

static void AddEnPassantMove(const S_BOARD *pos, s32 move, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(FROMSQ(move)));
	ASSERT(SqOnBoard(TOSQ(move)));
	ASSERT(CheckBoard(pos));
	ASSERT((RanksBrd[SQ64(TOSQ(move))] == RANK_6 && pos->side == WHITE) || (RanksBrd[SQ64(TOSQ(move))] == RANK_3 && pos->side == BLACK));
	
	list->moves[list->count].move = move;
	#ifdef USE_SEE
	list->moves[list->count].score = SEE(pos, move) + 1000000;
	#endif
	list->count++;
}

static void AddWhitePawnCapMove(const S_BOARD *pos, const u8 from, const u8 to, const u8 cap, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(PieceValid(cap));
	
	if(RanksBrd[SQ64(from)] == RANK_7){
		AddCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
	}else{
		AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
	}
}

static void AddWhitePawnMove(const S_BOARD *pos, const u8 from, const u8 to, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	
	if(RanksBrd[SQ64(from)] == RANK_7){
		AddQuietMove(pos, MOVE(from, to, EMPTY, wQ, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, wR, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, wB, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, wN, 0), list);
	}else{
		AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
	}
}

static void AddBlackPawnCapMove(const S_BOARD *pos, const u8 from, const u8 to, const u8 cap, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	ASSERT(PieceValid(cap));
	
	if(RanksBrd[SQ64(from)] == RANK_2){
		AddCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
		AddCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
	}else{
		AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
	}
}

static void AddBlackPawnMove(const S_BOARD *pos, const u8 from, const u8 to, S_MOVELIST *list){
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	
	if(RanksBrd[SQ64(from)] == RANK_2){
		AddQuietMove(pos, MOVE(from, to, EMPTY, bQ, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, bR, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, bB, 0), list);
		AddQuietMove(pos, MOVE(from, to, EMPTY, bN, 0), list);
	}else{
		AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
	}
}


/**
 * This function generates all possible moves without filterating.
 *
 * @param pos The position's pointer.
 * @param list The pointer of move list.
 */
void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list){
	
	ASSERT(CheckBoard(pos));
	
	list->count = 0;
	u8 pceNum, pceIndex, sq, t_sq, pce, index;
	s8 dir;
	u8 side = pos->side;
	
	
	if(side == WHITE){
		// white pawns
		for(pceNum = 0; pceNum < pos->pceNum[wP]; pceNum++){
			sq = pos->pList[wP][pceNum];
			ASSERT(SqOnBoard(sq));
			
			// quite moves
			if(pos->pieces[sq + 10] == EMPTY){
				AddWhitePawnMove(pos, sq, sq + 10, list);
				if(RanksBrd[SQ64(sq)] == RANK_2 && pos->pieces[sq + 20] == EMPTY) 
					AddQuietMove(pos, MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
			}
			
			// capture moves
			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK)
				AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
			
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK)
				AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq + 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq + 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
			}
			
		}
		
		// castling
		// white king castling
		if(pos->castlePerm & WKCA){
			if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY){
				if(!IsSqAttacked(E1, BLACK, pos) && !IsSqAttacked(F1, BLACK, pos)){
					AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
		
		// white quuen castling
		if(pos->castlePerm & WQCA){
			if(pos->pieces[B1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[D1] == EMPTY){
				if(!IsSqAttacked(E1, BLACK, pos) && !IsSqAttacked(D1, BLACK, pos)){
					AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
		
	}else{
		// black pawns
		for(pceNum = 0; pceNum < pos->pceNum[bP]; pceNum++){
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));
			
			// quite moves
			if(pos->pieces[sq - 10] == EMPTY){
				AddBlackPawnMove(pos, sq, sq - 10, list);
				if(RanksBrd[SQ64(sq)] == RANK_7 && pos->pieces[sq - 20] == EMPTY) 
					AddQuietMove(pos, MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
			}
			
			// capture moves
			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE)
				AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
			
			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE)
				AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq - 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq - 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
			}
			
		}
		
		// castling
		// black king castling
		if(pos->castlePerm & BKCA){
			if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY){
				if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(F8, WHITE, pos)){
					AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
		
		// black quuen castling
		if(pos->castlePerm & BQCA){
			if(pos->pieces[B8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[D8] == EMPTY){
				if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(D8, WHITE, pos)){
					AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
				}
			}
		}
		
	}
	
	// slide pieces
	pceIndex = LoopSlideIndex[side];
	pce = LoopSlidePce[pceIndex];
	
	while(pce != 0){
		ASSERT(PieceValid(pce));
		
		for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));
			
			for(index = 0; index < NumDir[pce]; index++){
				dir = PceDir[pce][index];
			    t_sq = sq + dir;
				
				while(!SQOFFBOARD(t_sq)){
					
					if(pos->pieces[t_sq] != EMPTY){
						if(PieceCol[pos->pieces[t_sq]] == side ^ 1){
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
				
					AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
					t_sq += dir;
				}
				
			}
		}
		
		pce = LoopSlidePce[++pceIndex];
	}
	
	// non slide pieces
	pceIndex = LoopNonSlideIndex[side];
	pce = LoopNonSlidePce[pceIndex];
	
	while(pce != 0){
		ASSERT(PieceValid(pce));
		
		for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));
			
			for(index = 0; index < NumDir[pce]; index++){
				dir = PceDir[pce][index];
			    t_sq = sq + dir;
				
				if(SQOFFBOARD(t_sq)) continue;
				
				if(pos->pieces[t_sq] != EMPTY){
					if(PieceCol[pos->pieces[t_sq]] == side ^ 1){
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
				
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
			}
		}
		
		pce = LoopNonSlidePce[++pceIndex];
	}
	
}

/**
 * This function generates all possible captured moves without filterating.
 *
 * @param pos The position's pointer.
 * @param list The pointer of move list.
 */
void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list){
	
	ASSERT(CheckBoard(pos));
	
	list->count = 0;
	u8 pceNum, pceIndex, sq, t_sq, pce, index;
	s8 dir;
	u8 side = pos->side;
	
	
	if(side == WHITE){
		// white pawns
		for(pceNum = 0; pceNum < pos->pceNum[wP]; pceNum++){
			sq = pos->pList[wP][pceNum];
			ASSERT(SqOnBoard(sq));
			
			// capture moves
			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK)
				AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
			
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK)
				AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq + 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq + 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
			}
			
		}
		
	}else{
		// black pawns
		for(pceNum = 0; pceNum < pos->pceNum[bP]; pceNum++){
			sq = pos->pList[bP][pceNum];
			ASSERT(SqOnBoard(sq));
			
			// capture moves
			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE)
				AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
			
			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE)
				AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq - 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq - 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
			}
			
		}
		
	}
	
	// slide pieces
	pceIndex = LoopSlideIndex[side];
	pce = LoopSlidePce[pceIndex];
	
	while(pce != 0){
		ASSERT(PieceValid(pce));
		
		for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));
			
			for(index = 0; index < NumDir[pce]; index++){
				dir = PceDir[pce][index];
			    t_sq = sq + dir;
				
				while(!SQOFFBOARD(t_sq)){
					
					if(pos->pieces[t_sq] != EMPTY){
						if(PieceCol[pos->pieces[t_sq]] == side ^ 1){
							// printf("\t\tCapture on %s\n", PrSq(t_sq));
							AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
						}
						break;
					}
				
					t_sq += dir;
				}
				
			}
		}
		
		pce = LoopSlidePce[++pceIndex];
	}
	
	// non slide pieces
	pceIndex = LoopNonSlideIndex[side];
	pce = LoopNonSlidePce[pceIndex];
	
	while(pce != 0){
		ASSERT(PieceValid(pce));
		
		for(pceNum = 0; pceNum < pos->pceNum[pce]; pceNum++){
			sq = pos->pList[pce][pceNum];
			ASSERT(SqOnBoard(sq));
			
			for(index = 0; index < NumDir[pce]; index++){
				dir = PceDir[pce][index];
			    t_sq = sq + dir;
				
				if(SQOFFBOARD(t_sq)) continue;
				
				if(pos->pieces[t_sq] != EMPTY){
					if(PieceCol[pos->pieces[t_sq]] == side ^ 1){
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					continue;
				}
				
			}
		}
		
		pce = LoopNonSlidePce[++pceIndex];
	}
	
}

/**
 * This function generates all possible moves for a given piece on a square on board without filterating.
 *
 * @param pos The position's pointer.
 * @param list The pointer of move list.
 * @param sq The square on board.
 */
void GenerateMovesForLocation(const S_BOARD *pos, S_MOVELIST *list, const u8 sq){
	
	ASSERT(CheckBoard(pos));
	ASSERT(SqOnBoard(sq));
	
	list->count = 0;
	u8 t_sq, pce, index;
	s8 dir;
	u8 side = pos->side;
	
	pce = pos->pieces[sq];
	ASSERT(PieceValid(pce));
	
	if(PiecePawn[pce]){
		if(side == WHITE ){
			// white pawn
			
			// quite moves
			if(pos->pieces[sq + 10] == EMPTY){
				if(RanksBrd[SQ64(sq)] == RANK_7)
					AddQuietMove(pos, MOVE(sq, sq + 10, EMPTY, wQ, 0), list);
				else
					AddQuietMove(pos, MOVE(sq, sq + 10, EMPTY, EMPTY, 0), list);
				if(RanksBrd[SQ64(sq)] == RANK_2 && pos->pieces[sq + 20] == EMPTY) 
					AddQuietMove(pos, MOVE(sq, sq + 20, EMPTY, EMPTY, MFLAGPS), list);
			}
			
			// capture moves
			if(!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK){
				if(RanksBrd[SQ64(sq)] == RANK_7)
					AddCaptureMove(pos, MOVE(sq, sq + 9, pos->pieces[sq + 9], wQ, 0), list);
				else
					AddCaptureMove(pos, MOVE(sq, sq + 9, pos->pieces[sq + 9], EMPTY, 0), list);
			}
			
			if(!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK){
				if(RanksBrd[SQ64(sq)] == RANK_7)
					AddCaptureMove(pos, MOVE(sq, sq + 11, pos->pieces[sq + 11], wQ, 0), list);
				else
					AddCaptureMove(pos, MOVE(sq, sq + 11, pos->pieces[sq + 11], EMPTY, 0), list);
			}
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq + 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq + 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
			}
		}else{
			// black pawns
				
			// quite moves
			if(pos->pieces[sq - 10] == EMPTY){
				if(RanksBrd[SQ64(sq)] == RANK_2)
					AddQuietMove(pos, MOVE(sq, sq - 10, EMPTY, bQ, 0), list);
				else
					AddQuietMove(pos, MOVE(sq, sq - 10, EMPTY, EMPTY, 0), list);
				if(RanksBrd[SQ64(sq)] == RANK_7 && pos->pieces[sq - 20] == EMPTY) 
					AddQuietMove(pos, MOVE(sq, sq - 20, EMPTY, EMPTY, MFLAGPS), list);
			}
			
			// capture moves
			if(!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE){
				if(RanksBrd[SQ64(sq)] == RANK_2)
					AddCaptureMove(pos, MOVE(sq, sq - 9, pos->pieces[sq - 9], bQ, 0), list);
				else
					AddCaptureMove(pos, MOVE(sq, sq - 9, pos->pieces[sq - 9], EMPTY, 0), list);
			}
			
			if(!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE){
				if(RanksBrd[SQ64(sq)] == RANK_2)
					AddCaptureMove(pos, MOVE(sq, sq - 11, pos->pieces[sq - 11], bQ, 0), list);
				else
					AddCaptureMove(pos, MOVE(sq, sq - 11, pos->pieces[sq - 11], EMPTY, 0), list);
			}
			
			// en passent
			if(pos->enPass != NO_SQ){
				if(sq - 9 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
				
				if(sq - 11 == pos->enPass)
					AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
			}
		}
	
	}else{
		
		for(index = 0; index < NumDir[pce]; index++){
			dir = PceDir[pce][index];
			t_sq = sq + dir;
			
			while(!SQOFFBOARD(t_sq)){
				
				if(pos->pieces[t_sq] != EMPTY){
					if(PieceCol[pos->pieces[t_sq]] == side ^ 1){
						AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
					}
					break;
				}
			
				AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
				t_sq += dir;
				
				if(IsKi(pce) ||  IsKn(pce)) break;
			}
			
		}
		
		if(pce == wK && sq == E1){
			// castling
			// white king castling
			if(pos->castlePerm & WKCA){
				if(pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY){
					if(!IsSqAttacked(E1, BLACK, pos) && !IsSqAttacked(F1, BLACK, pos)){
						AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
					}
				}
			}
			
			// white quuen castling
			if(pos->castlePerm & WQCA){
				if(pos->pieces[B1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[D1] == EMPTY){
					if(!IsSqAttacked(E1, BLACK, pos) && !IsSqAttacked(D1, BLACK, pos)){
						AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
					}
				}
			}
		}else if(pce == bK && sq == E8){
			// castling
			// black king castling
			if(pos->castlePerm & BKCA){
				if(pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY){
					if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(F8, WHITE, pos)){
						AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
					}
				}
			}
			
			// black quuen castling
			if(pos->castlePerm & BQCA){
				if(pos->pieces[B8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[D8] == EMPTY){
					if(!IsSqAttacked(E8, WHITE, pos) && !IsSqAttacked(D8, WHITE, pos)){
						AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
					}
				}
			}
		}
	}
	
}







