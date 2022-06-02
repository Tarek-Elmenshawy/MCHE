/**
 * @file board.c
 * This file is responsible for all processes that perform on board.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

/**
 * Check all contents of board and it is uesd in debugging in most of engine's functios.
 * 
 * @param pos The position's pointer to check.
 * @return TRUE if evrything is ok, FALSE if not. 
 */
u8 CheckBoard(const S_BOARD *pos){
	u8 t_pceNum[13] = {0};
	u8 t_bigPce[2] = {0};
	u8 t_majPce[2] = {0};
	u8 t_minPce[2] = {0};
	u16 t_material[2] = {0, 0};
	
	s8 sq ,t_piece, t_pce_num, pcount, colour;
	
	u64 t_pawns[3];
	t_pawns[WHITE] = pos->pawns[WHITE];
	t_pawns[BLACK] = pos->pawns[BLACK];
	t_pawns[BOTH] = pos->pawns[BOTH];
	
	// Check piece lists
	for(t_piece = wP; t_piece <= bK; t_piece++){
		for(t_pce_num = 0; t_pce_num < pos->pceNum[t_piece]; t_pce_num++){
			sq = pos->pList[t_piece][t_pce_num];
			ASSERT(pos->pieces[sq] == t_piece);
		}
	}
	
	// Check piece count and other counters
	for(sq = 0; sq < 64; sq++){
		t_piece = pos->pieces[SQ120(sq)];
		if(t_piece != EMPTY){
			t_pceNum[t_piece]++;
			colour = PieceCol[t_piece];
		
			if(PieceBig[t_piece] == TRUE) t_bigPce[colour]++;
			if(PieceMaj[t_piece] == TRUE) t_majPce[colour]++;
			if(PieceMin[t_piece] == TRUE) t_minPce[colour]++;
			
			t_material[colour] += PieceVal[t_piece];
		}
	}
	
	for(t_piece = wP; t_piece <= bK; t_piece++) ASSERT(pos->pceNum[t_piece] == t_pceNum[t_piece]);
	
	// Check bitboards count
	pcount = CNT(t_pawns[WHITE]);
	ASSERT(pcount == pos->pceNum[wP]);
	pcount = CNT(t_pawns[BLACK]);
	ASSERT(pcount == pos->pceNum[bP]);
	pcount = CNT(t_pawns[BOTH]);
	ASSERT(pcount == (pos->pceNum[wP] + pos->pceNum[bP]));
	
	// Check bit boards squares
	while(t_pawns[WHITE]){
		sq = POP(&t_pawns[WHITE]);
		ASSERT(pos->pieces[SQ120(sq)] == wP);
	}
	while(t_pawns[BLACK]){
		sq = POP(&t_pawns[BLACK]);
		ASSERT(pos->pieces[SQ120(sq)] == bP);
	}
	while(t_pawns[BOTH]){
		sq = POP(&t_pawns[BOTH]);
		ASSERT(pos->pieces[SQ120(sq)] == wP || pos->pieces[SQ120(sq)] == bP);
	}
	
	// These lines ar only for debugging
	ASSERT(t_material[WHITE] == pos->material[WHITE] && t_material[BLACK] == pos->material[BLACK]);
	ASSERT(t_bigPce[WHITE] == pos->bigPce[WHITE] && t_bigPce[BLACK] == pos->bigPce[BLACK]);
	ASSERT(t_minPce[WHITE] == pos->minPce[WHITE] && t_minPce[BLACK] == pos->minPce[BLACK]);
	ASSERT(t_majPce[WHITE] == pos->majPce[WHITE] && t_majPce[BLACK] == pos->majPce[BLACK]);
	
	ASSERT(pos->side == WHITE || pos->side == BLACK);
	ASSERT(GeneratePosKey(pos) == pos->posKey);
	ASSERT(GeneratePawnKey(pos) == pos->pawnKey);
	
	ASSERT(pos->enPass == NO_SQ || (pos->side == WHITE && RanksBrd[SQ64(pos->enPass)] == RANK_6) || (pos->side == BLACK && RanksBrd[SQ64(pos->enPass)] == RANK_3));
	
	ASSERT(pos->pieces[pos->kingSq[WHITE]] == wK);
	ASSERT(pos->pieces[pos->kingSq[BLACK]] == bK);
	
	return TRUE;
	
}

/**
 * Update pieces list.
 * 
 * @param pos The position's pointer to update.
 */
