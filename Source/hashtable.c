/**
 * @file hashtable.c
 * This file includes all methods that can deal with hash tables (ex. initialize, store, retrieve ,etc.).													
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

/**
 * Returns a thinking line depends on prinsiple variation moves
 * and store the thinking line in PvArray to be printed later.
 * 
 * @param depth The depth of search.
 * @param pos THe position's pointer.
 * @return count The number of moves in the thinkink line.
 */
u8 GetPvLine(const u8 depth, S_BOARD *pos){
	
	ASSERT(depth < MAXDEPTH && depth >= 1);
	
	u8 count = 0;
	s32 move = ProbePvTable(pos);
	while(move != NOMOVE && count < depth){
		
		if(MoveExists(pos, move)){
			MakeMove(pos,move);
			pos->PvArray[count++] = move;
		}else break;
		
		move = ProbePvTable(pos);
	}
	
	while(pos->ply > 0) UnMakeMove(pos);
	
	return count;
}

/**
 * Clear all the entries of the hash table to ensure thers is no garbage.
 * 
 * @param table The pointer of hash table.
 */
void ClearHashTable(S_HASHTABLE *table){
	
	S_HASHENTRY *hashEntry;
	
	for(hashEntry = table->pTableA; hashEntry < table->pTableA + table->numEntries; hashEntry++){
		
		hashEntry->posKey = 0ULL;
		hashEntry->move   = NOMOVE;
		hashEntry->depth  = 0;
		hashEntry->score  = 0;
		hashEntry->flag  = 0;
	}
	
	for(hashEntry = table->pTableD; hashEntry < table->pTableD + table->numEntries; hashEntry++){
		
		hashEntry->posKey = 0ULL;
		hashEntry->move   = NOMOVE;
		hashEntry->depth  = 0;
		hashEntry->score  = 0;
		hashEntry->flag  = 0;
	}
	
	table->newWrites = 0;
	table->overWrites = 0;
	table->hit = 0;
	table->cut = 0;
	table->pv = 0;
}

/**
 * Allocate memory for hash tables. the memory of table depends on a given
 * memory size by the user.
 *
 * NOTE: If RAM is not enough the fuction tries the given size divided by 2 until
 * tables initialization is done correctly.
 * 
 * @param table The pointer of hash table.
 * @param MB THe size of table in megabytes.
 */
void InitHashTable(S_HASHTABLE *table, const u8 MB){
	
	table->numEntries = (0x100000 * MB) / sizeof(S_HASHENTRY);
	//table->numEntries -= 2;
	
	//calling free() twice make a crash.
	
	table->pTableA = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
	table->pTableD = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
	SetColor(LIGHT_RED);
	if(table->pTableA == NULL || table->pTableD == NULL){
		if(MB == 1){
			printf("INFO: HashTable Allocation Faild, memory isn't enough!!!");
			exit(1);
		}
		printf("INFO: HashTable Allocation Faild, trying %d MB........\n", MB/2);
		InitHashTable(table, MB/2);
	}else{
		ClearHashTable(table);
		printf("INFO: HashTable Init Complete With %d Entries(%d MB).\n", table->numEntries, MB);
	}
	SetColor(LIGHT_GRAY);
	
}

/**
 * Probe a hash entry in the two hash tables and returns entry information of a given position.
 * 
 * NOTE: The used replacement strategy is two-tier repplacement schema.
 * TableA -> always replace
 * TableD -> replace if case of the new entry has a depth deeper than that stored
 * 
 * @param pos THe position's pointer.
 * @param move The pointer of variable to retrieve the stored move for the given position.
 * @param score The pointer of variable to retrieve the score of move.
 * @param alpha The value of alpha for the given search.
 * @param beta The value of beta for the given search.
 * @param depth The depth of search.
 * @return TRUE if a stored entry found, FALSE if not.
 */
