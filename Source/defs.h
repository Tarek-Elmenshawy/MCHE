/**
 * @file defs.h
 * This header file cointains all structures and macros.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#ifndef DEFS_H
#define DEFS_H

#include"stdio.h"
#include"stdlib.h"
#include"inttypes.h"

//#define EVAL_DEBUG
//#define DEBUG

/* These macros are used to define search configuration */
#define USE_TABLES						///< Macro to use hash table in searching process.
#define USE_NULLMOVE					///< Macro to use null-move technique in searching process.
#define USE_KILLERS						///< Macro to use killer moves (i.e., moves that cut off beta) in searching process.
#define USE_HISTORY						///< Macro to use history of best moves (i.e., moves that improve alpha) in searching process.
#define USE_EVALMOVE					///< Macro to use evaluation move technique in moves ordering.
#define USE_SEE							///< Macro to use static exchange evaluation technique in moves ordering.


#ifndef DEBUG
	/// Debugging funtion
	#define ASSERT(n)
#else
	/// Debugging funtion
	#define ASSERT(n)	\
	if(!(n)){	\
		printf("%s - Failed ", #n);	\
		printf("On %s", __DATE__);	\
		printf("At %s", __TIME__);	\
		printf("In Flie %s", __FILE__);	\
		printf("At Line %d\n", __LINE__);	\
		exit(1);	\
	}
#endif

/* Macros for SetColor flags */
#define _BLACK           0				///< Black color flag for SetColor().
#define BLUE             1				///< Blue color flag for SetColor().
#define GREEN            2				///< Green color flag for SetColor().
#define CYAN             3				///< Cyan color flag for SetColor().
#define RED              4				///< Red color flag for SetColor().
#define MAGENTA          5				///< Magneta color flag for SetColor().
#define BROWN            6				///< Brown color flag for SetColor().
#define LIGHT_GRAY       7				///< Light gray color flag for SetColor().
#define DARK_GRAY        8				///< Dark gray  color flag for SetColor().
#define LIGHT_BLUE       9				///< Light blue color flag for SetColor().
#define LIGHT_GREEN     10				///< Light green color flag for SetColor().
#define LIGHT_CYAN      11				///< Light cyan color flag for SetColor().
#define LIGHT_RED       12				///< Light red color flag for SetColor().
#define LIGHT_MAGENTA   13				///< Light magneta color flag for SetColor().
#define YELLOW          14				///< Yellow color flag for SetColor().
#define _WHITE          15				///< White color flag for SetColor().
 
typedef uint64_t u64;					///< Declares u64 to be an alias for the type uint64_t.
typedef uint8_t  u8;					///< Declares u8 to be an alias for the type uint8_t.
typedef int8_t   s8;					///< Declares s8 to be an alias for the type int8_t.
typedef int32_t  s32;					///< Declares s32 to be an alias for the type int32_t.
typedef uint32_t u32;					///< Declares u32 to be an alias for the type uint32_t.
typedef int16_t  s16;					///< Declares s16 to be an alias for the type int16_t.
typedef uint16_t u16;					///< Declares u16 to be an alias for the type uint16_t.

#define NAME         "MCHE 1.0"			///< Name and version of chess engine.
#define BRD_SQ_NUM    120				///< Number of board squres.

#define MAXDEPTH 			64			///< Maximum number of depth that we would expect in searching.
#define MAXGAMEMOVES 		350			///< Maximum number of moves that we would expect in a game.
#define MAXPOSITIONMOVES 	256			///< Maximum number of moves for a given position.
#define HASH_TABLE_SIZE_MB 	128			///< Size of hash table in MB.
#define EVAL_TABLE_SIZE_MB 	64			///< Size of evaluation table in MB.
#define PAWN_TABLE_SIZE_MB 	32			///< Size of pawn structure evaluation table in MB.

#define INFINITY 30000					///< Mathematical number represents infinity.
#define ISMATE (INFINITY - MAXDEPTH)    ///< Cantant value compared to the score to detect mate state.

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" ///< Starting position in FEN notation.           

/* Numerator constants */

// Types of pieces.
#define EMPTY	  0						///< Empty.
#define wP		  1						///< White pawn (piece type).
#define wN		  2						///< White knight (piece type).
#define wB		  3						///< White bishop (piece type).
#define wR		  4						///< White rook (piece type).
#define wQ		  5						///< White queen (piece type).
#define wK		  6						///< White king (piece type).
#define bP		  7						///< Black pawn (piece type).
#define bN	 	  8						///< Black knight (piece type).
#define bB	  	  9						///< Black bishop (piece type).
#define bR	 	 10						///< Black rook (piece type).
#define bQ	 	 11						///< Black queen (piece type).
#define bK	 	 12						///< Black king (piece type).

// Ranks.
#define FILE_A	  0						///< Index of file A
#define FILE_B	  1						///< Index of file B
#define FILE_C	  2						///< Index of file C
#define FILE_D	  3						///< Index of file D
#define FILE_E	  4						///< Index of file E
#define FILE_F	  5						///< Index of file F
#define FILE_G	  6						///< Index of file G
#define FILE_H	  7						///< Index of file H
#define FILE_NONE 8  					///< Index of none file

// Files.
#define RANK_1	  0						///< Index of rank 1
#define RANK_2	  1						///< Index of rank 2
#define RANK_3	  2						///< Index of rank 3
#define RANK_4	  3						///< Index of rank 4
#define RANK_5	  4						///< Index of rank 5
#define RANK_6	  5						///< Index of rank 6
#define RANK_7	  6						///< Index of rank 7
#define RANK_8	  7						///< Index of rank 8
#define RANK_NONE 8						///< Index of none rank

// Color of piece.
#define WHITE	  0						///< White flag
#define BLACK	  1						///< Black flag
#define BOTH	  2						///< White and black flag

/// Board squres indexes
enum{
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, NO_SQ, OFF_BOARD
};

#define TRUE  		 1					///< TRUE constant.
#define FALSE 		 0					///< TRUE constant.

// Castling flags
#define WKCA 		 1					///< White king cstling flag.
#define WQCA 		 2					///< White queen cstling flag.
#define BKCA 		 4					///< Black king cstling flag.
#define BQCA		 8					///< Black queen cstling flag.

// Numerator constants of interface modes
#define UCI_MODE	 0					///< UCI mode constant.
#define XBOARD_MODE	 1					///< XBoard mode constant.
#define CONSOLE_MODE 2					///< Console mode constant.

// Hash tables flags
#define HFNONE		 0					///< None flag constant of Hash Tables.
#define HFALPHA		 1					///< Alpha flag constant of Hash Tables.
#define HFBETA  	 2					///< Beta flag constant of Hash Tables.
#define HFEXACT		 3					///< Exact flag constant of Hash Tables.

/* Flags of pieces that can attack a square*/
#define ATTACK_NONE   0                 ///< No piece can attack
#define ATTACK_KQR    1                 ///< (King - Queen - Rook) can attack.
#define ATTACK_QR     2                 ///< (Queen - Rook) can attack.
#define ATTACK_KQBbP  3                 ///< (King - Queen - Bishop - Black Pawn) can attack.
#define ATTACK_KQBwP  4                 ///< (King - Queen - Bishop - White Pawn) can attack.
#define ATTACK_QB     5                 ///< (Queen - Bishop) can attack.
#define ATTACK_N      6                	///< Kinight can attack.

/* Game phase constans */
#define PHASE_OPENING        0			///< Opening game phase contant.
#define PHASE_MIDDLE         1			///< middle game phase contant.
#define PHASE_ENDING       	 2			///< Ending game phase contant.
#define PHASE_PAWN_ENDING  	 3 			///< Ending pawn game phase contant. No null-moves in this phase.

/* Contempt factor values */
#define CONTEMPT_OPENING  50			///< Contempt factor value for opening phase.
#define CONTEMPT_MIDDLE   25			///< Contempt factor value for middle phase.
#define CONTEMPT_ENDING    0			///< Contempt factor value for ending phase.

/// This is a struct to contain configuration options of the engine
typedef struct{
	u8 UseBook;							///< A flag to use opening book or not.
}S_OPTIONS;

/// This is a struct that represents an entry of hash table to store the data of search.
typedef struct{
	u64 posKey;							///< The position key that is an ID of a specified board position.
	s32 move;							///< The move is that the best for this board position.
	s16 score;							///< The score of this move.
	u8 depth;							///< The depth of search where this move is found.
	u8 flag;							///< The flag type (HFNONE, HFALPHA, HFBETA, HFEXACT).
}S_HASHENTRY;

/// This is a struct that represents one of evaluation table entries to store the evluation value of a specified board position.
typedef struct{
	u64 posKey;							///< The position key that is an ID of a specified board position.
	s16 eval;							///< The evaluation value of a specified board position.
}S_EVALENTRY;

/// This is a struct that represents one of evaluation table entries to store the pawn structure of a specified board position. 
typedef struct{
	u64 pawnKey;						///< The pawn key is an ID of a specified pawn structure.
	s8 whiteStructure;					///< The evaluation of the white pawn strucutre.
	s8 blackStructure;					///< The evaluation of the black pawn strucutre.
	u16 passers;						///< The white and black passed pawns.
}S_PAWNENTRY;

/// This is a struct to contain information about evaluation table (no. of entries and no. of times that data retrieve).
typedef struct{
	S_PAWNENTRY *pPawnTable;			///< A Pointer to the first entry of the pawn evaluation entries in the evaluation table.
	S_EVALENTRY *pEvalTable;			///< A Pointer to the first entry of the position evaluation entries in the evaluation table.
	u32 numEntriesPawn;					///< The number of pawn evaluation entries.
	u32 numEntriesEval;					///< The number of position evaluation entries.
	u32 evalhits;						///< the number of times that position evaluation entries are retrieved.
	u32 pawnhits;						///< the number of times that pawn evaluation entries are retrieved.
}S_EVALTABLE;

/// This is a struct to contain information about hash table (no. of entries, no. of times that data retrieve, no. of overwrite casee etc.).
typedef struct{
	S_HASHENTRY *pTableA;				///< A Pointer to the first entry of the hash table that overwrites when new data is found.
	S_HASHENTRY *pTableD;				///< A Pointer to the first entry of the hash table that overwrites when new deeper data is found.
	
	u32 numEntries;						///< The number of hash table entries.
	u32 newWrites;						///< The number of newwrites.
	u32 overWrites;						///< The number of ovewrites.
	u32 hit;							///< The number of times that stored data is retrieved.
	u32 cut;							///< The number of times that stored data lesd to cut in search tree.
	u32 pv;								///< The number of times that pv moves found in the hash table.
}S_HASHTABLE;

/// This is a struct to contain information about a generated move (move and move score).
typedef struct{
	/// An integer number that represents a move. This number cotains information about (from square, to square, ...).
	/**
	 *  Move Representation:\n
	 *  0000 0000 0000 0000 0000 0111 1111 -> From 0x7F \n
	 *  0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F \n
	 *  0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF \n
	 *  0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000 \n
	 *  0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000 \n
	 *  0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF \n
	 *  0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000 \n
	 */
	s32 move;					
	
	s32 score;							///< The score of move repressents of move priority, and socre is used in move ordering.
}S_MOVE;

/// This is a struct to contain information about a list of generated moves (moves and no. of moves).
typedef struct{
	S_MOVE moves[MAXPOSITIONMOVES]; 	///< Array of S_MOVE used in move generation for a given board position.
	u8 count;							///< The number of the actual generated moves.
}S_MOVELIST;

/// This is a struct to contain information about a specific move to be able to store history and not lose game data.
typedef struct{
	s32 move;							///< It is a bit field that represents a move in 25 bits. This number cotains information about (from square, to square, ...).
	u8 castlePerm;						///< An integer number that represents the castling permission flag in 4 bits(1001) according to numerator contants(WKCA = 1, ..., BQCA = 8).
	u8 enPass;							///< The en passent square if there is one active, if not that will be set to NO_SQ.
	u8 fiftyMove;						///< The fifty move counter, remember when that hits fifty move then the game is drawn.
	u64 posKey;							///< The position key that is an ID of a specified board position.
	u64 pawnKey;						///< The pawn key is an ID of a specified pawn structure.
}S_UNDO;

/// This is a struct to contain all information about board representation.
typedef struct{
	u8 pieces[BRD_SQ_NUM];              ///< An array of 120 elements to represent the total board in where stored integer values represent the status of our board.
    u64 pawns[3];                       ///< An array to represent the exsited pawns on 64 squares of the board using 64 bits. Three elements to represent three colors (WHITE, BLACK, BOTH).                          

	u8 kingSq[2];                       ///< An array of 2 elements to hold the current positions of two kings (kingSq[WHITE] = E1, kingSq[BLACK] = E8).
	
	u8 side;                            ///< The side represents the side/turn that have to play.
	u8 enPass;                          ///< The en passent square if there is one active, if not that will be set to NO_SQ.
	u8 fiftyMove;                       ///< The fifty move counter, remember when that hits fifty move then the game is drawn.
	
	u8 ply ;                            ///< Serach depth length
	s32 hisply;                         ///< Index To loook for and determine repetitions when we come to storing our history in histor array of type S_UNDO
	
	u8 castlePerm;                      ///< An integer number that represents the castling permission flag in 4 bits(1001) according to numerator contants(WKCA = 1, ..., BQCA = 8).

	u64 posKey;                         ///< The position key is a unique key, normally called hash key and represents an ID for each board position.						
	u64 pawnKey;						///< The pawn key is an ID of a specified pawn structure.
	
	// These arrays will be used when we're evaluating a given board position.
	u8 pceNum[13];                      ///< The number of pieces on the board where each index has a corresponding type according to numerator constants(0 = EMPTY, ..., 12 = bK)
	u8 bigPce[2];                       ///< Number of all pieces except pawns (bigPce[WHITE], bigPce[BLACK]).                      
	u8 majPce[2];                       ///< Number of rooks and queens and kings (majPce[WHITE], majPce[BLACK]). 
	u8 minPce[2];                       ///< Number of bishops and knights (minPce[WHITE], minPce[BLACK]). 
	u16 material[2];                    ///< The sum of material scores for each side (material[WHITE], material[BLACK]). 
	
	S_UNDO history[MAXGAMEMOVES];       ///< An array to store each move before it is made on the board.
	
	/// A list of types, numbers and positions of exsiting pieces on board(ex. pList[wN][0] = B1, pList[wN][1] = H1).
	/// The maximum no. of any piece dosen't exceed 10 that would happen if all pawns is promoted to rocks or bishops.
	u8 pList[13][10];
	
	S_EVALTABLE EvalTable[1];			///< A pinter to the evaluation table.
	S_HASHTABLE HashTable[1];			///< A pinter to the hash table.
	s32 PvArray[MAXDEPTH];				///< An array to store principle variation moves depends on searching process.
	
	u16 searchHistory[13][64];        	///< An array to store moves that improve alpha to be used to reduce searched nodes by increasing moves ordering.
	s32 searchKillers[2][MAXDEPTH];     ///< An array to store moves that cut off beta to be used to reduce searched nodes by increasing moves ordering.
}S_BOARD;

/// This is a struct to contain search information.
typedef struct{
	u32 starttime;						///< A variable to store the time of search starting.
	u32 stoptime;						///< A variable to store the time of search ending.
	u8 depth;							///< A variable to store the target depth of search (range = 1:64).
	u8 depthset;						///< A flag to use depth as a terminator of search (TRUE or FALSE).
	u8 timeset;							///< A flag to use time as a terminator of search (TRUE or FALSE).
	
	u32 nodes;							///< A variable to store number of searched nodes.
	
	u8 quit;							///< A flag to quit the chess engine (TRUE or FALSE).
	u8 stopped;							///< A flag to end search (TRUE or FALSE).
	
	float fh;							///< A variable to store information used in caculating percentage of move ordering.
	float fhf;							///< A variable to store information used in caculating percentage of move ordering.
	
	u32 nullCut;				   		///< A vriable to store number of times that null-move techinque is used.
	
	u8 GAME_MODE;						///< The interface mode (UCI_MODE, XBOARD_MODE, CONSOLE_MODE)
	u8 POST_THINKING;					///< A flag to print details of search steps (TRUE or FALSE).
	
	u8 ponder;							///< A flag to use pondering mode or not (TRUE or FALSE).
	u8 interrupt;						///< A flag to interrupt detection (TRUE or FALSE).
}S_SEARCHINFO;

// GAME MOVE 

/*
 *  Move Representation:
 *  0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
 *  0000 0000 0000 0011 1111 1000 0000 -> To >> 7, 0x7F
 *  0000 0000 0011 1100 0000 0000 0000 -> Captured >> 14, 0xF
 *  0000 0000 0100 0000 0000 0000 0000 -> EP 0x40000
 *  0000 0000 1000 0000 0000 0000 0000 -> Pawn Start 0x80000
 *  0000 1111 0000 0000 0000 0000 0000 -> Promoted Piece >> 20, 0xF
 *  0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
 */

#define FROMSQ(m)   (m & 0x7F)          ///< Returns the index of square where the piece move from
#define TOSQ(m)     ((m >> 7) & 0x7F)   ///< Returns the index of square where the piece move to
#define CAPTURED(m) ((m >> 14) & 0xF)   ///< Returns the type of the captured piece
#define PROMOTED(m) ((m >> 20) & 0xF)   ///< Returns the type of piece which the pawn is promoted into

#define MFLAGEP     0x40000             ///< Flag is uesd with (&) operator to check if there is EN PASSENT or not
#define MFLAGPS     0x80000             ///< Flag is uesd with (&) operator to check if it is the first move to the pawn or not
#define MFLAGCA     0x1000000           ///< Flag is uesd with (&) operator to check if there is CASTLING or not

#define MFLAGCAP    0x7C000             ///< Flag is uesd with (&) operator to check if there is CAPTURING or not         
#define MFLAGPRO    0xF00000            ///< Flag is uesd with (&) operator to check if there is PROMOTION or not

#define NOMOVE      0					///< None move constant.

// MACROS
#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))	///< Returns index120 by passing file and rank respectively
#define SQ64(sq120) (Sq120ToSq64[(sq120)])      ///< Returns converted index from 120 to 64
#define SQ120(sq64) (Sq64ToSq120[(sq64)])       ///< Returns converted index from 64 to 120

#define POP(b) PopBit(b)                        ///< Returns the index of the fist setted bit(1) from LSB to MSB afer reseting (0) -Used with 64 bit variables-
#define CNT(b) CountBits(b)                     ///< Reurn no. of setted bits(1) in a 64 bits variable
#define CLRBIT(bb, sq) (bb &= ~(1ULL << sq))    ///< Reset a specified bit in a 64 bits variable
#define SETBIT(bb, sq) (bb |=  (1ULL << sq))    ///< Set a specified bit in a 64 bits variable

#define IsKn(p) (PieceKnight[p])                ///< Returns TRUE if the passing piece is knight
#define IsKi(p) (PieceKing[p])                  ///< Returns TRUE if the passing piece is kings
#define IsBQ(p) (PieceBishopQueen[p])           ///< Returns TRUE if the passing piece is bishop or quuen
#define IsRQ(p) (PieceRookQueen[p])             ///< Returns TRUE if the passing piece is rook or queen
#define IsPn(p) (!PieceBig[p])                  ///< Returns TRUE if the passing piece is rook or queen

#define MIRROR64(sq) (Mirror64[(sq)])			///< Returns the mirrored square of the passing square.
#define SQOFFBOARD(sq) (Sq120ToSq64[(sq)] == OFF_BOARD)	///< Returns TRUE if the passing square is off th board.


// GLOBALS
extern u8 Sq120ToSq64[BRD_SQ_NUM];              ///< To convernt from 120 coordinates to 64 coordinates by storing in each index(120) the value of correponding index(64).
extern u8 Sq64ToSq120[64];                      ///< To convernt from 64 coordinates to 120 coordinates by storing in each index(64) the value of correponding index(120).

// Values used in genrating posKey 
extern u64 PieceKeys[13][64];					///< Postion key for each piece in all spots
extern u64 SideKey;								///< Postion key for side to move
extern u64 CastleKeys[16]; 						///< Postion key for castling

/// These array contain all character that are printed on screen
extern const char PieceChar[];					///< An array contains the characters of piece types.
extern const char SideChar[];					///< An array contains the characters of side colors.
extern const char RankChar[];					///< An array contains the characters of board ranks.
extern const char FileChar[];					///< An array contains the characters of board files.

extern const u8 PieceBig[13];                   ///< Returns true if the piece passed as an index is ine of all cjess pieces except pawn                        
extern const u8 PieceMaj[13];                   ///< Returns true if the piece passed as an index is a rook or queen or king
extern const u8 PieceMin[13];                   ///< Returns true if the piece passed as an index is knight or bishop
extern const s16 PieceVal[13];                  ///< Returns the value of the piece passed as an index          
extern const u8 PieceCol[13];                   ///< Returns the colour of the piece passed as an index

extern u8 FilesBrd[64];           		        ///< Returns the file of the square passed as an index
extern u8 RanksBrd[64];                		    ///< Returns the rank of the square passed as an index

extern u8 print;								///< A flag used in evalution debugging

/// These arrays give an answer about a specified question by passing the type of piece as an index
extern const u8 PiecePawn[13];                  ///< Returns true if the piece passed as an index is a pawn
extern const u8 PieceKnight[13];                ///< Returns true if the piece passed as an index is a knight
extern const u8 PieceKing[13];                  ///< Returns true if the piece passed as an index is a king
extern const u8 PieceRookQueen[13];             ///< Returns true if the piece passed as an index is a rook or queen
extern const u8 PieceBishopQueen[13];           ///< Returns true if the piece passed as an index is a bidhop or queen

extern const u8 Mirror64[64];					///< Returns the mirrored square of the passing square.

extern const u8 AttackArray[155];				///< An array contains some precalculated informatin used in attack check.

/// These arrays contain all direction that pieces can make on board
extern const s8 KnDir[8];                       ///< An array contains all directions that knights can make on board.
extern const s8 RkDir[4];                       ///< An array contains all directions that rooks can make on board.
extern const s8 BiDir[4];                       ///< An array contains all directions that bishops can make on board.
extern const s8 KiDir[8];                       ///< An array contains all directions that kings can make on board.
extern S_OPTIONS EngineOptions[1];				///< A pointer to a engine options struct.

#define INPUTBUFFER 400 * 6						///< Size of interrupt buffer.
extern char interrupt_string[INPUTBUFFER];		///< Interrupt buffer to store incoming data.


/* FUNCTIONS PROTOTYPES */

// init.c
extern void AllInit();							///< Initializing all used information arrays.

// bitboard.c
extern void PrintBitBoard(u64);                 ///< Print u64 variable as board on console.
extern u8 PopBit(u64 *bb);                      ///< Returns the index of the first setted(1) bit from LSB to MSB and resets it into 0.       
extern u8 CountBits(u64 b);                     ///< Returns the nnumber of setted bits.

// hashkeys.c
extern u64 GeneratePosKey(const S_BOARD *pos);         	///< Returns a unique posKey for each board posiyions by using Piece, Side and Castle keys arrays.
extern u64 GeneratePawnKey(const S_BOARD *pos);		   	///< Returns a unique posKey for each pawn structure.

// board.c
extern void ResetBoard(S_BOARD *pos);                  	///< Reset all contents of the board.
extern u8 ParseFen(const char *fen, S_BOARD *pos);   	///< Parse the incomming FEN notation and impelement it on our BOARD.
extern void PrintBoard(const S_BOARD *pos);            	///< Print BOARD's contents on screen.
extern void UpdateListsMaterial(S_BOARD *pos);         	///< Update pieces list.
extern u8 CheckBoard(const S_BOARD *pos);            	///< Check all contents of oard and it is uesd in debugging in most of engine.
extern void MirrorBoard(S_BOARD *pos);				   	///< Mirroring the board by swaping white to black and vice versa.

// attack.c
extern u8 IsSqAttacked(const u8 sq, const u8 side, const S_BOARD *pos);///< To check the passed square is in attack by the passed side or not.

// io.c
extern char *PrSq(const u8 sq);                      	///< Returns a pointer to an array of characters contained the algebraic form of square which is uesd in printing on console.
extern char *PrMove(const s32 move);                   	///< Returns a pointer to an array of characters contained the algebraic form of move which is uesd in printing on console.
extern void PrintMoveList(const S_MOVELIST *list);	   	///< Print moves in the passed structure.
extern s32 ParseMove(const char *getcher, S_BOARD *pos);///< Convert move from string (e.g. e2e4) into intger move.
extern char *SpePrMove(const s32 move);					///< Returns a pointer to an array of characters contained the custom algebraic form of move which is uesd in printing on console.

// validate.c
extern u8 SqIs120(const u8 sq);										///< Check if the given square on 120 board or 64 board.
extern u8 SqOnBoard(const u8 sq);									///< Check if the given square on board or out of board.
extern u8 SideValid(const u8 s);									///< For side validation. 
extern u8 FileRankValid(const u8 fr);								///< For file validation.
extern u8 PieceValid(const u8 p);									///< Check if the given piece is valid or not.
extern u8 PieceValidEmpty(const u8 p);								///< Check if the given piece is a valid piece or empty or not.
extern u8 MoveListOk(const S_MOVELIST *list, const S_BOARD *pos);	///< Check if the given list of moves is vaild or not.
extern u8 MoveValid(const s32 move, const S_BOARD *pos);			///< Check if the given move is valid or not.
extern void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info);	///< This function is used to make a performance test for the chess engine thinking.
extern void MirrorEvalTest(S_BOARD *pos);							///< This function is used to check the performace of EvalPosition().
extern void DebugSEETest(S_BOARD *pos);								///< This function is used to check the performace of SEE().
extern u8 CapturesListOk(const S_MOVELIST *list, const S_BOARD *pos);///< Check if the given list of capture moves is valid or not.
extern void CheckFen(S_BOARD *pos);									///< Check if the given FEN is valid or not.
extern void DebugPerftTest(S_BOARD *pos);							///< Used for test some pre-calculated cases.