void UpdateListsMaterial(S_BOARD *pos){
	u8 sq64, piece, colour;
	
	for(sq64 = 0; sq64 < 64; sq64++){
		piece = pos->pieces[SQ120(sq64)];
		if(piece != EMPTY){
			colour = PieceCol[piece];
			
			if(PieceBig[piece] == TRUE) pos->bigPce[colour]++;
			if(PieceMaj[piece] == TRUE) pos->majPce[colour]++;
			if(PieceMin[piece] == TRUE) pos->minPce[colour]++;
			
			pos->material[colour] += PieceVal[piece];
			
			// pieces list
			pos->pList[piece][pos->pceNum[piece]] = SQ120(sq64);
			pos->pceNum[piece]++;
			
			if(piece == wK) pos->kingSq[WHITE] = SQ120(sq64);
			if(piece == bK) pos->kingSq[BLACK] = SQ120(sq64);
			if(piece == wP){
				SETBIT(pos->pawns[WHITE], sq64);
				SETBIT(pos->pawns[BOTH], sq64);
			}else if(piece == bP){
				SETBIT(pos->pawns[BLACK], sq64);
				SETBIT(pos->pawns[BOTH], sq64);
			}
		}
	}
}

/**
 * Parsing the incomming FEN notation and impelement it on our BOARD.
 * 
 * @param fen The fen's pointer to parse.
 * @param pos The position's pointer.
 * @return TRUE if evrything is ok, FALSE if not. 
 */
u8 ParseFen(const char *fen, S_BOARD *pos){
	
	// Debugging check: If the pointers of the FEN string and S_bOARD point to data not nothing
	ASSERT(fen != NULL);
	ASSERT(pos != NULL);
	
	// Local variables used in this fuction
	s8 rank = RANK_8;                                       // We start with RANK_8 because the FEN notation starts with black pieces
	s8 file = FILE_A;
	u8 piece = EMPTY;
	u8 count = 1;
	u8 i = 0;
	u8 sq120 = 0;
	u8 sq64 = 0;
	
	ResetBoard(pos);                                         // Empty all contents of BOARD to udate it after
	
	// Pieces
	while(rank >= RANK_1 && *fen){
		count = 1;
		switch(*fen){
			
			// Assign the type of pieces according to the corresponding characters in FEN notation
			case 'p': piece = bP;	break;
			case 'n': piece = bN;	break;
			case 'b': piece = bB;	break;
			case 'r': piece = bR;	break;
			case 'q': piece = bQ;	break;
			case 'k': piece = bK;	break;
			case 'P': piece = wP;	break;
			case 'N': piece = wN;	break;
			case 'B': piece = wB;	break;
			case 'R': piece = wR;	break;
			case 'Q': piece = wQ;	break;
			case 'K': piece = wK;	break;
			
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				piece = EMPTY;
				count = *fen - '0';                           // Load the number of empty squares
				break;
				
			case ' ':
			case '/':
				rank--;                                       // To go to new rank by decrementing rank as we start from RANK_8 
				file = FILE_A;                                // To start from the first file                     
				fen++;                                        // Go to the next character
				continue;                                     // Skip this cycle because this is a turning point to the next rank
				
			default:
				printf("FEN ERROR!: %c", *fen);               // Print an error message if we recieve an unpredicted charater
				return FALSE;                                    
		}
		
		// Here, we update each piece's position
		for(i =0; i < count; i++){
			sq64 = rank * 8 + file;                           // Get an index ion 64 board
			sq120 = SQ120(sq64);                              // Convert the index from 64 to 120
			if(piece != EMPTY) pos->pieces[sq120] = piece;    // Update the board's square with the new piece
			file++;                                           // Go to the next file
		}
		fen++;                                                // Increament the pointer to capture the next character
	}
	
	// Debugging check: If "side is to move" is not out of range
	ASSERT(*fen == 'w' || *fen == 'b');
	
	pos->side = (*fen == 'w')? WHITE : BLACK;
	fen += 2;                                                 // Increament the pointer to capture the next character(twice: Because there is an extra space,so we ignor it)                      
	
	// castle permission
	for(i = 0; i < 4; i++){
		if(*fen == ' ') break;
		switch(*fen){
			// Assign the type of "castle permission" according to the corresponding characters in FEN notation
			case 'K': pos->castlePerm |= WKCA;	break;
			case 'Q': pos->castlePerm |= WQCA;	break;
			case 'k': pos->castlePerm |= BKCA;	break;
			case 'q': pos->castlePerm |= BQCA;	break;
			default : break;                                  // It may come '-' character,so we have to ignore it
		}
		fen++;                                                // Increament the pointer to capture the next character
	}
	
	fen++;
	
	// Debugging check: If "castle permission" is not out of range
	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);
	
	// en passent square
	if(*fen != '-'){
		file = fen[0] - 'a';                                  // Convert the chsrscter to the coreesponding file number using ASCII
		rank = fen[1] - '1';                                  // Convert the chsrscter to the coreesponding file number using ASCII
		
		// Debugging check: If the file and ran aren't out of range
		ASSERT(file >= FILE_A && file <= FILE_H);
		ASSERT(rank >= RANK_1 && rank <= RANK_8);
		
		pos->enPass = FR2SQ(file, rank);                      // Update the board's en passent square                                                                  
	}
	
	UpdateListsMaterial(pos);
	
	// position key
	pos->posKey = GeneratePosKey(pos);                        // Generate an new postion key corresponding to the new case then update the poskey
	pos->pawnKey = GeneratePawnKey(pos);
	
	return TRUE;                                              // Return 1 if no errors occure
}

