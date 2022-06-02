/**
 * @file MCHE.c
 * This is the main file of the chess engine.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"string.h"

/**
 * Main entry point of the program.
 */
s32 main(){	
	
	setbuf(stdin, NULL);
    setbuf(stdout, NULL);
	
	SetColor(LIGHT_GREEN);
	printf("************************************************************************\n");
	printf("* Momentum's Chess Engine (MCHE), version 1.0, April 2020.\n");
	printf("* By: Software Committee, Mommentum Team.\n*\n");
	printf("* This program is supposed to be a prototype to the chess engine that\n");
	printf("* will be used in the next version of \"Smart Chess Board\" project.\n");
	printf("************************************************************************\n\n");
	SetColor(LIGHT_GRAY);
	
	AllInit();
	
	S_BOARD pos[1];
	S_SEARCHINFO info[1];
	info->quit = FALSE;
	info->ponder = FALSE;
	info->interrupt = FALSE;
	info->stopped = FALSE;
	InitHashTable(pos->HashTable, HASH_TABLE_SIZE_MB);
	InitEvalTable(pos->EvalTable, PAWN_TABLE_SIZE_MB, EVAL_TABLE_SIZE_MB);
	
	printf("Welcome to %s! Type \"MCHE\" for console mode.....\n", NAME);
	char line[20];
	
	while(TRUE){
		
		fflush(stdout);
		
		memset(line, 0, sizeof(line));

		if(!fgets(line, sizeof(line), stdin)) continue;
		if(line[0] == '\n') continue;
		
		if (!strncmp(line, "uci", 3)){
		#ifdef WIN32
			system("cls");
		#else
			system("clear");
		#endif
			UCI_Loop(pos, info);
			break;
		}else if(!strncmp(line, "xboard", 6)){
		#ifdef WIN32
			system("cls");
		#else
			system("clear");
		#endif
			XBoard_Loop(pos, info);
			break;
		}else if (!strncmp(line, "MCHE", 4)){
		#ifdef WIN32
			system("cls");
		#else
			system("clear");
		#endif
			Console_Loop(pos, info);
			break;
		}else if(!strncmp(line, "quit", 4)){
			break;
		}else{
			SetColor(LIGHT_RED);
			printf("UnKnown! %s\n", line);
			SetColor(LIGHT_GRAY);
		}
	} 
	
	CleanMemory(pos->HashTable, pos->EvalTable);
	return 0;
}














