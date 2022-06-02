/** 
 * @file validate.c
 * This file includes all methods used in debugging.
 *
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"string.h"
#include"ctype.h"

u8 print = FALSE;		 ///< A flag used in evalution debugging

/**
 * This function takes a list of capture moves then check if this list
 * of mves actualy exists in the given board position or not.
 * 
 * @param list The pointer of move list. 
 * @param pos The position's pointer.
 * @return TURE if all moves matched, FALSE if not. 
 */
u8 CapturesListOk(const S_MOVELIST *list, const S_BOARD *pos){
	
	if(list->count < 0 || list->count >= MAXPOSITIONMOVES){
		printf("CapturesListOk(): count(%d) failed ",list->count);
		return FALSE;
	}
	
	u8 moveNum, from, to;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		from = FROMSQ(list->moves[moveNum].move);
		to = TOSQ(list->moves[moveNum].move);
		
		if(!SqOnBoard(to)){
			printf("CapturesListOk(): to(%d) failed ",to);
			return FALSE;
		}
		
		if(!SqOnBoard(from)){
			printf("CapturesListOk(): to(%d) failed ",from);
			return FALSE;
		}
		
		if(!PieceValid(pos->pieces[from])){
			PrintBoard(pos);
			return FALSE;
		}
		
		if(CAPTURED(list->moves[moveNum].move) != EMPTY){
			if(!PieceValid(CAPTURED(list->moves[moveNum].move))){
				PrintBoard(pos);
				return FALSE;
			}
		}else{
			if(list->moves[moveNum].move & MFLAGEP) return TRUE;
			printf("CapturesListOk(): no ccaptures(%d) failed ",CAPTURED(list->moves[moveNum].move));
			return FALSE;
		}
		
	}
	
	return TRUE;
}

/**
 * This function takes a list of moves then check if this list
 * of mves actualy exists in the given board position or not.
 * 
 * @param list The pointer of move list. 
 * @param pos The position's pointer.
 * @return TURE if all moves matched, FALSE if not. 
 */
u8 MoveListOk(const S_MOVELIST *list, const S_BOARD *pos){
	
	if(list->count < 0 || list->count >= MAXPOSITIONMOVES){
		printf("MoveListOk(): count(%d) failed ",list->count);
		return FALSE;
	}
	
	u8 moveNum, from, to;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		from = FROMSQ(list->moves[moveNum].move);
		to = TOSQ(list->moves[moveNum].move);
		
		if(!SqOnBoard(to)){
			printf("MoveListOk(): to(%d) failed ",to);
			return FALSE;
		}
		
		if(!SqOnBoard(from)){
			printf("MoveListOk(): to(%d) failed ",from);
			return FALSE;
		}
		
		if(!PieceValid(pos->pieces[from])){
			PrintBoard(pos);
			return FALSE;
		}
		
		if(CAPTURED(list->moves[moveNum].move) != EMPTY){
			if(!PieceValid(CAPTURED(list->moves[moveNum].move))){
				PrintBoard(pos);
				return FALSE;
			}
		}
		
	}
	
	return TRUE;
}

/**
 * This function takes a move then check if this move
 * can be applied on the given board position or not.
 * 
 * @param move  
 * @param pos The position's pointer.
 * @return TURE if the move matched, FALSE if not. 
 */
u8 MoveValid(const s32 move, const S_BOARD *pos){
	
	u8 from = FROMSQ(move);
	u8 to = TOSQ(move);
	
	if(!SqOnBoard(to)){
		printf("MoveValid(): to(%d) failed ",to);
		return FALSE;
	}
	
	if(!SqOnBoard(from)){
		printf("MoveValid(): to(%d) failed ",from);
		return FALSE;
	}
	
	if(!PieceValid(pos->pieces[from])){
		PrintBoard(pos);
		return FALSE;
	}
	
	if(CAPTURED(move) != EMPTY){
		if(!PieceValid(CAPTURED(move))){
			PrintBoard(pos);
			return FALSE;
		}
	}
	
	return TRUE;
}

