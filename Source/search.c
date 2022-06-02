/**
 * @file search.c
 * This file includes all the used searching methods.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

#define MATE (pos->ply - INFINITY)        ///< Constant value represents mate state after plys.
extern const u8 piece_values[13];
extern void QuickSort(S_MOVELIST *list, s16 first, s16 last);

/**
 * This method returns the game phase.
 * 
 * @param pos The position's pointer to decide game phase on.
 * @return gamePhase The phase the position on the board is in.
 */	
u8 GetGamePhase(const S_BOARD *pos){
	u8 phase = PHASE_OPENING; 							// Initialize to opening
	u8 gamePhaseCheck = 0; 								// Initialize the count

	gamePhaseCheck += pos->pceNum[wN];
	gamePhaseCheck += pos->pceNum[bN];
	gamePhaseCheck += pos->pceNum[wB];
	gamePhaseCheck += pos->pceNum[bB];
	gamePhaseCheck += pos->pceNum[wR] * 2;
	gamePhaseCheck += pos->pceNum[bR] * 2;
	gamePhaseCheck += pos->pceNum[wQ] * 4;
	gamePhaseCheck += pos->pceNum[bQ] * 4;
	
	
	if(gamePhaseCheck == 0) phase = PHASE_PAWN_ENDING;
	else if(gamePhaseCheck <= 8) phase = PHASE_ENDING;
	else if(gamePhaseCheck >20) phase = PHASE_OPENING;
	else phase = PHASE_MIDDLE;
	
	return phase;
}

static void PickNextMove(u8 moveNum, S_MOVELIST *list){
	
	ASSERT(moveNum >= 0 && moveNum < list->count);
	
	u8 index;
	s32 bestScore = 0;
	u8 bestNum = moveNum;
	
	for(index = moveNum; index < list->count; index++){
		if(list->moves[index].score > bestScore){
			bestScore = list->moves[index].score;
			bestNum = index;
		}
	}
	
	ASSERT(bestNum >= 0 && bestNum < list->count);
	
	S_MOVE temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
	
}

static void CheckUp(S_SEARCHINFO *info){
	// Checking time up, interrupts from GUI
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime) info->stopped = TRUE;
	if(info->ponder == TRUE) ReadInput(info);
}

static u8 IsRepetition(const S_BOARD *pos){
	
	s32 index;
	
	// Start with hisply which has fifty move = 0 
	// becaues the board couldn't have the same position again 
	// where a pawn gwebt forward or a piece is captured.                     
	for(index = pos->hisply - pos->fiftyMove; index < pos->hisply - 1; index++){
		ASSERT(index >= 0 && index < MAXGAMEMOVES);
		if(pos->posKey == pos->history[index].posKey) return TRUE;
	}
	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info){
	
	u8 index, index2;
	
	// Clear serach history values.
	for(index = 0; index < 13; index++){
		for(index2 = 0; index2 < 64; index2++)
			pos->searchHistory[index][index2] = 0;
	}
	
	// Clear serach killers values.
	for(index = 0; index < 2; index++){
		for(index2 = 0; index2 < MAXDEPTH; index2++)
			pos->searchKillers[index][index2] = 0;
	}
	
	// Reset ply to zero in the begaining of search.
	pos->ply = 0;
	
	// Reset hash table feedback variables to zero in the begaining of search.
	pos->HashTable->overWrites = 0;
	pos->HashTable->hit = 0;
	pos->HashTable->cut = 0;
	pos->HashTable->pv = 0;
	
	// Reset chess engine information flags and feedback varuables.
	info->stopped = FALSE;
	info->nodes = 0;
	info->fhf = 0;
	info->fh = 0;
	
	// Reset evaluation tables feedback valiables.
	pos->EvalTable->evalhits = 0;
	pos->EvalTable->pawnhits = 0;
	
}

/**
 *  Quiescent search, looks at captures passed the search depth.
 *
 *  @param alpha Alpha value
 *  @param beta Beta value
 *  @param pos The position's pointer.
 *  @param info The pinter of the engine information stucture.
 *  @param contemptFactor
 *  @return s16 The expected evaluation
 */
