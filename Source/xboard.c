/**
 * @file xboard.c
 * This file handles XBoard and Console modes.
 * Refrence: http://www.open-aurec.com/wbforum/WinBoard/engine-intf.html
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"string.h"

static u8 ThreeFoldRep(const S_BOARD *pos){
	s32 index;
	u8 r = 0;
	
	// Start with hisply which has fifty move = 0 becaues the board couldn't have the same position again where a pawn gwebt forward or a piece is captured .                       
	for(index = pos->hisply - pos->fiftyMove; index < pos->hisply - 1; index++){
		if(pos->posKey == pos->history[index].posKey) r++;
	}
	return r;
}

static u8 DrawMaterial(const S_BOARD *pos){
	if(pos->pceNum[wP] || pos->pceNum[bP]) return FALSE;
	if(pos->pceNum[wQ] || pos->pceNum[bQ] || pos->pceNum[wR] || pos->pceNum[bR]) return FALSE;
	if(pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1) return FALSE;
	if(pos->pceNum[wN] > 1 || pos->pceNum[bN] > 1) return FALSE;
	if(pos->pceNum[wN] && pos->pceNum[wB]) return FALSE;
	if(pos->pceNum[bN] && pos->pceNum[bB]) return FALSE;
	
	return TRUE;
}

static u8 CheckResult(S_BOARD *pos){
	if(pos->fiftyMove > 100){
		printf("1/2-1/2 {fifty move rule (claimed by %s)}\n", NAME);
		return TRUE;
	}
	
	if(ThreeFoldRep(pos) >= 2){
		printf("1/2-1/2 {3-fold repetition (claimed by %s)}\n", NAME);
		return TRUE;
	}
	
	if(DrawMaterial(pos) == TRUE){
		printf("1/2-1/2 {insufficient material (claimed by %s)}\n", NAME);
		return TRUE;
	}
	
	S_MOVELIST list[1];
	GenerateAllMoves(pos, list);
	
	u8 moveNum, found = 0;
	for(moveNum = 0; moveNum < list->count; moveNum++){
		if(!MakeMove(pos, list->moves[moveNum].move)) continue;
		found++;
		UnMakeMove(pos);
		break;
	}
	
	if(found != 0) return FALSE;
	
	u8 InCheck = IsSqAttacked(pos->kingSq[pos->side], pos->side ^ 1, pos);
	if(InCheck == TRUE){
		if(pos->side == WHITE){
			printf("0-1 {black mates (claimed by %s)}\n", NAME);
			return TRUE;
		}else{
			printf("0-1 {white mates (claimed by %s)}\n", NAME);
			return TRUE;
		}
	}else{
		printf("\n1/2-1/2 {stalemate (claimed by %s)}\n", NAME);
			return TRUE;
	}
}

static void PrintOption(){
	printf("feature ping=1 setboard=1 colors=0 usermove=1\n");      
	printf("feature done=1\n");
}

/**
 * This function handles some commands of XBoard protocol.
 * 
 * @param pos The posintion's pointer.
 * @param info The pointer of engine inforamtion structure.
 */