u8 ProbeHashEntry(S_BOARD *pos, s32 *move, s16 *score, const s16 alpha, const s16  beta, const u8 depth){
	
	u32 index = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1)
	ASSERT(alpha < beta);
	ASSERT(alpha >= -INFINITY && alpha <= INFINITY);
	ASSERT(beta >= -INFINITY && beta <= INFINITY);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	if(pos->HashTable->pTableA[index].posKey == pos->posKey){
		*move = pos->HashTable->pTableA[index].move;
		
		if(pos->HashTable->pTableA[index].depth >= depth){
			pos->HashTable->hit++;
			
			ASSERT(pos->HashTable->pTableA[index].depth >= 1 && pos->HashTable->pTableA[index].depth < MAXDEPTH);
			ASSERT(pos->HashTable->pTableA[index].flag >= HFNONE && pos->HashTable->pTableA[index].flag <= HFEXACT);                                                                        
			
			*score = pos->HashTable->pTableA[index].score;
			if(*score > ISMATE) *score -= pos->ply;
			else if(*score < -ISMATE) *score += pos->ply;
			
			switch(pos->HashTable->pTableA[index].flag){
				
				ASSERT(*score >= -INFINITY && *score <= INFINITY);
				
				case HFEXACT:
					return TRUE;
				
				case HFALPHA:
					if(*score <= alpha){
						*score = alpha;
						return TRUE;
					}
					break;
				
				case HFBETA:
					if(*score >= beta){
						*score = beta;
						return TRUE;
					}
					break;
				
				default: ASSERT(FALSE);
			}
			
		}
	}else if(pos->HashTable->pTableD[index].posKey == pos->posKey){
		*move = pos->HashTable->pTableD[index].move;
		
		if(pos->HashTable->pTableD[index].depth >= depth){
			pos->HashTable->hit++;
			
			ASSERT(pos->HashTable->pTableD[index].depth >= 1 && pos->HashTable->pTableD[index].depth < MAXDEPTH);
			ASSERT(pos->HashTable->pTableD[index].flag >= HFNONE && pos->HashTable->pTableD[index].flag <= HFEXACT);                                                                        
			
			*score = pos->HashTable->pTableD[index].score;
			if(*score > ISMATE) *score -= pos->ply;
			else if(*score < -ISMATE) *score += pos->ply;
			
			switch(pos->HashTable->pTableD[index].flag){
				
				ASSERT(*score >= -INFINITY && *score <= INFINITY);
				
				case HFEXACT:
					return TRUE;
				
				case HFALPHA:
					if(*score <= alpha){
						*score = alpha;
						return TRUE;
					}
					break;
				
				case HFBETA:
					if(*score >= beta){
						*score = beta;
						return TRUE;
					}
					break;
				
				default: ASSERT(FALSE);
			}
			
		}
	}
	
	return FALSE;
	
}

/**
 * Store a hash entry in the two hash tables.
 * 
 * NOTE: The used replacement strategy is two-tier repplacement schema.
 * TableA -> always replace
 * TableD -> replace if case of the new entry has a depth deeper than that stored
 * 
 * @param pos THe position's pointer.
 * @param move The move to be stored for the given position.
 * @param score The score of the given move.
 * @param flag The hash table flag related to the given move (HFALPHA, HFBETA, HFEXACT).
 * @param depth The depth of search.
 */
void StoreHashEntry(S_BOARD *pos, const s32 move, s16 score, const u8 flag, const u8 depth){
	
	u32 index = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
	ASSERT(score >= -INFINITY && score <= INFINITY);
	ASSERT(flag >= HFNONE && flag <= HFEXACT);
	ASSERT(depth >= 1 && depth < MAXDEPTH);
	ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
	
	//Reset MATE score to INFINITY so we can adjuxt and use it in shallower search
	if(score > ISMATE) score += pos->ply;
	else if(score < -ISMATE) score -= pos->ply;
	
	if(pos->HashTable->pTableA[index].posKey == 0ULL){
		
		pos->HashTable->newWrites++;
		
		pos->HashTable->pTableD[index].posKey = pos->posKey;
		pos->HashTable->pTableD[index].move = move;
		pos->HashTable->pTableD[index].score = score;
		pos->HashTable->pTableD[index].flag = flag;
		pos->HashTable->pTableD[index].depth = depth;
		
	}else if(pos->HashTable->pTableD[index].depth <= depth){
		
		pos->HashTable->overWrites++;
		
		pos->HashTable->pTableD[index].posKey = pos->posKey;
		pos->HashTable->pTableD[index].move = move;
		pos->HashTable->pTableD[index].score = score;
		pos->HashTable->pTableD[index].flag = flag;
		pos->HashTable->pTableD[index].depth = depth;
		
	} 
	
	pos->HashTable->pTableA[index].posKey = pos->posKey;
	pos->HashTable->pTableA[index].move = move;
	pos->HashTable->pTableA[index].score = score;
	pos->HashTable->pTableA[index].flag = flag;
	pos->HashTable->pTableA[index].depth = depth;
	
}

