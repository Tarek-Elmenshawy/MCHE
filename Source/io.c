/**
 * @file io.c
 * This file includes methods that are responsile for preparing 
 * information to be printed on colsole and vice versa.             
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

/**
 * A function to take a move in algebric form (ex. e2e4) and convert it to 
 * an integer form (i.e., the form used in seraching process).
 *
 * @param ptrchar The pinter to string (ex. e2e4, e7e8q).
 * @param pos The position's pointer.
 * @return move
 */
s32 ParseMove(const char *ptrchar, S_BOARD *pos){
	
	ASSERT(CheckBoard(pos));
	
	if(ptrchar[0] > 'h' || ptrchar[0] < 'a') return NOMOVE;
	if(ptrchar[2] > 'h' || ptrchar[2] < 'a') return NOMOVE;
	if(ptrchar[1] > '8' || ptrchar[1] < '1') return NOMOVE;
	if(ptrchar[3] > '8' || ptrchar[3] < '1') return NOMOVE;
	
	u8 from = FR2SQ(ptrchar[0] - 'a', ptrchar[1] - '1');
	u8 to = FR2SQ(ptrchar[2] - 'a', ptrchar[3] - '1');
	
	ASSERT(SqOnBoard(from));
	ASSERT(SqOnBoard(to));
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	
	u8 moveNum, promoted;
	s32 move;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		
		move = list->moves[moveNum].move;
		
		if(from == FROMSQ(move) && to == TOSQ(move)){
			
			promoted = PROMOTED(move);
			if(promoted != EMPTY){
				
				if(IsKn(promoted) && ptrchar[4] == 'n') return move;
				else if(IsRQ(promoted) && IsBQ(promoted) && ptrchar[4] == 'q') return move;
				else if(IsRQ(promoted) && !IsBQ(promoted) && ptrchar[4] == 'r') return move;
				else if(!IsRQ(promoted) && IsBQ(promoted) && ptrchar[4] == 'b') return move;
				
				continue;
			}
			
			return move;
		}
		
	}
	
	return NOMOVE;
}

/**
 * A function to take an integer number which represents a squar on the board,
 * converts it into an algebraic form (ex. a1) as string and returns a pointer
 * that points to the array that contains the algebraic form.
 *
 * @param sq The square on the board.
 * @return a pointer to string. 
 */
char *PrSq(const u8 sq){
	static char SqStr[3];
	
	u8 file = FilesBrd[SQ64(sq)];
	u8 rank = RanksBrd[SQ64(sq)];
	
	if(sq != NO_SQ) sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));
	else sprintf(SqStr, "%c%c", '-', '-');
	
	return SqStr;
}

/**
 * A function to take an integer number which represents a move on the board,
 * converts it into an algebraic form (ex. a1a8) as string and returns a pointer
 * that points to the array that contains the algebraic form.
 * 
 * @param move
 * @return a pointer to string. 
 */
char *PrMove(const s32 move){
	static char MvStr[6];
	
	u8 ff = FilesBrd[SQ64(FROMSQ(move))];
	u8 rf = RanksBrd[SQ64(FROMSQ(move))];
	u8 ft = FilesBrd[SQ64(TOSQ(move))];
	u8 rt = RanksBrd[SQ64(TOSQ(move))];
	
	u8 promoted = PROMOTED(move);
	if(promoted){
		char pchar ='q';
		if(IsKn(promoted)) pchar = 'n';
		else if(IsRQ(promoted) && !IsBQ(promoted)) pchar = 'r';
		else if(!IsRQ(promoted) && IsBQ(promoted)) pchar = 'b';
		
		sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	}
	else sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt)); 
	
	return MvStr;
}

/**
 * A function to take an integer number which represents a move on the board,
 * converts it into a custom algebraic form (ex. -Na1a8) as string and returns a pointer
 * that points to the array that contains the algebraic form.
 * 
 * @param move
 * @return a pointer to string. 
 */
char *SpePrMove(const s32 move){
	static char MvStr[6];
	
	u8 ff = FilesBrd[SQ64(FROMSQ(move))];
	u8 rf = RanksBrd[SQ64(FROMSQ(move))];
	u8 ft = FilesBrd[SQ64(TOSQ(move))];
	u8 rt = RanksBrd[SQ64(TOSQ(move))];
	
	u8 promoted = PROMOTED(move);
	u8 captured = (move & MFLAGCAP)? 1 : 0 ;
	
	char tchar = 'N';
	if(MFLAGCA & move) tchar = 'C';
	if(MFLAGEP & move) tchar = 'E';
	if(promoted) tchar = 'P';
	
	sprintf(MvStr, "%c%c%c%c%c%c", captured? 'X':'-', tchar, ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));                
	
	return MvStr;
}

/**
 * A function to print a given move list.
 * 
 * @param list The pointer of move list.
 */
void PrintMoveList(const S_MOVELIST *list){
	u8 index;
	s32 move, score;
	
	printf("MoveList:\n");
	
	for(index = 0; index < list->count; index++){
		move = list->moves[index].move;
		score = list->moves[index].score;
		
		printf("Move[%-2d]: -> %s (score: %d) \n", index+1, PrMove(move), score);
	}
	printf("MoveList Total %d Moves:\n\n", list->count);
}