// movege.c
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list); ///< Generates all possible moves without filterating.
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);	///< Generates all possible captured moves without filterating.
extern u8 MoveExists(S_BOARD *pos, const s32 move);  				///< Check if the given move exists on the current board or not.
// extern void InitMvvLva();
extern void GenerateMovesForLocation(const S_BOARD *pos, S_MOVELIST *list, const u8 sq); ///< Generates all possible moves for a given piece on a square on board without filterating.

// makemove.c
extern void UnMakeMove(S_BOARD *pos);								///< Undo the last move and return to the previos move using history structure.			
extern u8 MakeMove(S_BOARD *pos, s32 move);							///< Performs some changes on the board to make a move on boud then updates board information.
extern void MakeNullMove(S_BOARD *pos);								///< Performs some changes on the board to make a null-move on boud
extern void UnMakeNullMove(S_BOARD *pos);							///< Undo the last null-move and return to the previos move using history structure.

// perft.c
extern u64 PerftTest(const u8 depth, S_BOARD *pos);					///< Counts all the leaf nodes of a certan depth.
extern s32 SEETest(const S_BOARD *pos);								///< Used for SEE testing.

// search.c
extern s32 SearchDebug(S_BOARD *pos, S_SEARCHINFO *info, s8 *moveTo, u8 am);///< Used for test some pre-calculated cases.
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);		///< Search for the best move for the given position.
extern u8 GetGamePhase(const S_BOARD *pos);							///< Return the current phase game.