static s16 Quiescence(s16 alpha, s16 beta, S_BOARD *pos, S_SEARCHINFO *info, u8 contemptFactor){
	
	ASSERT(CheckBoard(pos));
	ASSERT(beta > alpha);

	if(!(info->nodes & 2047)) CheckUp(info);                               		// check if search time is out or not every 2048 nodes
	
	info->nodes++;
	
	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply){               // Check if there is repetition or fifty move rule is broken out which leads to draw                                        
		if(pos->ply % 2 == 0) return -contemptFactor;
		else return contemptFactor;
	}
	
	if(pos->ply > MAXDEPTH - 1) return EvalPosition(pos);
	
	s16 score = EvalPosition(pos);
	ASSERT(score >= -INFINITY && score <= INFINITY);
	
	if(score >= beta) return beta;
	if(score > alpha) alpha = score;
	
	// Generate all capture moves 
	S_MOVELIST list[1];
	GenerateAllCaps(pos,list);
	ASSERT(CapturesListOk(list, pos));
	
	u8 moveNum, legal = 0;
	score = -INFINITY;
	
	// Evaluate each capture move using Static Exchange Evaluation
	// to neglect all bad capture moves that cost more search time.
	for(moveNum = 0; moveNum < list->count; moveNum++){
		s32 move = list->moves[moveNum].move;
		list->moves[moveNum].score = SEE(pos, move);
		if(PROMOTED(move) != EMPTY) list->moves[moveNum].score += piece_values[PROMOTED(move)];
	}
	
	// Sort move depending on moves scores.
	if(list->count != 0 && list->count != 1) QuickSort(list, 0, list->count - 1);
	
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		// Neglect all bad moves that cost more time.
		if(list->moves[moveNum].score < 0) continue;
		
		if(!MakeMove(pos, list->moves[moveNum].move)) continue;
		
		legal++;
		
		score = -Quiescence(-beta, -alpha, pos, info, contemptFactor);
		UnMakeMove(pos);
		
		if(info->stopped == TRUE) return 0;
		
		if(score > alpha){
			
			if(score >= beta){
				if(legal == 1) info->fhf++;
				info->fh++;
				return beta;
			}
			alpha = score;
		}
	}

	return alpha;
}