void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info){
	
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	PrintOption();
	
	s8 depth = -1;
	s32 movestogo[2] = {30, 30}, movetime = -1;
	s32 time = -1, inc = 0;
	u8 sec;
	u8 engineSide = BLACK;           
	s32 timeleft;
	s32 mps;
	s32 move = NOMOVE;
	s32 i, score;
	char inBuf[80], command[80];
	
	ParseFen(START_FEN,pos);
	
	while(TRUE){ 

		fflush(stdout);

		if(pos->side == engineSide && !CheckResult(pos)){  
			
			info->timeset = FALSE;
			info->starttime = GetTimeMs();
			if(time != -1){
				time /= movestogo[pos->side];
				time -= 50;
				info->stoptime = info->starttime + time + inc;
				info->timeset = TRUE;
			}
			
			if(depth == -1 || depth > MAXDEPTH)info->depth = MAXDEPTH;
			else info->depth = depth;
			
			printf("time:%d start:%d stop:%d depth:%d timeset:%d movestogo:%d mps:%d\n",
				time, info->starttime, info->stoptime, info->depth, info->timeset, movestogo[pos->side],mps);
			
			SearchPosition(pos, info);
			
			if(mps != 0){
				
				movestogo[pos->side ^ 1]--;
				if(movestogo[pos->side^1] < 1) movestogo[pos->side ^ 1] = mps;
				
			}
			
			
		}
	
		memset(inBuf, 0, sizeof(inBuf));
		fflush(stdout);
		if(!fgets(inBuf, 80, stdin))continue;
    
		sscanf(inBuf, "%s", command);
		printf("command seen:%s\n", inBuf);
    
		if(!strcmp(command, "quit")){
			info->quit = TRUE;
			break; 
		}
		
		if(!strcmp(command, "force")){ 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(!strcmp(command, "protover")){
			PrintOption();
			continue;
		}
		
		if(!strcmp(command, "sd")){
			sscanf(inBuf, "sd %d", &depth);
			printf("DEBUG depth:%d\n", depth);
			continue; 
		}
		
		if(!strcmp(command, "st")) {
			sscanf(inBuf, "st %d", &movetime);
			printf("DEBUG movetime:%d\n", movetime);
			continue; 
		}
		
		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &time);
			time *= 10;
			printf("DEBUG time:%d\n", time);
			continue; 
		}
		
		if(!strcmp(command, "level")){
			
			sec = 0;
			movetime = -1;
			time = -1;
			if(sscanf(inBuf, "level %d %d %d", &mps, &timeleft, &inc) != 3){
				sscanf(inBuf, "level %d %d:%d %d", &mps, &timeleft, &sec, &inc);
				printf("DEBUG level with :\n");
			}else{
				printf("DEBUG level without :\n");
			}
			
			timeleft *= 60000;
			timeleft += sec * 1000;
			
			movestogo[0] = movestogo[1] = 30;
			if(mps != 0) movestogo[0] = movestogo[1] = mps;
			
			printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n", timeleft, movestogo[0], inc, mps);
			continue; 
			
		}
		
		if(!strcmp(command, "ping")) { 
			printf("pong%s\n", inBuf + 4); 
			continue; 
		}
		
		if(!strcmp(command, "new")) { 
			engineSide = BLACK; 
			ParseFen(START_FEN, pos);
			depth = -1; 
			time = -1;
			continue; 
		}
		
		if(!strcmp(command, "setboard")){
			engineSide = BOTH;  
			ParseFen(inBuf + 9, pos); 
			continue; 
		}   		
		
		if(!strcmp(command, "go")) { 
			engineSide = pos->side;  
			continue; 
		}		
		  
		if(!strcmp(command, "usermove")){
			movestogo[pos->side]--;
			move = ParseMove(inBuf + 9, pos);	
			if(move == NOMOVE) continue;
			MakeMove(pos, move);
            pos->ply = 0;
		}    
    }	
}

/**
 * This function handles some commands of Console mode.
 * 
 * @param pos The posintion's pointer.
 * @param info The pointer of engine inforamtion structure.
 */