/**
 * Reset all contents of the board.
 * 
 * @param pos The position's pointer to reset.
 */
void ResetBoard(S_BOARD *pos){
	
	u8 index = 0;
	
	for(index = 0; index < BRD_SQ_NUM; index++) pos->pieces[index] = OFF_BOARD;
	for(index = 0; index < 64; index++) pos->pieces[SQ120(index)] = EMPTY;
	
	for(index = 0; index < 2; index++){
		pos->bigPce[index] = 0;
		pos->majPce[index] = 0;
		pos->minPce[index] = 0;
		pos->material[index] = 0;
	}
	
	for(index = 0; index < 3; index++) pos->pawns[index] = 0ULL;
	
	for(index = 0; index < 13; index++) pos->pceNum[index] = 0;
	
	pos->kingSq[WHITE] = 0;
	pos->kingSq[WHITE] = 0;
	
	pos->side = BOTH;
	pos->enPass = NO_SQ;
	pos->fiftyMove = 0;
	
	pos->ply = 0;
	pos->hisply = 0;
	
	pos->castlePerm = 0;
	
	pos->posKey = 0ULL;
	pos->pawnKey = 0ULL;
	
}

/**
 * Printing BOARD's contents on console.
 * 
 * @param pos The position's pointer to print.
 */
void PrintBoard(const S_BOARD *pos){
	s8 sq, file, rank, piece;
	
	printf("\nGame Board:\n\n");
	SetColor(DARK_GRAY);
	printf("   +---+---+---+---+---+---+---+---+\n");
	for(rank = RANK_8; rank >= RANK_1; rank--){
		SetColor(_WHITE);
		printf(" %d ", rank + 1);
		SetColor(DARK_GRAY);
		printf("|");
		for(file = FILE_A; file <= FILE_H; file++){
			SetColor(_WHITE);
			sq = FR2SQ(file, rank);
			piece = pos->pieces[sq];
			if(PieceCol[piece] == BLACK) SetColor(LIGHT_GREEN);
			printf(" %c ",PieceChar[piece]);
			SetColor(DARK_GRAY);
			printf("|");
		}
		printf("\n   +---+---+---+---+---+---+---+---+\n");
	}
	
	SetColor(_WHITE);
	printf("\n    ");
	for(file = FILE_A; file <= FILE_H; file++) printf(" %c  ", file + 'a');
	SetColor(LIGHT_GRAY);
	printf("\n\n");
	printf("side      : %c\n", SideChar[pos->side]);
	printf("enPass    : %s\n", PrSq(pos->enPass));
	printf("castlePerm: %c%c%c%c\n",
			pos->castlePerm & WKCA? 'K' : '-',
			pos->castlePerm & WQCA? 'Q' : '-',
			pos->castlePerm & BKCA? 'k' : '-',
			pos->castlePerm & BQCA? 'q' : '-'
			);
#ifdef DEBUG
	printf("posKey: %llX\n", pos->posKey);
	printf("pawnKey: %llX\n", pos->pawnKey);
#endif
	
}

/**
 * Mirroring the board by swaping white to black and vice versa.
 * This function is used in evaluation debugging.
 * 
 * @param pos The position's pointer to mirror.
 */
void MirrorBoard(S_BOARD *pos){
	
	u8 tempPieces[64];
	u8 tempSide = pos->side ^ 1;
	u8 tempEnnPass = NO_SQ;
	u8 tempCastlePerm = 0;
	u8 SwapPieces[13] = {EMPTY, bP, bN, bB, bR, bQ, bK, wP, wN, wB, wR, wQ, wK};
	
	u8 sq;
	
	if(pos->castlePerm & WKCA) tempCastlePerm |= BKCA;
	if(pos->castlePerm & WQCA) tempCastlePerm |= BQCA;
	
	if(pos->castlePerm & BKCA) tempCastlePerm |= WKCA;
	if(pos->castlePerm & BQCA) tempCastlePerm |= WQCA;
	
	if(pos->enPass != NO_SQ) tempEnnPass = SQ120(MIRROR64(SQ64(pos->enPass)));
	
	for(sq = 0; sq < 64; sq++) tempPieces[sq] = pos->pieces[SQ120(MIRROR64(sq))];
	
	ResetBoard(pos);
	
	for(sq = 0; sq < 64; sq++) pos->pieces[SQ120(sq)] = SwapPieces[tempPieces[sq]];
	
	UpdateListsMaterial(pos);
	
	pos->side = tempSide;
	pos->castlePerm = tempCastlePerm;
	pos->enPass = tempEnnPass;
	pos->posKey = GeneratePosKey(pos);
	pos->pawnKey = GeneratePawnKey(pos);
	
	
	
	ASSERT(CheckBoard(pos));
	
}









