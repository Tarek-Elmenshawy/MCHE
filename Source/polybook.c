/**
 * @file polybook.c
 * This file handles opening books depends on polyglot book format.
 * Refrence: http://hgm.nubati.net/book_format.html
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"
#include"polykeys.h"

static const s8 PolyKindOfPiece[13] = {-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10};

/// This is a struct that represents one of opening book entries to load the opening book in ram.
typedef struct{
	u64 key;		///< "key" It is a 64 bit hash of a board position. key=piece^castle^enpassant^turn;
	u16 move;		///< "move" It is a bit field that represents the best move of the entry position
	u16 weight;		///< "weight" It is a measure for the quality of the move.
	u32 learn;		///< "learn" This field is used to record learning information
}S_POLY_BOOK_ENTRY;

static u32 NumEntries;
static S_POLY_BOOK_ENTRY *entries;

/**
 * Read opening book.bin and load in RAM.
 * 
 */
void InitPolyBook(){
	EngineOptions->UseBook = FALSE;
#ifdef WIN32
	FILE *pFile = fopen("opening_book.bin", "rb");
#else
	FILE *pFile = fopen("/home/pi/Desktop/Project_3/opening_book.bin", "rb");
#endif
	
	SetColor(LIGHT_RED);
	if(pFile == NULL){
		printf("Book File Not Read.\n");
	}else{
		fseek(pFile, 0, SEEK_END);
		long positions = ftell(pFile);
		
		if(positions < sizeof(S_POLY_BOOK_ENTRY)){
			printf("No Entries Found In Book File.\n");
		}else{
			NumEntries = positions / sizeof(S_POLY_BOOK_ENTRY);
			printf("INFO: %ld Entries Found In Book File.\n", NumEntries);
			
			entries = (S_POLY_BOOK_ENTRY*)malloc(NumEntries * sizeof(S_POLY_BOOK_ENTRY));
			rewind(pFile);
			
			size_t returnVal = fread(entries,sizeof(S_POLY_BOOK_ENTRY), NumEntries, pFile);
			printf("INFO: fread() %ld Entries Read In From Book File.\n", returnVal);
			
			if(NumEntries > 0) EngineOptions->UseBook = TRUE;
		}
	}
	SetColor(LIGHT_GRAY);
}

/**
 * Free the allocated memory for the opening book in RAM.
 * 
 */
void CleanPolyBook(){
	free(entries);
}

static u8 HasPawnForCapture(const S_BOARD *board){
	u8 sqWithPawn = 0;
	u8 targetPce = (board->side == WHITE)? wP : bP;
	
	if(board->enPass != NO_SQ){
		
		if(board->side == WHITE) sqWithPawn = board->enPass - 10;
		else sqWithPawn = board->enPass + 10;
		
		if(board->pieces[sqWithPawn - 1] == targetPce) return TRUE;
		else if(board->pieces[sqWithPawn + 1] == targetPce) return TRUE;
	}
	
	return FALSE;
}

static u64 PolyKeyFromBoard(const S_BOARD *board){
	
	u8 sq = 0, rank = 0, file = 0;
	u8 piece = EMPTY;
	s8 polyPiece = -1;
	s16 offset = 0;
	u64 finalKey = 0ULL;
	
	//pieces
	for(sq = 0; sq < 64; sq++){
		piece = board->pieces[SQ120(sq)];
		if(piece != EMPTY){
			ASSERT(piece >= wP && piece <= bK);
			polyPiece = PolyKindOfPiece[piece];
			rank = RanksBrd[sq];
			file = FilesBrd[sq];
			finalKey ^= Random64Ploy[(64 * polyPiece) + (8 * rank) + file];
		}
	}
	
	//castling
	offset = 768;
	if(board->castlePerm & WKCA) finalKey ^= Random64Ploy[offset + 0];
	if(board->castlePerm & WQCA) finalKey ^= Random64Ploy[offset + 1];
	if(board->castlePerm & BKCA) finalKey ^= Random64Ploy[offset + 2];
	if(board->castlePerm & BQCA) finalKey ^= Random64Ploy[offset + 3];
	
	//enpassent 
	offset = 772;
	if(HasPawnForCapture(board) == TRUE){
		file = FilesBrd[SQ64(board->enPass)];
		finalKey ^= Random64Ploy[offset + file];
	}
	
	//side
	if(board->side == WHITE) finalKey ^= Random64Ploy[780];
	
	return finalKey;
}

static u16 endian_swap_u16(u16 x) {
    x = (x>>8) | 
        (x<<8); 
    return x;
} 

static u32 endian_swap_u32(u32 x) { 
    x = (x>>24) | 
        ((x<<8) & 0x00FF0000) | 
        ((x>>8) & 0x0000FF00) | 
        (x<<24); 
    return x;
} 

static u64 endian_swap_u64(u64 x) {
    x = (x>>56) | 
        ((x<<40) & 0x00FF000000000000) | 
        ((x<<24) & 0x0000FF0000000000) | 
        ((x<<8)  & 0x000000FF00000000) | 
        ((x>>8)  & 0x00000000FF000000) | 
        ((x>>24) & 0x0000000000FF0000) | 
        ((x>>40) & 0x000000000000FF00) | 
        (x<<56); 
    return x;
}

static s32 ConvertPolyMoveToInternalMove(u16 polyMobe, S_BOARD *board){
	u8 ff = (polyMobe >> 6) & 7;
	u8 fr = (polyMobe >> 9) & 7;
	u8 tf = (polyMobe >> 0) & 7;
	u8 tr = (polyMobe >> 3) & 7;
	u8 pp = (polyMobe >> 12) & 7;
	
	char moveString[6];
	if(pp == 0){
		sprintf(moveString, "%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr]);
	}else{
		char promChar = 'q';
		switch(pp){
			case 1: promChar = 'n'; break;
			case 2: promChar = 'b'; break;
			case 3: promChar = 'r'; break;
		}
		sprintf(moveString, "%c%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr],
			promChar);
	}
	
	return ParseMove(moveString, board);
}

/**
 * Search for the best move for the current board position in the opening book
 * then return the best move if it is found.
 * 
 * @param pos The position's pointer.
 * @return move The best move for the curent position.
 */
s32 GetBookMove(S_BOARD *pos){
	u64 polyKey = PolyKeyFromBoard(pos);
	u8 index = 0;
	S_POLY_BOOK_ENTRY *entry;
	u16 polyMobe;
	const u8 MAXBOOKMOVES = 32;
	s32 bookMoves[MAXBOOKMOVES];
	s32 move = NOMOVE;
	u8 count = 0;
	
	for(entry = entries; entry < entries + NumEntries; entry++){
		if(polyKey == endian_swap_u64(entry->key)){
			polyMobe = endian_swap_u16(entry->move);
			move = ConvertPolyMoveToInternalMove(polyMobe, pos);
			if(move != NOMOVE){
				bookMoves[count++] = move;
				if(count >= MAXBOOKMOVES) break;
			}
		}
	}
	
	printf("Listing Book Moves:\n");
	for(index = 0; index < count; index++){
		printf("BookMove:%d :%s\n", index + 1, PrMove(bookMoves[index]));
	}
	
	if(count == 0) return NOMOVE;
	else return bookMoves[rand() % count];
	
}



