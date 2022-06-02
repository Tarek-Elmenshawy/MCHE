/**
 * @file uci.c
 * This file handles UCI mode.
 * Refrence: http://wbec-ridderkerk.nl/html/UCIProtocol.html
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */
#include"defs.h"
#include"string.h"


/**
 * This function receives a locatoin on th board in algebric form (ex. e2, a1, etc.)
 * and generates all the possible moves of the piece on it then print moves in custom form.
 * 
 * @param line A pinter to array of character.
 * @param pos The position's pointer.
 */
static void PrintMovesForLocation(char *line, S_BOARD *pos){
	
	u8 file, rank;
	char *ptr;
	if((ptr = strstr(line, "location"))){
		file = *(ptr + 9) - 'a';
		rank = *(ptr + 10) - '1';
	}
	
	S_MOVELIST list[1];
	GenerateMovesForLocation(pos, list, FR2SQ(file, rank));
	ASSERT(MoveListOk(list, pos));
	printf("moves");
	for(u8 moveNum = 0; moveNum < list->count; moveNum++){
		if(MakeMove(pos, list->moves[moveNum].move)){
			UnMakeMove(pos);
			printf(" %s", SpePrMove(list->moves[moveNum].move));
		}
	}
	printf("\n");
}

/**
 * This function parses the given 'go' command and store informatin then start seraching.
 * 
 * NOTE: go cammand -> go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
 * - wtime 
 * 	   white has x msec left on the clock
 * - btime 
 *     black has x msec left on the clock
 * - winc 
 * 	   white increment per move in mseconds if x > 0
 * - binc 
 *     black increment per move in mseconds if x > 0
 * - movestogo 
 *     there are x moves to the next time control,
 *	   this will only be sent if x > 0,
 *	   if you don't get this and get the wtime and btime it's sudden death
 * - depth 
 *	   search x plies only.
 * - movetime 
 *	   search exactly x mseconds
 * - infinite
 *	   search until the "stop" command. Do not exit the search without being told so in this mode!
 * 
 * @param line A pinter to array of character.
 * @param info The pointer of engine inforamtion structure.
 * @param pos The position's pointer.
 */
static void ParseGo(char *line, S_SEARCHINFO *info, S_BOARD *pos){
	
	s8 depth = -1;
	s32 movestogo = 30, movetime = -1;
	s32 time = -1, inc = 0;
	
	char *ptr = NULL;
	
	if((ptr = strstr(line, "infinite"))) ;
	if((ptr = strstr(line, "depth"))) depth = atoi(ptr + 6);
	
	if((ptr = strstr(line, "wtime")) && pos->side == WHITE) time = atoi(ptr + 6);
	if((ptr = strstr(line, "btime")) && pos->side == BLACK) time = atoi(ptr + 6);
	
	if((ptr = strstr(line, "winc")) && pos->side == WHITE) inc = atoi(ptr + 5);
	if((ptr = strstr(line, "binc")) && pos->side == BLACK) inc = atoi(ptr + 5);
	
	if((ptr = strstr(line, "movetime"))) movetime = atoi(ptr + 9);
	if((ptr = strstr(line, "movestogo"))) movestogo = atoi(ptr + 10);
	
	if(movetime != -1){
		time = movetime;
		movestogo = 1;
	}
	
	info->timeset = FALSE;
	info->starttime = GetTimeMs();
	if(time != -1){
		time /= movestogo;
		time -= 50;
		info->stoptime = info->starttime + time + inc;
		info->timeset = TRUE;
	}
	
	if(depth == -1)info->depth = MAXDEPTH;
	else info->depth = depth;
	
	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time, info->starttime, info->stoptime, info->depth, info->timeset);
		
	SearchPosition(pos, info);
	
}

/**
 * This fiction parses the given postion notation then performs it on the board.
 *
 * NOTE: The function can deel with
 * 	-  position fen "fen string"
 *  -  position startpos
 *  -  position moves e2e4 e7e5 b7b8q
 * 
 * @param line A pinter to array of character.
 * @param pos The position's pointer.
 */
static void ParsePosition(char *line, S_BOARD *pos){
	line += 9;
	
	char *ptrChar = line;
	
	if(!strncmp(line, "startpos", 8)){
		ParseFen(START_FEN, pos);
	}else{
		ptrChar = strstr(line, "fen");
		
		if(ptrChar == NULL){
			ParseFen(START_FEN, pos);
		}else{
			ptrChar += 4;
			ParseFen(ptrChar, pos);
		}
	}
	
	ptrChar = strstr(line, "moves");
	s32 move;
	
	if(ptrChar != NULL){
		ptrChar += 6;
		while(*ptrChar){
			
			move = ParseMove(ptrChar, pos);
			if(move == NOMOVE) break;
			
			MakeMove(pos, move);
			pos->ply = 0;
			
			while(*ptrChar && *ptrChar != ' ') ptrChar++;
			ptrChar++;
		}
	}
		
}

/**
 * This function handles some commands of UCI protocol in addition to some custom commands.
 * 
 * @param pos The posintion's pointer.
 * @param info The pointer of engine inforamtion structure.
 */
void UCI_Loop(S_BOARD *pos, S_SEARCHINFO *info){
	
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	
	printf("id name %s\n", NAME);
	printf("id author T.Elmenshawy\n");
	printf("uciok\n");
	
	char line[INPUTBUFFER];
	
	while(TRUE){
		
		memset(line, 0, INPUTBUFFER);
		
		fflush(stdout);
		
		if(info->interrupt){
			strncpy(line, interrupt_string, INPUTBUFFER);			
			info->interrupt = FALSE;
		}else{
			if(!fgets(line, INPUTBUFFER, stdin)) continue;
		}
		
		if(line[0] == '\n' || line[0] == 0) continue;
		
		if(!strncmp(line, "isready", 7)){
			printf("readyok\n");
			continue;
		}else if(!strncmp(line, "quit", 4)){
			info->quit = TRUE;
			break;
		}else if(!strncmp(line, "ponder_on", 9)){
			info->ponder = TRUE;
			printf("ponder is on\n");
			continue;
		}else if(!strncmp(line, "ponder_off", 10)){
			info->ponder = FALSE;
			printf("ponder is off\n");
			continue;
		}else if(!strncmp(line, "position", 8)){
			ParsePosition(line, pos);
		}else if(!strncmp(line, "ucinewgame", 10)){
			ParsePosition("postion startpos\n", pos);
		}else if(!strncmp(line, "go", 2)){
			ParseGo(line,info, pos);
		}else if(!strncmp(line, "uci", 3)){
			printf("id name %s\n", NAME);
			printf("id author T.Elmenshawy\n");
			printf("uciok\n");
		}else if(!strncmp(line, "generate", 8)){
			PrintMovesForLocation(line, pos);
		}
		
		if(info->quit == TRUE) break;
		
	}
	
}