static s16 AlphaBeta(s16 alpha, s16 beta, u8 depth, S_BOARD *pos, S_SEARCHINFO *info, u8 DoNull, u8 contemptFactor){
	
	ASSERT(CheckBoard(pos));
	ASSERT(beta > alpha);
	ASSERT(depth >= 0);
	
	if(depth <= 0){
		return Quiescence(alpha, beta, pos, info, contemptFactor);
	}
	
	if(!(info->nodes & 2047)) CheckUp(info);                               // check if search time is out or not every 2048 nodes
	
	info->nodes++;
	
	/* 
	 *  Improvement:
	 *	 In cheakmate case, we need a specific function that generates all possible moves to prevent checkmate 
	 *	 instead of generating all moves for all pieces and which is discarded by IsSqAttach(). 
	 */
	
	// If the side to move is in check ,extend the depth 
	// for more searching to take a more accurate decision
	u8 InCheck = IsSqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos);
	if(InCheck == TRUE) depth++;
	
	// Check if there is repetition or fifty move rule is broken out which leads to draw   
	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply){                                     
		if(pos->ply % 2 == 0) return -contemptFactor;
		else return contemptFactor;
	}
	
	// if we get the maximum depth, return the evaluation value of the position
	if(pos->ply > MAXDEPTH - 1) return EvalPosition(pos);
	
	s16 score = -INFINITY;
	s32 pvMove = NOMOVE;
	
	#ifdef USE_TABLES
	if(ProbeHashEntry(pos, &pvMove, &score, alpha, beta, depth)){
		pos->HashTable->cut ++;
		return score;
	}
	#endif
	
	u8 mateThreat = FALSE;
	
	#ifdef USE_NULLMOVE
	if(DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 1) && depth >= 4){
		
		MakeNullMove(pos);
		score = -AlphaBeta(-beta, -beta + 1, depth - 4, pos, info, FALSE, contemptFactor);
		UnMakeNullMove(pos);
		
		if(info->stopped == TRUE) return 0;
		
		if(score >= beta && abs(score) < ISMATE){
			info->nullCut++;
			return beta;
		}
		
		if(abs(score) >= ISMATE) mateThreat = TRUE;
		
	}
	#endif
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos,list);
	ASSERT(MoveListOk(list, pos));
	
	u8 moveNum, legal = 0;
	s32 bestMove = NOMOVE;
	s16 bestScore = -INFINITY;
	score = -INFINITY;
	s16 oldAlpha = alpha;
	
	// If we have a pv move, give it the highest score to be search first.
 	if(pvMove != NOMOVE){
		for(moveNum = 0; moveNum < list->count; moveNum++){
			if(pvMove == list->moves[moveNum].move){
				list->moves[moveNum].score = 2000000;
				pos->HashTable->pv++;
				break;
			}
		}
	}
	
	// Sort move depending on moves scores.
	if(list->count != 0 && list->count != 1) QuickSort(list, 0, list->count - 1);
	
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		if(!MakeMove(pos, list->moves[moveNum].move)) continue;

		u8 extend = 0;
		// if(mateThreat) extend++;
		if(pos->pieces[FROMSQ(list->moves[moveNum].move)] == wP && RanksBrd[SQ64(FROMSQ(list->moves[moveNum].move))] == RANK_7) extend++;
		else if(pos->pieces[FROMSQ(list->moves[moveNum].move)] == bP && RanksBrd[SQ64(FROMSQ(list->moves[moveNum].move))] == RANK_2) extend++;
		
		if(legal > 0){
			
			// Late Move Reduction
			if(legal >= 4 && !InCheck && extend == 0 && CAPTURED(list->moves[moveNum].move) == EMPTY && depth >= 3){
				
				score = -AlphaBeta(-alpha - 1, -alpha, depth - 2, pos, info, TRUE, contemptFactor);
				
			}else score = alpha + 1; // If the move was not reduced make sure we enter the ordinary searches below
			
			if(score > alpha){
				info->nodes--;
				
				// PVs serach
				score = -AlphaBeta(-alpha - 1, -alpha, depth - 1 + extend, pos, info, TRUE, contemptFactor);
				
				if(score > alpha && score < beta){
					info->nodes--;
					
					// Full depth search
					score = -AlphaBeta(-beta, -alpha, depth - 1 + extend, pos, info, TRUE, contemptFactor);
				}
			}
		}else{
			score = -AlphaBeta(-beta, -alpha, depth - 1 + extend, pos, info, TRUE, contemptFactor);
		}
		
		UnMakeMove(pos);
		
		legal++;
		
		if(info->stopped == TRUE) return 0;
		
		if(score > bestScore){
			bestScore = score;
			bestMove = list->moves[moveNum].move;
			
			if(score > alpha){				
				if(score >= beta){
					
					if(legal == 1) info->fhf++;
					info->fh++;
					
					if(!(bestMove & MFLAGCAP)){
						pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
						pos->searchKillers[0][pos->ply] = bestMove;
					}
					
					StoreHashEntry(pos, bestMove, beta, HFBETA,depth);
					
					return beta;
					
				}
				
				if(!(bestMove & MFLAGCAP)){
					pos->searchHistory[pos->pieces[FROMSQ(bestMove)]][SQ64(TOSQ(bestMove))] += depth; 
				} 
				
				alpha = score;
			}
		}
	}
	
	if(legal == 0){
		if(InCheck) return MATE;   // Mate
		else return 0;             // Stalemate
	}
	
	if(alpha != oldAlpha) StoreHashEntry(pos, bestMove, bestScore, HFEXACT, depth);
	else StoreHashEntry(pos, bestMove, alpha, HFALPHA, depth);
	
	return alpha;
}