/**
 * This function takes a square index and check if it is related
 * to 120 coordinate or not.
 * 
 * @param sq The index of boaed square.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 SqIs120(const u8 sq){
	return (sq>= 0 && sq < 120);
}

/**
 * This function takes a piece type and check if it is one
 * of available piece types, or it is EMPTY flag.
 * 
 * @param p The piece tyoe.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 PieceValidEmpty(const u8 p){
	return (p >= EMPTY && p <= bK)? 1 : 0;
}

/**
 * This function takes a piece type and check if it is one
 * of available piece types, or it is EMPTY or OFF_BOARD flags.
 * 
 * @param pce The piece tyoe.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 PceValidEmptyOffbrd(const u8 pce){
	return (PieceValidEmpty(pce) || pce == OFF_BOARD);
}

/**
 * This function takes a square index and check if it is on board
 * or out of board.
 * 
 * @param sq The index of boaed square.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 SqOnBoard(const u8 sq){
	return Sq120ToSq64[sq] == OFF_BOARD? 0 : 1;
}

/**
 * This function takes a side and check if it is one
 * of available sides (i.e., white, black) or not.
 * 
 * @param s The side color.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 SideValid(const u8 s){
	return (s == WHITE || s == BLACK)? 1 : 0;
}

/**
 * This function takes an index of file or rank and check if it is
 * valid or not.
 * 
 * @param fr The index of file or rank.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 FileRankValid(const u8 fr){
	return (fr >= 0 && fr <= 7)? 1 : 0;
}

/**
 * This function takes a type of piece and check if it is
 * valid or not.
 * 
 * @param p The piece type.  
 * @return TURE if the case matched, FALSE if not. 
 */
u8 PieceValid(const u8 p){
	if(!(p >= wP && p <= bK)){
		printf("PieceValid(): pce(%d) failed ",p);
		return 0;
	}else return 1;
}

/**
 * This function is used to make a performance test for the chess engine thinking
 * using test suites.
 * 
 * @param pos The position's pointer.
 * @param info The pointer of engine inforamtion structure.
 */