/**
 * Returns the pv move stored in hash tables for a given position.
 * 
 * NOTE: The used replacement strategy is two-tier repplacement schema.
 * TableA -> always replace
 * TableD -> replace if case of the new entry has a depth deeper than that stored
 *
 * @param pos THe position's pointer.
 * @return move The pv move.
 */
s32 ProbePvTable(const S_BOARD *pos){
	
	u32 index = pos->posKey % pos->HashTable->numEntries;
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
	
	if(pos->HashTable->pTableA[index].posKey == pos->posKey){
		return pos->HashTable->pTableA[index].move;
	}else if(pos->HashTable->pTableD[index].posKey == pos->posKey){
		return pos->HashTable->pTableD[index].move;
	}else return NOMOVE;
	
}

/**
 * Clear all the entries of the evaluation table to ensure thers is no garbage.
 * 
 * @param table The pointer of evaluation tables.
 */
void ClearEvalTable(S_EVALTABLE *table){
	
	S_PAWNENTRY *pawnEntry;
	
	for(pawnEntry = table->pPawnTable; pawnEntry < table->pPawnTable + table->numEntriesPawn; pawnEntry++){
		
		pawnEntry->pawnKey = 0ULL;
		pawnEntry->whiteStructure   = 0;
		pawnEntry->blackStructure  = 0;
		pawnEntry->passers  = 0;
	}
	
	S_EVALENTRY *evalEntry;
	
	for(evalEntry = table->pEvalTable; evalEntry < table->pEvalTable + table->numEntriesEval; evalEntry++){
		
		evalEntry->posKey = 0ULL;
		evalEntry->eval   = 0;
	
	}

}

/**
 * Allocate memory for evaluation tables. the memory of table depends on a given
 * memory size by the user.
 *
 * NOTE: If RAM is not enough the fuction tries the given size divided by 2 until
 * tables initialization is done correctly.
 * 
 * @param table The pointer of evalaution table.
 * @param MB_PAWN THe size of pawn structure evaluation table in megabytes.
 * @param MB_EVAL THe size of position evaluation table in megabytes.
 */
void InitEvalTable(S_EVALTABLE *table, const u8 MB_PAWN, const u8 MB_EVAL){
	
	table->numEntriesPawn = (0x100000 * MB_PAWN) / sizeof(S_PAWNENTRY);
	table->numEntriesEval = (0x100000 * MB_EVAL) / sizeof(S_EVALENTRY);
	
	//calling free() twice make a crash.
	
	table->pPawnTable = (S_PAWNENTRY *) malloc(table->numEntriesPawn * sizeof(S_PAWNENTRY));
	table->pEvalTable = (S_EVALENTRY *) malloc(table->numEntriesEval * sizeof(S_EVALENTRY));
	
	SetColor(LIGHT_RED);
	if(table->pPawnTable == NULL || table->pEvalTable == NULL){
		if(MB_PAWN == 1 || MB_EVAL == 1){
			printf("INFO: EvalTable Allocation Faild, memory isn't enough!!!");
			exit(1);
		}
		printf("INFO: PawnTable Allocation Faild, trying %d MB........\n", MB_PAWN/2);
		printf("INFO: EvalTable Allocation Faild, trying %d MB........\n", MB_EVAL/2);
		InitEvalTable(table, MB_PAWN/2, MB_EVAL/2);
	}else{
		ClearEvalTable(table);
		printf("INFO: PawnTable Init Complete With %d Entries(%d MB).\n", table->numEntriesPawn, MB_PAWN);
		printf("INFO: EvalTable Init Complete With %d Entries(%d MB).\n\n", table->numEntriesEval, MB_EVAL);
	}
	SetColor(LIGHT_GRAY);
	
}