static void Pondering(S_BOARD *pos, S_SEARCHINFO *info){
	s32 bestMove = NOMOVE;
	s16 bestScore = -INFINITY;
	u8 currentDepth;
	u8 pvMoves, pvNum;
	
	ClearForSearch(pos, info);
	
	if(EngineOptions->UseBook == TRUE) bestMove = GetBookMove(pos);
	
	//  Decide the contempt factor by looking at the phase the game is in
	u8 gamePhase = GetGamePhase(pos);
	u8 contemptFactor;
	
	if(gamePhase == PHASE_OPENING) contemptFactor = CONTEMPT_OPENING;
	else if(gamePhase == PHASE_ENDING || gamePhase == PHASE_PAWN_ENDING) contemptFactor = CONTEMPT_ENDING;
	else contemptFactor = CONTEMPT_MIDDLE;
	
	s16 alpha = -INFINITY;
	s16 beta = INFINITY;
	
	info->timeset = FALSE;
	u8 tempDepth = info->depth;
	info->depth = MAXDEPTH;
	
	if(bestMove == NOMOVE){
		// Iterative deepening
		for(currentDepth = 1; currentDepth <= info->depth;){
		
			u32 f = GetTimeMs();
			bestScore = AlphaBeta(alpha, beta, currentDepth, pos, info, TRUE, contemptFactor);
			u32 l = GetTimeMs();
			
			if(info->stopped == TRUE) break;
		
			pvMoves = GetPvLine(currentDepth, pos);
			bestMove = pos->PvArray[0];
			
			currentDepth++;
		}
	}
	
	info->depth = tempDepth ;
	
}

/**
 * The main function to find the best move for a given position.
 *
 * @param pos The position's pointer.
 * @param info The pinter of the engine information stucture.
 */