// misc.c
extern s32 GetTimeMs();                            	   				///< Returns the current time in millisecodes
extern void ReadInput(S_SEARCHINFO *info);			   				///< Check if engine is interrupted or not and store check in info sturcture
extern void SetColor(s32 ForgC);					   				///< Setup console font color

// hashtable.c
/// Allocate memory for hash tables.
extern void InitHashTable(S_HASHTABLE *table, const u8 MB);
/// Allocate memory for evaluation tables.
extern void InitEvalTable(S_EVALTABLE *table, const u8 MB_PAWN, const u8 MB_EVAL);
/// Free all allocated memory of hash tables, evaluation tables and memory used for opening book
extern void CleanMemory(S_HASHTABLE *hashTable, S_EVALTABLE *evalTable);
/// Clear all the entries of the hash table to ensure thers is no garbage.
extern void ClearHashTable(S_HASHTABLE *table);
/// Store a hash entry in the two hash tables.
extern void StoreHashEntry(S_BOARD *pos, const s32 move, s16 score, const u8 flag, const u8 depth);
/// Returns the pv move stored in hash tables for a given position.
extern s32 ProbePvTable(const S_BOARD *pos);
/// Returns a thinking line depends on prinsiple variation moves
extern u8 GetPvLine(const u8 depth, S_BOARD *pos);
/// Probe a hash entry in the two hash tables and returns entry information of a given position.
extern u8 ProbeHashEntry(S_BOARD *pos, s32 *move, s16 *score, const s16 alpha, const s16  beta, const u8 depth);
/// Probe an evaluation entry in the evaluation tables and returns entry information of a given position.
extern u8 ProbeEval(S_BOARD *pos, s16 *eval);
/// Probe a pawn structure evaluation entry in the pawn structure tables and returns entry information of a given position.
extern u8 ProbePawnEval(S_BOARD *pos, s8 *whiteStructure, s8 *blackStructure, u16 *passers);
/// Store the pawn structure evaluation value of a given position.
extern void StoreEval(S_BOARD *pos, s16 eval);
/// Store the evaluation value of a given position.
extern void StorePawnEval(S_BOARD *pos, s8 whiteStructure, s8 blackStructure, u16 passers);

// evaluate.c
extern s16 EvalPosition(S_BOARD *pos);								///< Caculate the total evaluation value for a given board position.
extern s8 EvalMove(const S_BOARD *pos, const u8 from, const u8 to);	///< Caculate the evaluation value for a given move depends on piece positions.

// uci.c
extern void UCI_Loop(S_BOARD *pos, S_SEARCHINFO *info);				///< This function handles some commands of UCI protocol in addition to some custom commands.

// xboard.c
extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);			///< This function handles some commands of XBoard protocol.
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);			///< This function handles some commands of Console mode.

// see.c
extern s32 SEE(const S_BOARD *pos, s32 move);						///< Return nalue of ststic exchange evaluation for the given maove.

// polybook.c
extern s32 GetBookMove(S_BOARD *ppos);								///< Return the best move for the current board position in the opening book.
extern void InitPolyBook();											///< Read opening book.bin and load in RAM.
extern void CleanPolyBook();										///< Free the allocated memory for the opening book in RAM.

#endif
