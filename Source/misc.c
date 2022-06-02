/**
 * @file misc.c
 * This file includes all used miscellaneous methods.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"unistd.h"
#include"string.h"

/* 
 * If running the program in windows then WIN32 is defined and we get the time buy th library windows.h
 * else if running on lunix then we include sys/time.h to get the time.
 */
#ifdef WIN32
	#include"windows.h"
#else
	#include"sys/time.h"
	#include"sys/select.h"
#endif

char interrupt_string[INPUTBUFFER];

/**
 * The finction allows the user to set the font color on console
 * depends on a given flag color (ex. _BLACK, _WHITE, LIGHT_GRAY, ...), 
 * these flags are defined in def.h.
 * 
 * @param ForgC THe color flag (0: 15)
 */
void SetColor(s32 ForgC){
	#ifdef WIN32
		WORD wColor;

		HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		// We use csbi for the wAttributes word.
		if(GetConsoleScreenBufferInfo(hStdOut, &csbi)){
			// Mask out all but the background attribute, and add in the forgournd color
			wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
			SetConsoleTextAttribute(hStdOut, wColor);
		}
	#else
		switch(ForgC){
			case LIGHT_RED: printf("\E[31m"); break; 	// red
			case LIGHT_GREEN: printf("\E[32m"); break; 	// greeen
			case LIGHT_GRAY: printf("\E[0m"); break; 	// gray
			default: printf("\E[0m");
		}
	#endif
}

/**
 * This fuction returns the current time in milliseconds.
 *
 * @return millis
 */
s32 GetTimeMs(void){
	#ifdef WIN32
		return GetTickCount();
	#else
		struct timeval t;
		gettimeofday(&t, NULL);
		return t.tv_sec*1000 + t.tv_usec/1000;
	#endif

}

static s32 InputWaiting(){
	
	#ifndef WIN32
		fd_set readfds;
		struct timeval tv;
		FD_ZERO (&readfds);
		FD_SET (fileno(stdin), &readfds);
		tv.tv_sec=0; tv.tv_usec=0;
		select(16, &readfds, 0, 0, &tv);

		return (FD_ISSET(fileno(stdin), &readfds));
	#else
		static s32 init = 0, pipe;
		static HANDLE inh;
		DWORD dw;

		if(!init){
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);
			if(!pipe){
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}
		if(pipe){
			if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
			return dw;
		}else{
			GetNumberOfConsoleInputEvents(inh, &dw);
			return dw <= 1 ? 0 : dw;
		}
	#endif

}

/**
 * This fuction is to check the peeking in the stdin...to whether there is an input waiting or it
 * if there is we must read it out.
 *
 * This code is taken from chess programming wiki.
 * 
 * @param info The pointer of engine inforamtion structure.
 */
void ReadInput(S_SEARCHINFO *info){
	s32 bytes = 0;
	char *endc = NULL;
	
    if(InputWaiting() && info->interrupt == FALSE){
		info->stopped = TRUE;
		info->interrupt = TRUE;
		
		memset(interrupt_string, 0, INPUTBUFFER);
		
		do
		{
			bytes = read(fileno(stdin), interrupt_string, INPUTBUFFER);
		}
		while(bytes < 0);
		
		endc = strchr(interrupt_string,'\n');
		if(endc) *endc = 0;

		if(strlen(interrupt_string) > 0){
			if(!strncmp(interrupt_string, "quit", 4)) info->quit = TRUE;
		}		
    }
}