void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info){
	
	s32 bestMove = NOMOVE;
	s32 predictMove = NOMOVE;
	s16 bestScore = -INFINITY;
	u8 currentDepth;
	u8 pvMoves, pvNum, bookMove = FALSE;
	
	ClearForSearch(pos, info);
	
	if(EngineOptions->UseBook == TRUE){
		bestMove = GetBookMove(pos);
		bookMove = TRUE;
	}	
	
	//  Decide the contempt factor by looking at the phase the game is in
	u8 gamePhase = GetGamePhase(pos);
	u8 contemptFactor;
	
	if(gamePhase == PHASE_OPENING) contemptFactor = CONTEMPT_OPENING;
	else if(gamePhase == PHASE_ENDING || gamePhase == PHASE_PAWN_ENDING) contemptFactor = CONTEMPT_ENDING;
	else contemptFactor = CONTEMPT_MIDDLE;
	
	s16 alpha = -INFINITY;
	s16 beta = INFINITY;
	
	if(bestMove == NOMOVE){
		// Iterative deepening
		for(currentDepth = 1; currentDepth <= info->depth;){
		
			u32 f = GetTimeMs();
			bestScore = AlphaBeta(alpha, beta, currentDepth, pos, info, TRUE, contemptFactor);
			u32 l = GetTimeMs();
			
			if(info->stopped == TRUE) break;
		
			pvMoves = GetPvLine(currentDepth, pos);
			bestMove = pos->PvArray[0];
			predictMove = pos->PvArray[1];
			
			if(info->GAME_MODE == UCI_MODE){
				
				printf("info score cp %d  depth %d nodes %ld time %d ",
					bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
					
			}else if(info->GAME_MODE == XBOARD_MODE && info->POST_THINKING == TRUE){
				
				printf("%d %d %d %ld", currentDepth, bestScore, (GetTimeMs() - info->starttime) / 10, info->nodes);
			}else if(info->POST_THINKING == TRUE){
				
				printf("score:%d  depth:%d nodes:%ld time:%d(ms) p:%ld ",
					bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime, l - f);
				
			}
			
			if(info->GAME_MODE == UCI_MODE || info->POST_THINKING == TRUE){
				printf("pv");
				
				for(pvNum = 0; pvNum < pvMoves; pvNum++)
					printf(" %s", PrMove(pos->PvArray[pvNum]));
				
				printf("\n");
			}
			
			currentDepth++;
		}
	}
	
	if(info->GAME_MODE == UCI_MODE){
		printf("bestmove %s\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
	}else if(info->GAME_MODE == XBOARD_MODE){
		printf("move %s\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
	}else{
		printf("\n\n!! %s make move %s !!\n\n", NAME, PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
	
	if(info->ponder && !bookMove){
		if(info->GAME_MODE == CONSOLE_MODE) printf("\n%s > ", NAME);
		MakeMove(pos, predictMove);
		Pondering(pos, info);
		pos->ply++;
		UnMakeMove(pos);
	}
}

/** 
 * This is fuction used in DebugAnalysisTest() to check some test suites and
 * compare the result of search to the result of the test suite.
 * 
 * @param pos The position's pointer.
 * @param info The pinter of the engine information stucture.
 * @param moveTo The square wher the piece move to from test suite.
 * @param am A flag to avoid move.
 * @return bestMove The best move for the given position.
 */
s32 SearchDebug(S_BOARD *pos, S_SEARCHINFO *info, s8 *moveTo, u8 am){
	
	s32 bestMove = NOMOVE;
	s16 bestScore = -INFINITY;
	u8 currentDepth;
	u8 pvMoves, pvNum;
	
	ClearForSearch(pos, info);
	
	//  Decide the contempt factor by looking at the phase the game is in
	u8 gamePhase = GetGamePhase(pos);
	u8 contemptFactor;
	
	if(gamePhase == PHASE_OPENING) contemptFactor = CONTEMPT_OPENING;
	else if(gamePhase == PHASE_ENDING || gamePhase == PHASE_PAWN_ENDING) contemptFactor = CONTEMPT_ENDING;
	else contemptFactor = CONTEMPT_MIDDLE;
	
	s16 alpha = -INFINITY;
	s16 beta = INFINITY;
	
	for(currentDepth = 1; currentDepth <= info->depth;){
	
		u32 f = GetTimeMs();
		bestScore = AlphaBeta(alpha, beta, currentDepth, pos, info, TRUE, contemptFactor);
		u32 l = GetTimeMs();
		
		if(info->stopped == TRUE){
			SetColor(LIGHT_GREEN);
			printf("TIME OUT!");
			SetColor(LIGHT_GRAY);
			break;
		}
		
	
		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];
		
		if(info->GAME_MODE == UCI_MODE){
			
			printf("info score cp %d  depth %d nodes %ld time %d ",
				bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
				
		}else if(info->GAME_MODE == XBOARD_MODE && info->POST_THINKING == TRUE){
			
			printf("%d %d %d %ld", currentDepth, bestScore, (GetTimeMs() - info->starttime) / 10, info->nodes);
		}else if(info->POST_THINKING == TRUE){
			
			printf("score:%d  depth:%d nodes:%ld time:%d(ms) p:%ld ",
				bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime, l - f);
			
		}
		
		if(info->GAME_MODE == UCI_MODE || info->POST_THINKING == TRUE){
			printf("pv");
			
			for(pvNum = 0; pvNum < pvMoves; pvNum++)
				printf(" %s", PrMove(pos->PvArray[pvNum]));
			
			printf("\n");
		}
		
		char *sq = PrSq(TOSQ(bestMove));
		if(sq[0] == moveTo[0] && sq[1] == moveTo[1] && !am) break;

		currentDepth++;
	}
	
	if(info->GAME_MODE == UCI_MODE){
		printf("bestmove %s\n", PrMove(bestMove));
		
	}else if(info->GAME_MODE == XBOARD_MODE){
		printf("move %s\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
	}else{
		printf("\n\n!! %s make move %s !!\n\n", NAME, PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
	return bestMove;
}