void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info){
	
	printf("Welcome to %s In Console Mode.\n", NAME);
	printf("Type \"help\" for commands.\n\n");
	
	info->GAME_MODE = CONSOLE_MODE;
	info->POST_THINKING = TRUE;
	
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	s32 depth = MAXDEPTH;
	s32 movetime = 6;
	u8 engineSide = BLACK;           

	s32 move = NOMOVE;
	char inBuf[80], command[80];
	
	ParseFen(START_FEN, pos);
	
	while(TRUE){
		
		fflush(stdout);

		if(pos->side == engineSide && !CheckResult(pos)){  
			
			info->starttime = GetTimeMs();
			info->timeset = FALSE;
			
			if(movetime != 0){
	
				info->stoptime = info->starttime + movetime * 1000;
				info->timeset = TRUE;
			}
			
			info->depth = depth;
			
			if(info->POST_THINKING) printf("-> Thinking\n");
			else printf("-> Thinking ");
			SearchPosition(pos, info);	
			
		}
		
		if(info->interrupt == FALSE) printf("\n%s > ", NAME);
		
		fflush(stdout);
		memset(inBuf, 0, sizeof(inBuf));
		
		if(info->interrupt) {
			printf("interrupt string: %s\n", interrupt_string);
			strncpy(inBuf, interrupt_string, 80);
			info->interrupt = FALSE;
			printf("inBuf string: %s\n", inBuf);
		}else{
			if(!fgets(inBuf, 80, stdin)) continue;
		}
		
		if(inBuf[0] == 0) continue;
		sscanf(inBuf,"%s", command);
		
		fflush(stdin);
		
		if(!strcmp(command, "help")){
			printf("\nCommands:\n");
			printf(" quit       -> Quit game.\n");
			printf(" force      -> Computer will not think.\n");
			printf(" board      -> Show board.\n");
			printf(" clear      -> Clear screen.\n");
			printf(" post       -> Show thinking (default).\n");
			printf(" nopost     -> Do not show thinking.\n");
			printf(" new        -> Start new game.\n");
			printf(" go         -> Set computer thinking to the side to move.\n");
			printf(" undo       -> To take the last excuted move.\n");
			printf(" depth x    -> Set depth to 'x' ('x' range 1:64), maximum depth is default.\n");
			printf(" time x     -> Set thinking time to 'x' seconds (depth still applies if set).\n");
			printf(" default    -> To reset time and depth to default value (time = 6s, depth = Max).\n");
			printf(" view       -> Show current depth and movetime settings.\n");
			printf(" ponder_on  -> Turn ON ponder.\n");
			printf(" ponder_off -> Turn OFF ponder.\n\n");
			printf(">> Notes <<\n");
			printf("- You can make moves in the form \"e2e4\", \"e1g1\", \"e7e8q\" (promoring to queen), etc.\n");
			printf("- After making \"undo\" command the computer will not think, so you should make \"go\" command to return thinking.\n");
			printf("- Don't make any mistake during entering your move.\n");
			SetColor(LIGHT_GRAY);
			continue;
		}
		
		if(!strcmp(command, "mirrortest")){
			MirrorEvalTest(pos);
			engineSide = BOTH;
			continue;
		}
		
		if(!strcmp(command, "checkfen")){
			CheckFen(pos);
			engineSide = BOTH;
			continue;
		}
		
		if(!strcmp(command, "mirror")){
			PrintBoard(pos);
			printf("Eval: %d\n", EvalPosition(pos));
			MirrorBoard(pos);
			PrintBoard(pos);
			printf("Eval: %d\n", EvalPosition(pos));
			MirrorBoard(pos);
			continue;
		}
		
		if(!strcmp(command, "quit")){
			info->quit = TRUE;
			break;
		}
		
		if(!strcmp(command, "ponder_on")){
			info->ponder = TRUE;
			continue;
		}
		
		if(!strcmp(command, "ponder_off")){
			info->ponder = FALSE;
			continue;
		}
		
		if(!strcmp(command, "force")){
			engineSide = BOTH;
			continue;
		}
		
		if(!strcmp(command, "board")){
			PrintBoard(pos);
			continue;
		}
		
		if(!strcmp(command, "post")){
			info->POST_THINKING = TRUE;
			continue;
		}
		
		if(!strcmp(command, "nopost")){
			info->POST_THINKING = FALSE;
			continue;
		}
		
		if(!strcmp(command, "new")){
			ClearHashTable(pos->HashTable);
			engineSide = BLACK;
			ParseFen(START_FEN,pos);
			printf("\n---------- New Game ----------\n");
			PrintBoard(pos);
			continue;
		}
		
		if(!strcmp(command, "clear")){
		#ifdef WIN32
			system("cls");
		#else
			system("clear");
		#endif
			continue;
		}
		
		if(!strcmp(command, "go")){
			engineSide = pos->side;
			continue;
		}
		
		if(!strcmp(command, "undo")){
			pos->ply++;
			if(pos->hisply == 0){
				SetColor(LIGHT_RED);
				printf("-> ERROR! \n");
				SetColor(LIGHT_GRAY);
				continue;
			} 
			UnMakeMove(pos);
			PrintBoard(pos);
			engineSide = BOTH;
			continue;
		}
		
		if(!strcmp(command, "perft")){
			engineSide = BOTH;
			DebugPerftTest(pos);
			continue;
		}
		
		if(!strcmp(command, "fen")){
			engineSide = BOTH;  
			ParseFen(inBuf + 4, pos); 
			PrintBoard(pos);
			continue; 
		}
		
		if(!strcmp(command, "debug")){
			engineSide = BOTH;
			DebugAnalysisTest(pos, info);
			continue;
		}
		
		if(!strcmp(command, "qui")){
			engineSide = BOTH;
			SEETest(pos);
			continue;
		}
		
		if(!strcmp(command, "SEE")){
			engineSide = BOTH;
			DebugSEETest(pos);
			continue;
		}
	
		if(!strcmp(command, "depth")){
			sscanf(inBuf,"depth %d", &depth);
			if(!(depth > 0 &&  depth <= MAXDEPTH)){
				printf("-> Out Of range! '%d'\n",depth);
				depth = MAXDEPTH;
			}else{
				printf("-> depth is set to '%d'\n",depth);
			}
			continue;
		}
		
		if(!strcmp(command, "time")){
			sscanf(inBuf,"time %d", &movetime);
			printf("-> time is set to '%d'\n", movetime);
			continue;
		}
		
		if(!strcmp(command, "default")){
			depth = 64;
			movetime = 3;
			printf("-> depth and time set to default\n");
			continue;
		}
		
		if(!strcmp(command, "view")){
			if(depth == MAXDEPTH) printf("depth default set ");
			else printf("depth %d",depth);
			
			if(movetime != 0) printf(" movetime %ds\n",movetime);
			else printf(" movetime not set\n");
			continue;
		}
		
		move = ParseMove(inBuf,pos);
		if(move == NOMOVE){
			SetColor(LIGHT_RED);
			printf("-> Unknown Command! \"%s\"\n", command);
			SetColor(LIGHT_GRAY);
			continue;
		}
		MakeMove(pos, move);
		PrintBoard(pos);
		pos->ply = 0;
	}
	
	
}