/**
 * Free all allocated memory of hash tables, evaluation tables and
 * memory used for opening book
 * 
 * @param hashTable The pointer of hash tables.
 * @param evalTable The pointer of evluation tables.
 */
void CleanMemory(S_HASHTABLE *hashTable, S_EVALTABLE *evalTable){
	free(hashTable->pTableA);
	free(hashTable->pTableD);
	
	free(evalTable->pPawnTable);
	free(evalTable->pEvalTable);
	
	CleanPolyBook();
}

/**
 * Probe an evaluation entry in the evaluation tables and returns entry information of a given position.
 * 
 * @param pos THe position's pointer.
 * @param eval The pointer of variable to retrieve the evaluation value of the given position.
 * @return TRUE if a stored entry found, FALSE if not.
 */
u8 ProbeEval(S_BOARD *pos, s16 *eval){
	
	u32 index = pos->posKey % pos->EvalTable->numEntriesEval;
	ASSERT(index >= 0 && index <= pos->EvalTable->numEntriesEval - 1);
	
	if(pos->EvalTable->pEvalTable[index].posKey == pos->posKey){
		if(pos->side == WHITE) *eval = pos->EvalTable->pEvalTable[index].eval;
		else *eval = -1 * pos->EvalTable->pEvalTable[index].eval;
		
		return TRUE;
	}
	
	return FALSE;
}

/**
 * Probe a pawn structure evaluation entry in the pawn structure tables and returns entry information of a given position.
 * 
 * @param pos THe position's pointer.
 * @param whiteStructure The pointer of variable to retrieve the white pawn structure evaluation value of the given position.
 * @param blackStructure The pointer of variable to retrieve the black pawn structure evaluation value of the given position.
 * @param passers The pointer of variable to retrieve the passer structure.
 * @return TRUE if a stored entry found, FALSE if not.
 */
u8 ProbePawnEval(S_BOARD *pos, s8 *whiteStructure, s8 *blackStructure, u16 *passers){
	
	u32 index = pos->pawnKey % pos->EvalTable->numEntriesPawn;
	ASSERT(index >= 0 && index <= pos->EvalTable->numEntriesPawn - 1);
	
	if(pos->EvalTable->pPawnTable[index].pawnKey == pos->pawnKey){
		*whiteStructure = pos->EvalTable->pPawnTable[index].whiteStructure;
		*blackStructure = pos->EvalTable->pPawnTable[index].blackStructure;
		*passers = pos->EvalTable->pPawnTable[index].passers;
		return TRUE;
	}
	
	return FALSE;
}

/**
 * Store the pawn structure evaluation value of a given position.
 *
 * @param pos THe position's pointer.
 * @param eval The evaluation value to be stored for the given position.
 */
void StoreEval(S_BOARD *pos, s16 eval){
	
	u32 index = pos->posKey % pos->EvalTable->numEntriesEval;
	ASSERT(index >= 0 && index <= pos->EvalTable->numEntriesEval - 1);
	
	pos->EvalTable->pEvalTable[index].posKey = pos->posKey;
	pos->EvalTable->pEvalTable[index].eval = eval;
}

/**
 * Store the evaluation value of a given position.
 *
 * @param pos THe position's pointer.
 * @param whiteStructure The white pawn structure evaluation value of the given position.
 * @param blackStructure The black pawn structure evaluation value of the given position.
 * @param passers The passer structure.
 */
void StorePawnEval(S_BOARD *pos, s8 whiteStructure, s8 blackStructure, u16 passers){
	
	u32 index = pos->pawnKey % pos->EvalTable->numEntriesPawn;
	ASSERT(index >= 0 && index <= pos->EvalTable->numEntriesPawn - 1);
	
	pos->EvalTable->pPawnTable[index].pawnKey = pos->pawnKey;
	pos->EvalTable->pPawnTable[index].whiteStructure = whiteStructure;
	pos->EvalTable->pPawnTable[index].blackStructure = blackStructure;
	pos->EvalTable->pPawnTable[index].passers = passers;
}