void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info){

	FILE *file;
    file = fopen("arasan20.epd","r");
    char lineIn [1024];

	info->depth = MAXDEPTH;
	info->timeset = TRUE;
	s32 time = 60000 * 30;
	s16 errors[1000] = {0};
	s16 index = 0;
	//096 is ok
	s16 er_pet[152] = {
		9, 12, 15, 17, 18, 19, 21, 29, 32, 34, 42, 44, 45, 47, 48
	};
	s32 counter = 0, errorIndex = 0;

    if(file == NULL){
        printf("File Not Found\n");
        return;
    }else{
        while(fgets (lineIn , 1024 , file) != NULL){
			info->starttime = GetTimeMs();
			info->stoptime = info->starttime + time;
			ClearHashTable(pos->HashTable);
			counter++;
			//if(re[index] != counter) continue;
			index++;
            ParseFen(lineIn, pos);
			char *ptr = strstr(lineIn, "bm");
			u8 am = 0;
			if(ptr == NULL){
				ptr = strstr(lineIn, "am");
				am = 1;
			}
			ptr += 3;
			while(!(ptr[0] >= 'a' && ptr[0] <= 'h' && ptr[1] >= '1' && ptr[1] <= '8')) ptr ++;
            printf("\n%s\n",lineIn);
			printf("time:%d start:%d stop:%d depth:%d timeset:%d moveto:%c%c\n",
				time,info->starttime,info->stoptime,info->depth,info->timeset, ptr[0], ptr[1]);
			s32 move = SearchDebug(pos, info, ptr, am);

			char *res = PrSq(TOSQ(move));
			if(!(ptr[0] == res[0] && ptr[1] == res[1])) errors[errorIndex++] = counter;
			SetColor(LIGHT_GREEN);
			printf("\nTotal:%d error:%d\n", index, errorIndex);
			SetColor(LIGHT_GRAY);
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
		
		printf("positions didn't match [%d]:\n", errorIndex);
		for(s32 i = 0; i < errorIndex; i++){
			if(i % 20 == 0) printf("\n");
			printf("%d, ", errors[i]);
		}
    }
}

/**
 * This function is used to check the performace of EvalPosition()  
 * by comparing the evaluation value of a specific board position to 
 * the same evaluation value of that board position after mirroring.
 *
 * This process is performed on test suites.
 * 
 * @param pos The position's pointer.
 */
void MirrorEvalTest(S_BOARD *pos){
    FILE *file;
    file = fopen("trapped.epd","r");
    char lineIn [2048];
    s32 ev1 = 0; s32 ev2 = 0;
    s32 positions = 0;
    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }else{
        while(fgets (lineIn , 1024 , file) != NULL){
			positions++;
			printf("\n%d  %s\n", positions,lineIn);
            ParseFen(lineIn, pos);
			PrintBoard(pos);
			printf("\n********** ORIGINAL **********\n\n");
            ev1 = EvalPosition(pos);
			printf("\n*********** MIRROR ***********\n\n");
            MirrorBoard(pos);
            ev2 = EvalPosition(pos);
			print = FALSE;
            if(ev1 != ev2) {
				print = TRUE;
                printf("\n\n\n");
                ParseFen(lineIn, pos);
                PrintBoard(pos);
				EvalPosition(pos);
                MirrorBoard(pos);
                PrintBoard(pos);
				ev1 = EvalPosition(pos);
                printf("\n\nMirror Fail:\n%s\n",lineIn);
                getchar();
                return;
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

/**
 * This function is used to check the performace of SEE().
 *
 * This process is performed on test suites.
 * 
 * @param pos The position's pointer.
 */
void DebugSEETest(S_BOARD *pos){

	FILE *file;
    file = fopen("SEE_test.epd","r");
    char lineIn [1024];

	s32 counter = 0;


    if(file == NULL){
        printf("File Not Found\n");
        return;
    }else{
        while(fgets (lineIn , 1024 , file) != NULL){

            ParseFen(lineIn, pos);
			counter++;
            printf("\n%2d\t%s\n", counter, lineIn);
			PrintBoard(pos);

			s32 move = SEETest(pos);
			printf("\nThe 1st capture move is %s\n", PrMove(move));
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
		
    }
}

/**
 * This function is used to check the performace of ParseFen().
 *
 * This process is performed on test suites.
 * 
 * @param pos The position's pointer.
 */
void CheckFen(S_BOARD *pos){

	FILE *file;
    file = fopen("arasan20.epd","r");
    char lineIn [1024];

	s32 counter = 0;


    if(file == NULL){
        printf("File Not Found\n");
        return;
    }else{
        while(fgets (lineIn , 1024 , file) != NULL){
			
			counter++;
            if(!ParseFen(lineIn, pos)){
				printf("\n%2d\t%s\n", counter, lineIn);
				//PrintBoard(pos);
			}
            memset(&lineIn[0], 0, sizeof(lineIn));
        }
		
    }
}

static u64 myAtoi(char* str){
    // Initialize result
    u64 res = 0;
 
    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corosponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    for (u8 i = 0; str[i] != '\0'; ++i){
		if(str[i] == ' ' || str[i] == ';' || str[i] == '\n') break;
        res = (res * 10) + (str[i] - '0');
	}
 
    // return result.
    return res;
}

/**
 * This function is used to check the performace of PerftTest().
 *
 * This process is performed on test suites.
 * 
 * @param pos The position's pointer.
 */
void DebugPerftTest(S_BOARD *pos){
    FILE *file;
    file = fopen("perftsuite.epd","r");
    char lineIn [2048];
    u64 ev1 = 0, ev2 = 0;
    s32 positions = 0;
    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }else{
        while(fgets (lineIn , 1024 , file) != NULL){
			positions++;
			printf("\n\n%d  %s\n", positions,lineIn);
            ParseFen(lineIn, pos);
			char *ptr = NULL;
			char D[3] = {'D', '1', '\0'};
			for(u8 i = 0; i <= 5; i++){
				D[1] = '1' + i;
				if((ptr = strstr(lineIn, D))){
					ev1 = myAtoi(ptr + 3);
					if(ev1 < 0){
						SetColor(LIGHT_RED);
						printf("ERROR: nodes_suite: %lld\n", ev1);
						SetColor(LIGHT_GRAY);
						return;
					}
					ev2 = PerftTest(i+1, pos);
					if(ev1 != ev2){
						PrintBoard(pos);
						SetColor(LIGHT_RED);
						printf("\n nodes_suite: %lld\n nodes_perft: %lld\n", ev1, ev2);
						printf("\n perfttest Fail:\n%s\n",lineIn);
						SetColor(LIGHT_GRAY);
						getchar();
						return;
					}
				}
			}

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}

