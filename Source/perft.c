/** 
 * @file perft.c
 * This file includes fuctions used in performace test.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

extern const u8  VictimScores[13];
extern const u8 piece_values[13];
static u64 leafNodes;

static void Perft(const u8 depth, S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	
	if(depth == 0){
		leafNodes++;
		return;
	}
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	
	u8 moveNum;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		if(!MakeMove(pos, list->moves[moveNum].move)) continue;
		Perft(depth - 1, pos);
		UnMakeMove(pos);
		
	}
	
	return;
	
}

/**
 * A debugging function to walk the move generation tree of strictly legal moves to count 
 * all the leaf nodes of a certain depth, which can be compared to predetermined values
 * and used to isolate bugs.
 * 
 * @param depth 
 * @param pos The position's pointer.
 * @return leafNodes The number of visited leaf nodes.
 */
u64 PerftTest(const u8 depth, S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	
	printf("\nStarting search in depth %d:\n", depth);
	
	leafNodes = 0;
	s32 start = GetTimeMs();
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	
	u8 moveNum;
	
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		s32 move =list->moves[moveNum].move;
		if(!MakeMove(pos, move)) continue;
		long comuNodes = leafNodes;
		Perft(depth - 1, pos);
		UnMakeMove(pos);
		long branNodes = leafNodes - comuNodes;		
	}
	
	printf("Test compelet: %lld visited nodes in %ld ms\n", leafNodes, GetTimeMs() - start);
	
	return leafNodes;
	
}

/**
 * Sorting a list of moves in descending order denpending on move scores.
 * 
 * It's a simple bubble sort algorithm.
 *
 * @param list A pointer to a move list.
 * @param first the index of the first element in the move list array.
 * @param last The index of the last element in the move list array.
 */
void QuickSort(S_MOVELIST *list, s16 first, s16 last){
	
	s8 i, j, pivot;
	S_MOVE temp;

	if(first < last){
		pivot = last;
		i = first;
		j = last;

		while(i < j){
			// Find the index of the move that has a smaller score than which pivot has.
			while(list->moves[i].score >= list->moves[pivot].score && i < last) i++;
			
			// Find the index of the move that has a greater score than which pivot has.
			while(list->moves[j].score < list->moves[pivot].score) j--;
			
			// Swap the two moves.
			if(i < j){
				temp = list->moves[i];
				list->moves[i] = list->moves[j];
				list->moves[j] = temp;
			}
		}
		
		// Swaping pivot to prepare for partitions.
		temp = list->moves[pivot];
		list->moves[pivot] = list->moves[j];
		list->moves[j] = temp;
		
		
		// Call the funtion again to repaet the same process for the right partition. 
		QuickSort(list, first, j-1);
		// Call the funtion again to repaet the same process for the left partition. 
		QuickSort(list, j+1, last);

	}
}
/**
 *  This function is uesd to generates all captures moves for the given board position, 
 *  calculate all capture value using SEE() then compared result to the result of the used test suites.
 *
 *  @param pos The position's pointer.
 *  @return The move of higher.
 */
s32 SEETest(const S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	
	printf("\nStarting SEE test:\n");
	
	leafNodes = 0;
	
	long start = GetTimeMs();
	S_MOVELIST list[1];
	GenerateAllCaps(pos, list);
	ASSERT(CapturesListOk(list, pos));
	u8 moveNum;

	for(moveNum = 0; moveNum < list->count; moveNum++){
		list->moves[moveNum].score = SEE(pos, list->moves[moveNum].move);
	}
	
	if(list->count != 0 && list->count != 1) QuickSort(list, 0, list->count - 1);
	u8 index[20], indexCounter = 0;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		if(list->moves[moveNum].score < 0) continue;
		if(moveNum == 0){
			if(list->moves[moveNum].score == list->moves[moveNum + 1].score){
				index[indexCounter++] = moveNum;
				index[indexCounter++] = moveNum + 1;
				moveNum++;
			}
		}
		else if(list->moves[moveNum].score == list->moves[moveNum + 1].score){
			index[indexCounter++] = moveNum;
			index[indexCounter++] = moveNum + 1;
			moveNum++;
		}else if(list->moves[moveNum - 1].score == list->moves[moveNum].score){
			index[indexCounter++] = moveNum;
		}
	}
	
	if(indexCounter){
		printf("indexes have the same score:");
		for(moveNum = 0; moveNum < indexCounter; moveNum++) printf("%d ", index[moveNum]);
	}
	
	printf("\nCaptures Moves: %d\n", list->count);
	for(moveNum = 0; moveNum < list->count; moveNum++){
		printf("cap[%d]: %s : %d\n", moveNum, PrMove(list->moves[moveNum].move), list->moves[moveNum].score);         
	}
	
	for(moveNum = 0; moveNum < indexCounter; moveNum++){
		s32 move = list->moves[index[moveNum]].move;
		list->moves[index[moveNum]].score = (VictimScores[CAPTURED(move)]*10) + 6 - (VictimScores[pos->pieces[FROMSQ(move)]]/10);            
		if(PROMOTED(move) != EMPTY) list->moves[index[moveNum]].score += piece_values[PROMOTED(move)];
	}
	
	if(list->count != 0 && list->count != 1) QuickSort(list, 0, list->count - 1);
	printf("\nCaptures Moves After Adjustment: %d\n", list->count);
	for(moveNum = 0; moveNum < list->count; moveNum++){
		printf("cap[%d]: %s : %d\n", moveNum, PrMove(list->moves[moveNum].move), list->moves[moveNum].score);         
	}

	printf("\nTest compelet in %ldms \n", GetTimeMs() - start);
	
	return list->moves[0].move;
	
}










