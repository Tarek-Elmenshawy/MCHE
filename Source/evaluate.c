/**
 * @file evaluate.c
 * This file  includes evaluation functions.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))		///< Retuens the maximun number
#define MIN(x, y) (((x) < (y)) ? (x) : (y))		///< Retuens the minimum number

#define COUNT_BIT         	 7					///< Count mask of WB/BB.
#define PAWN_BIT       	     8					///< Pawn mask of WB/BB.
#define MINOR_BIT        	16					///< Minor mask of WB/BB.
#define ROOK_BIT         	32					///< Rook mask of WB/BB.
#define QUEEN_BIT        	64					///< Queen mask of WB/BB.
#define KING_BIT           128					///< King mask of WB/BB.
#define ATTACKERS_MASK   	31					///< Attackers mask of KING_ATTACK_PATTERN.

#define QUEEN_VALUE  	   975					///< Evaluation value of queen.
#define ROOK_VALUE 		   500					///< Evaluation value of rook.
#define BISHOP_VALUE  	   325					///< Evaluation value of bishop.
#define KNIGHT_VALUE  	   325					///< Evaluation value of knight.
#define PAWN_VALUE  	   100					///< Evaluation value of pawn.

// Evaluation constants
#define PINNED_PIECE  		20					///< Evaluation value (bonus) of the pinned piece.
#define PASSED_PAWN  		20					///< Evaluation value (bonus) of the passed pawn.
#define DOUBLED_PAWN  		10					///< Evaluation value (bonus) of the doubled pawn.
#define ISOLATED_PAWN  		20					///< Evaluation value (bonus) of the isolated pawn.
#define WEAK_PAWN  			15					///< Evaluation value (bonus) of the weak pawn.
#define ROOK_ON_SEVENTH  	20					///< Evaluation value (bonus) of the rook is on 7th or 2nd rank.
#define ROOK_BEHIND_PASSER  15					///< Evaluation value (bonus) of the rook is behind passers.
#define QUEEN_ON_SEVENTH  	10					///< Evaluation value (bonus) of the queen is on 7th or 2nd rank.
#define ROOK_ON_OPEN  		20					///< Evaluation value (bonus) of the rook is on open file.
#define ROOK_ON_SEMI  		15					///< Evaluation value (bonus) of the rook is on semi-open file (with only enemy pawns).
#define HUNG_PIECE_PENALTY  80					///< Evaluation value (bonus) of the penalty of hung piece.
#define BISHOP_PAIR  		50					///< Evaluation value (bonus) of the bishop pair.
#define TEMPO  				10					///< Evaluation value (bonus) of the tempo in opening and middel phase.

/*
 *  Index [0 : 63] -> square on board
 *  Bit Field Representation od WB/BB arrays:
 *  0000 0111 -> Number of attackers to this square >> 7
 *  0000 1000 -> Pawn attacks this square >> 8
 *  0001 0000 -> Minor (knught or bishop) attack this square >> 16
 *  0010 0000 -> Rook attacks this square >> 32
 *  0100 0000 -> Queen attacks this square >> 64
 *  1000 0000 -> King attacks this square >> 128
 *
 */
static u8 WB[64] = {0};						///< Bit field attacked squares on white board.
static u8 BB[64] = {0};						///< Bit field attacked squares on black board.
static u8 wPPos[10];
static u8 bPPos[10];
static u16 passers;

static const u8 FILE_TO_BIT_MASK[8] = {1,2,4,8,16,32,64,128};

static u8 w_bestPromDist;
static u8 b_bestPromDist;

static s16 gamePhase;


/* The following pos The position's pointers should be read as follow:

   a1,  b1,  c1,  d1,  e1,  f1,  g1,  h1,
   a2,  b2,  c2,  d2,  e2,  f2,  g2,  h2,
   a3,  b3,  c3,  d3,  e3,  f3,  g3,  h3,
   a4,  b4,  c4,  d4,  e4,  f4,  g4,  h4,
   a5,  b5,  c5,  d5,  e5,  f5,  g5,  h5,
   a6,  b6,  c6,  d6,  e6,  f6,  g6,  h6,
   a7,  b7,  c7,  d7,  e7,  f7,  g7,  h7,
   a8,  b8,  c8,  d8,  e8,  f8,  g8,  h8,
*/

/// Positioning of the knights
static const s16 wN_POS[] = {		
	-50	, -40, -30, -25, -25, -30, -40, -50,		
	-35	, -25, -15, -10, -10, -15, -25, -35,	
	-20	, -10,   0,   5,   5,   0, -10, -20,	
	-10	,   0,  10,  15,  15,  10,   0, -10,	
	-5	,   5,  15,  20,  20,  15,   5,  -5,		
	-5	,   5,  15,  20,  20,  15,   5,  -5,	
	-20	, -10,   0,   5,   5,   0, -10, -20,	
	-135, -25, -15, -10, -10, -15, -25,-135
};

static const s8 KNIGHT_POS_ENDING[] = {
	-10,  -5,  -5,  -5,  -5,  -5,  -5, -10,    
	-5,   0,   0,   0,   0,   0,   0,  -5,    
	-5,   0,   5,   5,   5,   5,   0,  -5,    
	-5,   0,   5,  10,  10,   5,   0,  -5,    
	-5,   0,   5,  10,  10,   5,   0,  -5,    
	-5,   0,   5,   5,   5,   5,   0,  -5,    
	-5,   0,   0,   0,   0,   0,   0,  -5,    
	-10,  -5,  -5,  -5,  -5,  -5,  -5, -10
};

/// Positioning of the bishops
static const s8 wB_POS[] = {
   -20, -15, -15, -13, -13, -15, -15, -20,    
	-5,   0,  -5,   0,   0,  -5,   0,  -5,    
	-6,  -2,   4,   2,   2,   4,  -2,  -6,    
	-4,   0,   2,  10,  10,   2,   0,  -4,    
	-4,   0,   2,  10,  10,   2,   0,  -4,    
	-6,  -2,   4,   2,   2,   4,  -2,  -6,    
	-5,   0,  -2,   0,   0,  -2,   0,  -5,    
	-8,  -8,  -6,  -4,  -4,  -6,  -8,  -8
};

static const s8 BISHOP_POS_ENDING[] = {
	-18, -12,  -9,  -6,  -6,  -9, -12, -18,   
	-12,  -6,  -3,   0,   0,  -3,  -6, -12,   
	 -9,  -3,   0,   3,   3,   0,  -3,  -9,   
	 -6,   0,   3,   6,   6,   3,   0,  -6,   
	 -6,   0,   3,   6,   6,   3,   0,  -6,   
	 -9,  -3,   0,   3,   3,   0,  -3,  -9,   
	-12,  -6,  -3,   0,   0,  -3,  -6, -12,   
	-18, -12,  -9,  -6,  -6,  -9, -12, -18
};


static const s8 wR_POS[] = {
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6,    
	-6,   -3,   0,   3,   3,   0,   -3,   -6
};

static const s8 ROOK_POS_ENDING[] = {
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0
};

static const s8 wQ_POS[] = {
  -10, -10, -10, -10, -10, -10, -10, -10,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0
};

static const s8 QUEEN_POS_ENDING[] = {
	-24, -16, -12,  -8,  -8, -12, -16, -24,    
	-16,  -8,  -4,   0,   0,  -4,  -8, -16,    
	-12,  -4,   0,   4,   4,   0,  -4, -12,    
	 -8,   0,   4,   8,   8,   4,   0,  -8,    
	 -8,   0,   4,   8,   8,   4,   0,  -8,    
	-12,  -4,   0,   4,   4,   0,  -4, -12,    
	-16,  -8,  -4,   0,   0,  -4,  -8, -16,    
	-24, -16, -12,  -8,  -8, -12, -16, -24
};

/// Positioning of the pawns
static const s8 wP_POS[] = {
	-15,   -5,   0,   5,   5,   0,   -5,   -15,    
	-15,   -5,   0,   5,   5,   0,   -5,   -15,    
	-15,   -5,   0,  15,  15,   0,   -5,   -15,    
	-15,   -5,   0,  25,  25,   0,   -5,   -15,    
	-15,   -5,   0,  15,  15,   0,   -5,   -15,    
	-15,   -5,   0,   5,   5,   0,   -5,   -15,    
	-15,   -5,   0,   5,   5,   0,   -5,   -15,    
	-15,   -5,   0,   5,   5,   0,   -5,   -15
};

/// The following two king positions will be used in opening and middle game
static const s8 wK_POS[] = {
	
	 10,  20,   0,   0,   0,  10,  20,  10,    
	 10,  15,   0,   0,   0,   0,  15,  10,    
	-10, -20, -20, -25, -25, -20, -20, -10,    
	-15, -25, -40, -40, -40, -40, -25, -15,    
	-30, -40, -40, -40, -40, -40, -40, -30,    
	-40, -50, -50, -50, -50, -50, -50, -40,    
	-50, -50, -50, -50, -50, -50, -50, -50,    
	-50, -50, -50, -50, -50, -50, -50, -50
};		

/// Used to encourage the kings to move to the center in the ending
static const s8 KING_POS_ENDING[] = {
	
	-20, -15, -10, -10, -10, -10, -15, -20,
	-15,  -5,   0,   0,   0,   0,  -5, -15,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,  10,  10,   5,   0, -10,
	-10,   0,   5,   5,   5,   5,   0, -10,
	-15,  -5,   0,   0,   0,   0,  -5, -15,
	-20, -15, -10, -10, -10, -10, -15, -20
};	

//// Marks the outpost squares for knight, do not put outpost values
//// on the edges since we check for protecting pawns without checking out of pos The position's pointer
static const s8 wN_OUTPOST[] = {
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   2,   5,  10,  10,   5,   2,   0,    
	0,   2,   5,  10,  10,   5,   2,   0,    
	0,   0,   4,   5,   5,   4,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0,    
	0,   0,   0,   0,   0,   0,   0,   0
};	


/// Straight from Ed Schrِder's site
static const s8 KING_ATTACK_PATTERN[] = {
//. P N N R R R R Q Q Q Q Q Q Q Q K K K K K K K K K K K K K K K K
//      P   P N N   P N N R R R R   P N N R R R R Q Q Q Q Q Q Q Q
//              P       P   N N N       P   P N N   P N N R R R R
   0,0,0,0,0,0,1,1,0,1,2,2,2,3,3,3,0,0,0,0,1,1,2,2,2,3,3,3,3,3,3,3 
};


static const s16 KING_ATTACK_EVAL[] = { 
	0,  2,  3,  6, 12, 18, 25, 37, 50, 75,
	100,125,150,175,200,225,250,275,300,325,
	350,375,400,425,450,475,500,525,550,575, 
	600,600,600,600,600,600,600,600,600,600,
	600,600,600,600,600,600,600,600,600,600,
	600,600,600,600,600,600,600,600,600,600,
	600,600,600,600,600,600,600,600,600,600,
	600,600,600,600,600,600,600,600,600,600
};

static const u8 FIRST_BIT_TO_FILE_MASK[] = {
	8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
	4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
};

/// The closer the piece is to the opponent's king the better,
/// knights score higher than bishops since bishops can attack from afar as well
static const u8 TROPISM_KNIGHT[]    = {0, 3, 3, 2, 1, 0, 0, 0};
static const u8 TROPISM_BISHOP[] 	 = {0, 2, 2, 1, 0, 0, 0, 0};
static const u8 TROPISM_ROOK[]   	 = {0, 3, 2, 1, 0, 0, 0, 0};
static const u8 TROPISM_QUEEN[]  	 = {0, 4, 3, 2, 1, 0, 0, 0};

static const u8 PASSED_RANK_BONUS[] = {0,10,20,40,60,120,150,0}; 

/**
 * Returns the shortest distance between two squares.
 * 
 * @param squareA
 * @param squareB
 * @return distance: The distance between the squares
 */
static u8 Distance(u8 squareA, u8 squareB){
	
	ASSERT(SqOnBoard(squareA));
	ASSERT(SqOnBoard(squareB));
	return MAX(abs(FilesBrd[SQ64(squareA)] - FilesBrd[SQ64(squareB)]), abs(RanksBrd[SQ64(squareA)] - RanksBrd[SQ64(squareB)]));
}

/**
 * Check if drawn by material or not. 
 *
 * @param pos The position's pointer to check.
 * @param side 1 to check if black has enough material to win, 0 if white, and 2 if both.
 * @return TRUE if drawn, FALSE if not.
 */
static u8 DrawByMaterial(const S_BOARD *pos, u8 side){
	
	if(side == WHITE){
		
		if(pos->pceNum[wP] != 0 || pos->pceNum[wR] != 0 || pos->pceNum[wQ] != 0 || pos->pceNum[wB] > 1 || pos->pceNum[wN] > 2){
			return FALSE;
		}
		if((pos->pceNum[wB] > 0 && pos->pceNum[wN] > 0)) return FALSE;
		
		return TRUE;
	}else if(side == BLACK){
		
		if(pos->pceNum[bP] != 0 || pos->pceNum[bR] != 0 || pos->pceNum[bQ] != 0 || pos->pceNum[bB] > 1 || pos->pceNum[bN] > 2){
			return FALSE;
		}
		if((pos->pceNum[bB] > 0 && pos->pceNum[bN] > 0)) return FALSE;

		return TRUE;
	}
	
	if(pos->pceNum[wP] != 0 || pos->pceNum[bP] != 0 || pos->pceNum[wR] != 0 || pos->pceNum[bR] != 0 || pos->pceNum[wQ] != 0 ||
	   pos->pceNum[bQ] != 0 || pos->pceNum[wB] > 1 || pos->pceNum[bB] > 1 ||  pos->pceNum[wN] > 2 || pos->pceNum[bN] > 2){
		return FALSE;
	}
	
	if((pos->pceNum[wB] > 0 && pos->pceNum[wN] > 0) || (pos->pceNum[bB] > 0 && pos->pceNum[bN] > 0)){
		return FALSE;
	}
	

	return TRUE;
}

/**
 * Takes the total evaluation and adjusts towards 0 if a draw is likely.
 * 
 * @param pos The position's pointer.
 * @param totalEval The total evaluation before adjustment.
 * @return adjustedScore The final evaluation after adjustment.
 */
static s16 DrawProbability(const S_BOARD *pos, s16 totalEval){

	s16 adjustedScore = totalEval;
	u8 w_rank,w_file,b_rank,b_file;
	u8 fifty = pos->fiftyMove;
	
	
	if(gamePhase == PHASE_ENDING){
	
		// If the score says one side is winning but that side does not
		// have enough material to win (e.g. only a bishop) return draw
		if(adjustedScore > 0 && DrawByMaterial(pos, WHITE)) return 0;
		if(adjustedScore < 0 && DrawByMaterial(pos, BLACK)) return 0;
		
		// Opposite color bishops
		// If there is exactly one bishop on both sides and they
		// are of opposite color and neither side has more that 4 pawns,
		// reduce the score towards 0 with 20%
		if(pos->pceNum[wB] == 1 && pos->pceNum[bB]  == 1 && pos->pceNum[wP]  <= 4 && pos->pceNum[bP]  <= 4){
			
			w_rank = RanksBrd[SQ64(pos->pList[wB][0])];
			w_file = FilesBrd[SQ64(pos->pList[wB][0])];
			b_rank = RanksBrd[SQ64(pos->pList[bB][0])];
			b_file = FilesBrd[SQ64(pos->pList[bB][0])];
			ASSERT(FileRankValid(w_rank));
			ASSERT(FileRankValid(b_rank));
			ASSERT(FileRankValid(b_file));
			ASSERT(FileRankValid(w_file));
			
			if(((w_rank + w_file) & 1) != ((b_rank + b_file) & 1)) adjustedScore = 80 * adjustedScore / 100;
			
			
		}
	}
	
	// 50 moves rule
	// Starting at move 20 without pawn moves or captures the score
	// is adjusted with 1% towards 0 for every move, this way
	// it won't suddenly run into the 50 moves rule 
	if(fifty > 20) adjustedScore = (120 - fifty) * adjustedScore / 100;
	
	

	return adjustedScore;		
}


/**
 * Takes a few common trapped piece patterns and checks
 * if any of them exists on the pos The position's pointer.
 * 
 * Also includes some devolopment pattern like blocked center pawns.
 * 
 * For white pieces.
 * 
 * @param pos The position's pointer.
 * @return trapped_eval The penalty if one or more patterns exists 0 if no exist.
 */
static s16 WhiteTrapped(const S_BOARD *pos){
	s16 trapped_eval = 0;
	
	// Knights
	/* 
	 * * * * * * * *	* * * * * * * *		N * * * * * * *		* * * * * * * N
	 N p * * * * * *	* * * * * * p N		p * p * * * * *		* * * * * p * p
	 * * p * * * * *	* * * * * p * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 */
	if(pos->pieces[A7] == wN && pos->pieces[B7] == bP && pos->pieces[C6] == bP) trapped_eval -= 100;
	
	if(pos->pieces[H7] == wN && pos->pieces[G7] == bP && pos->pieces[F6] == bP) trapped_eval -= 100;
	
	if(pos->pieces[A8] == wN && (pos->pieces[A7] == bP || pos->pieces[C7] == bP)) trapped_eval -= 50;
	
	if(pos->pieces[H8] == wN && (pos->pieces[H7] == bP || pos->pieces[F7] == bP)) trapped_eval -= 50;
	
	
	// Bishops
	/* 
	 * * * * * * * *	* B * * * * * *		* * * * * * * *		* * * * * * B *
	 B * p * * * * *	* * p * * * * *		* * * * * p * B		* * * * * p * *
	 * p * * * * * *	* p * * * * * *		* * * * * * p *		* * * * * * p *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 * * * * * * * *	* * * * * * * *		* * * * * * * *		* * * * * * * *
	 */
	if(pos->pieces[A7] == wB && pos->pieces[B6] == bP){
		trapped_eval -= 100;
		if(pos->pieces[C7] == bP) trapped_eval -= 50; 		// Even more if the trapping pawn is supported
	}
	
	if(pos->pieces[B8] == wB && pos->pieces[C7] == bP){
		trapped_eval -= 100;
		if(pos->pieces[B6] == bP) trapped_eval -= 50; 		// Even more if it can't get out via A7
	}
	
	if(pos->pieces[H7] == wB && pos->pieces[G6] == bP){
		trapped_eval -= 100;
		if(pos->pieces[F7] == bP) trapped_eval -= 50; 		// Even more if the trapping pawn is supported
	}
	
	if(pos->pieces[G8] == wB && pos->pieces[F7] == bP){
		trapped_eval -= 100;
		if(pos->pieces[G6] == bP) trapped_eval -= 50; 		// Even more if it can't get out via H7
	}
	
	/* 
	 * * * * * * * *	* * * * * * * *
	 * * * * * * * *	* * * * * * * *
	 B * * * * * * *	* * * * * * * B
	 * p * * * * * *	* * * * * * p *	
	 * * * * * * * *	* * * * * * * *	
	 * * * * * * * *	* * * * * * * *	
	 * * * * * * * *	* * * * * * * *	
	 * * * * * * * *	* * * * * * * *	
	 */
	if(pos->pieces[A6] == wB && pos->pieces[B5] == bP) trapped_eval -= 100;
	
	if(pos->pieces[H6] == wB && pos->pieces[G5] == bP) trapped_eval -= 100;

	
	// Rooks (trapped in the corner by the own king)
	if((pos->pieces[G1] == wR || pos->pieces[G2] == wR || pos->pieces[H1] == wR || pos->pieces[H2] == wR) && (pos->pieces[G1] == wK || pos->pieces[F1] == wK)) trapped_eval -= 50;
	
	if((pos->pieces[A1] == wR || pos->pieces[A2] == wR || pos->pieces[B1] == wR || pos->pieces[B2] == wR) && (pos->pieces[C1] == wK || pos->pieces[B1] == wK)) trapped_eval -= 50;
	
	
	// Blocked center pawn
	/* 
	 * * * * * * * *	* * * * * * * *
	 * * * * * * * *	* * * * * * * *
	 * * * * * * * *	* * * * * * * *
	 * * * * * * * *	* * * * * * * *	
	 * * * * * * * *	* * * * * * * *	
	 * * * O * * * *	* * * * O * * *	
	 * * * P * * * *	* * * * P * * *	
	 * * B * * * * *	* * * * * B * *	
	 */
	if(pos->pieces[D2] == wP && pos->pieces[D3] != EMPTY){
		trapped_eval -= 20;
		if(pos->pieces[C1] == wB) trapped_eval -= 30; 		// Even more if there is still a bishop on c1
	}
	
	if(pos->pieces[E2] == wP && pos->pieces[E3] != EMPTY){
		trapped_eval -= 20;
		if(pos->pieces[F1] == wB) trapped_eval -= 30; 		// Even more if there is still a bishop on f1
	}
	
	
	return trapped_eval;		
}


/**
 * Takes a few common trapped piece patterns and checks
 * if any of them exists on the pos The position's pointer.
 * 
 * Also includes some devolopment pattern like blocked center pawns.
 * 
 * For black pieces.
 * 
 * @param pos The position's pointer.
 * @return trapped_eval The penalty if one or more patterns exists 0 if no exist.
 */
static s16 BlackTrapped(const S_BOARD *pos){
	s16 trapped_eval = 0;
	
	// Knights
	if(pos->pieces[A2] == bN && pos->pieces[B2] == wP && pos->pieces[C3] == wP) trapped_eval -= 100;
	
	if(pos->pieces[H2] == bN && pos->pieces[G2] == wP && pos->pieces[F3] == wP) trapped_eval -= 100;
	
	if(pos->pieces[A1] == bN && (pos->pieces[A2] == wP || pos->pieces[C2] == wP)) trapped_eval -= 50;
	
	if(pos->pieces[H1] == bN && (pos->pieces[H2] == wP || pos->pieces[F2] == wP)) trapped_eval -= 50;
	
	
	// Bishops
	if(pos->pieces[A2] == bB && pos->pieces[B3] == wP){
		trapped_eval -= 100;
		if(pos->pieces[C2] == wP) trapped_eval -= 50; 		// Even more if the trapping pawn is supported
	}
	
	if(pos->pieces[B1] == bB && pos->pieces[C2] == wP){
		trapped_eval -= 100;
		if(pos->pieces[B3] == wP) trapped_eval -= 50; 		// Even more if it can't get out via A7
	}
	
	if(pos->pieces[H2] == bB && pos->pieces[G3] == wP){
		trapped_eval -= 100;
		if(pos->pieces[F2] == wP) trapped_eval -= 50; 		// Even more if the trapping pawn is supported
	}
	
	if(pos->pieces[G1] == bB && pos->pieces[F2] == wP){
		trapped_eval -= 100;
		if(pos->pieces[G3] == wP) trapped_eval -= 50; 		// Even more if it can't get out via H7
	}		
	
	if(pos->pieces[A3] == bB && pos->pieces[B4] == wP) trapped_eval -= 100;
	
	if(pos->pieces[H3] == bB && pos->pieces[G4] == wP) trapped_eval -= 100;
	
	
	// Rooks (trapped in the corner by the own king)
	if((pos->pieces[G8] == bR || pos->pieces[G7] == bR || pos->pieces[H8] == bR || pos->pieces[H7] == bR) && (pos->pieces[G8] == bK || pos->pieces[F8] == bK)) trapped_eval -= 50;
	
	if((pos->pieces[A8] == bR || pos->pieces[A7] == bR || pos->pieces[B8] == bR || pos->pieces[B7] == bR) && (pos->pieces[C8] == bK || pos->pieces[B8] == bK)) trapped_eval -= 50;
	
	
	// Blocked center pawn
	if(pos->pieces[D7] == bP && pos->pieces[D6] != EMPTY){
		trapped_eval -= 20;
		if(pos->pieces[C8] == bB) trapped_eval -= 30; 		// Even more if there is still a bishop on c1
	}
	
	if(pos->pieces[E7] == bP && pos->pieces[E6] != EMPTY){
		trapped_eval -= 20;
		if(pos->pieces[F8] == bB) trapped_eval -= 30; 		// Even more if there is still a bishop on f1
	}
	
	
	return trapped_eval;	
}



/**
 * Evaluates the white pawns and adds passed pawn to the array for later analysis.
 * 
 * @param pos The position's pointer.
 * @return pawnEval The evaluation of the pawn strucutre (without passed pawns).
 */
static s8 WhitePawnEval(const S_BOARD *pos){
	
	u8 index,file,rank,testIndex;
	u8 tempWeak;
	s8 pawnEval = 0;
	
	for(u8 i = 0; i < pos->pceNum[wP]; i++){
		
		index = pos->pList[wP][i];
		file = FilesBrd[SQ64(index)];
		rank = RanksBrd[SQ64(index)];
		
		// Check for doubled pawn(detected by the most forward)
		// If the rank recorded is not the same as this pawn which is
		// on the same file, there is another pawn on the same file
		// so the pawns are doubled
		if((wPPos[file+1] & 0xFF) != rank){
			pawnEval -= DOUBLED_PAWN;	
		}
		
		// Check for isolated pawn
		// If the file to the right and left of this file does not contain
		// any friendly pawns, the pawn is isolated
		if(wPPos[file+1+1] == 0 && wPPos[file+1-1] == 0){
			pawnEval -= ISOLATED_PAWN;						
		}
		
		// If it's not isolated it could be backwards/weak
		// There are two scenarios, either the pawn can be
		// left behind (backwards) so it can not advance and be protected by other pawns
		// or it could be pushed too far (weak) so no other pawns can advance and
		// protect it
		else if((WB[SQ64(index)] & PAWN_BIT) == 0){ // If no pawn is protecting it

			tempWeak = TRUE;
			// If the pawn moved atleast two ranks
			if(rank >= RANK_4){
				
				// If the square to two ranks behind to the left is a friendly pawn
				if(!SQOFFBOARD(index - 21) && pos->pieces[(index -21)] == wP){
					
					testIndex = index - 11;
					
					// If the friendly pawn is not blocked by a black pawn and the square
					// it is advancing to is protected by a friendly pawn or not attacked
					// by an enemy pawn
					if(!SQOFFBOARD(testIndex) && pos->pieces[testIndex] != bP && (WB[SQ64(testIndex)] & PAWN_BIT) >= (BB[SQ64(testIndex)] & PAWN_BIT)){
						
						// The pawn can be supported by a friendly pawn advance
						// so it is not weak
						tempWeak = FALSE;							
					}						
				}
				
				// Same as above but checking for friendly pawn to the right
				if(!SQOFFBOARD(index - 19) && pos->pieces[(index -19)] == wP){
					
					testIndex = index - 9;
					if(!SQOFFBOARD(testIndex) && pos->pieces[testIndex] != bP && (WB[SQ64(testIndex)] & PAWN_BIT) >= (BB[SQ64(testIndex)] & PAWN_BIT)){
						
						// The pawn can be supported by a friendly pawn advance
						// so it is not weak
						tempWeak = FALSE;							
					}						
				}
				
				// The pawn can not be supported by a friendly pawn advancing so check
				// if it can advance itself and be supported
				if(tempWeak){
					
					testIndex = index + 10;
					// If the square in front of the pawn is not blocked and the opponent
					// is not controlling the square with it's pawns, it is not backwards
					if(!(pos->pieces[testIndex] != wP && pos->pieces[testIndex] != bP && (WB[SQ64(testIndex)] & PAWN_BIT) >= (BB[SQ64(testIndex)] & PAWN_BIT))){
						pawnEval -= WEAK_PAWN;
					}
					
				}
				
				// Give penalty for the weak/backwards pawn
				// if(tempWeak) pawnEval -= WEAK_PAWN;
			
				
			}
		}
		
		// TODO: Doubled passed pawns are valued too high
		// Check if the pawn is passed	
		if(((wPPos[file+1] & 0xFF00) >> 8) == rank){ 									// Make sure it is the most forward pawn we're checking
		
			if((bPPos[file+1] == 0 || (bPPos[file+1] & 0xFF) < rank) && 				// Either no enemy pawn on same file, or behind the pawn
				(bPPos[file+1+1] == 0 || (bPPos[file+1+1] & 0xFF) <= rank) && 			// Either no enemy pawn to the front right, or next to or behind the pawn
				(bPPos[file+1-1] == 0 || (bPPos[file+1-1] & 0xFF) <= rank))  			// Either no enemy pawn to the front left, or next to or behind the pawn
			{
				passers = passers | FILE_TO_BIT_MASK[file];
			}
		}
	}
	return pawnEval;
	
}


/**
 * Evaluates the black pawns and adds passed pawn to the array for later analysis.
 * 
 * @param pos The position's pointer.
 * @return pawnEval The evaluation of the pawn strucutre (without passed pawns).
 */
static s8 BlackPawnEval(const S_BOARD *pos){
	
	u8 index,file,rank,testIndex;
	u8 tempWeak;
	s8 pawnEval = 0;
	
	for(u8 i = 0; i < pos->pceNum[bP]; i++){
		index = pos->pList[bP][i];
		file = FilesBrd[SQ64(index)];
		rank = RanksBrd[SQ64(index)];
		
		// Check for doubled pawn
		// If the rank recorded is not the same as this pawn which is
		// on the same file, there is another pawn on the same file
		// so the pawns are doubled
		if((bPPos[file+1] & 0xFF) != rank){
			pawnEval -= DOUBLED_PAWN;
		}
		
		// Check for isolated pawn
		// If the file to the right and left of this file does not contain
		// any friendly pawns, the pawn is isolated
		if(bPPos[file+1+1] == 0 && bPPos[file+1-1] == 0){
			pawnEval -= ISOLATED_PAWN;						
		}
		
		// If it's not isolated it could be backwards/weak
		// There are two scenarios, either the pawn can be
		// left behind (backwards) so it can not advance and be protected by other pawns
		// or it could be pushed too far (weak) so no other pawns can advance and
		// protect it
		else if((BB[SQ64(index)] & PAWN_BIT) == 0){ // If no pawn is protecting it
		
			tempWeak = TRUE;
			
			// If the pawn moved atleast two ranks
			if(rank <= RANK_5){
				
				// If the square to two ranks behind to the left is a friendly pawn
				if(!SQOFFBOARD(index + 21) && pos->pieces[(index + 21)] == bP){
					
					testIndex = index + 11;
					// If the friendly pawn is not blocked by a black pawn and the square
					// it is advancing to is protected by a friendly pawn or not attacked
					// by an enemy pawn
					if(!SQOFFBOARD(testIndex) && pos->pieces[testIndex] != wP && (BB[SQ64(testIndex)] & PAWN_BIT) >= (WB[SQ64(testIndex)] & PAWN_BIT)){
						
						// The pawn can be supported by a friendly pawn advance
						// so it is not weak
						tempWeak = FALSE;							
					}						
				}
				
				// Same as above but checking for friendly pawn to the right
				if(!SQOFFBOARD(index + 19) && pos->pieces[(index +19)] == bP){
				
					testIndex = index + 9;
					
					if(!SQOFFBOARD(testIndex) && pos->pieces[testIndex] != wP && (BB[SQ64(testIndex)] & PAWN_BIT) >= (WB[SQ64(testIndex)] & PAWN_BIT)){
					
						// The pawn can be supported by a friendly pawn advance
						// so it is not weak
						tempWeak = FALSE;							
					}						
				}
				
				// The pawn can not be supported by a friendly pawn advancing so check
				// if it can advance itself and be supported
				if(tempWeak){
					
					testIndex = index - 10;
					// If the square in front of the pawn is not blocked and the opponent
					// is not controlling the square with it's pawns, it is not backwards
					if(!(pos->pieces[testIndex] != wP && pos->pieces[testIndex] != bP && (BB[SQ64(testIndex)] & PAWN_BIT) >= (WB[SQ64(testIndex)] & PAWN_BIT))){
						pawnEval -= WEAK_PAWN;
					}
					
				}
				
				// Give penalty for the weak/backwards pawn
				// if(tempWeak) pawnEval -= WEAK_PAWN;
			}
		}
		
		if(((bPPos[file+1] & 0xFF00) >> 8) == rank){ 									// Make sure it is the most forward pawn we're checking
		
			// Check if the pawn is passed	(see white for comments)		
			if((wPPos[file+1] == 0 || (wPPos[file+1] & 0xFF) > rank) && 				// Either no enemy pawn on same rank, or behind the pawn
				(wPPos[file+1+1] == 0 || (wPPos[file+1+1] & 0xFF) >= rank) && 			// Either no enemy pawn to the right, or next to or behind the pawn
				(wPPos[file+1-1] == 0 || (wPPos[file+1-1] & 0xFF) >= rank))   			// Either no enemy pawn to the left, or next to or behind the pawn
			{
				passers = passers | (FILE_TO_BIT_MASK[file] << 8);				
			}
		}			
	}
	return pawnEval;
	
}

/**
 * Evaluates the passsed pawns.
 * 
 * @param pos The position's pointer.
 * @return passerEval The evaluation of the passed pawns.
 */
static s16 EvaluatePassers(const S_BOARD *pos){
	s16 passerEval = 0;
	u8 whitePassers = passers & 0xFF;
	u8 blackPassers = (passers & 0xFF00) >> 8;
	
	u8 index, file, rank;
	u8 rankBonus;
	s16 whiteEval = 0;
	s16 blackEval = 0;
	u8 promDist;
	
	// White
	while(whitePassers != 0){
		
		file = FIRST_BIT_TO_FILE_MASK[whitePassers];
		rank = (wPPos[file+1] & 0xFF00) >> 8;
		index = FR2SQ(file, rank);
		
		rankBonus = (gamePhase >= PHASE_ENDING) ? PASSED_RANK_BONUS[rank] : PASSED_RANK_BONUS[rank] / 2;
		
		whiteEval += rankBonus;
		
		if((wPPos[file+1] & 0xFF) != rank){
			whiteEval += (gamePhase >= PHASE_ENDING) ? PASSED_RANK_BONUS[wPPos[file+1] & 0xFF] : PASSED_RANK_BONUS[wPPos[file+1] & 0xFF] / 2;
		}
		
		// The passer is blocked so remove half the bonus
		if(pos->pieces[index + 10] != EMPTY){
			whiteEval -= rankBonus / 2;
		}
		
		// Protected passer
		if((WB[SQ64(index)] & PAWN_BIT) != 0){
			whiteEval += rankBonus / 2;
		}
		
		// Check how far the passed pawn is from the enemy king
		// the farther the better, and more is given if it is an ending
		if(gamePhase <= PHASE_MIDDLE){
			whiteEval += (rankBonus * abs(FilesBrd[SQ64(pos->kingSq[BLACK])] - file)) / 14;
		}
		else{ // Ending
			whiteEval += (rankBonus * abs(FilesBrd[SQ64(pos->kingSq[BLACK])] - file)) / 10;					
		}
		
		// If no pieces left (i.e. pawn ending) check if the opponent
		// king can catch the passer, do not do this if this is the least advanced of a doubled passer
		if(gamePhase == PHASE_PAWN_ENDING){
			
			promDist = 7 - rank; // Moves to promotion
			
			// If the opponent is to move we add one square
			// representing the black king moving towards the pawn
			// I.e. the pawn needs to be one square closer to promotion
			// to not be caught
			if(pos->side == BLACK) promDist++;
			
			// If the own king is on the same file, and in front of the pawn
			// we need to add one move to remove the king
			if(FilesBrd[SQ64(pos->kingSq[WHITE])] == file && RanksBrd[SQ64(pos->kingSq[WHITE])] > rank) promDist++;
			
			// If the pawn is on the original square it can move two squares
			// so remove one square from the distance
			if(rank == RANK_2) promDist--;
			
			// Compare the number of moves it takes for the passer to promote
			// with the number of moves it takes for opponent king to
			// reach the promotion square
			if(promDist < MAX(abs(RanksBrd[SQ64(pos->kingSq[BLACK])] - rank), abs(FilesBrd[SQ64(pos->kingSq[BLACK])] - file))){
				w_bestPromDist = MIN(w_bestPromDist, promDist);
			}
			
		}
		
		whitePassers ^= FILE_TO_BIT_MASK[file];
	}
	
	// Black
	while(blackPassers != 0){
		
		file = FIRST_BIT_TO_FILE_MASK[blackPassers];
		rank = (bPPos[file+1] & 0xFF00) >> 8;
		index = FR2SQ(file, rank);
		
		rankBonus = (gamePhase >= PHASE_ENDING) ? PASSED_RANK_BONUS[7 - rank] : PASSED_RANK_BONUS[7 - rank] / 2;
		
		blackEval += rankBonus;
		
		if((bPPos[file+1] & 0xFF) != rank){
			blackEval += (gamePhase >= PHASE_ENDING) ? PASSED_RANK_BONUS[7 - (bPPos[file+1] & 0xFF)] : PASSED_RANK_BONUS[7 - (bPPos[file+1] & 0xFF)] / 2;
		}
		
		if(pos->pieces[index - 10] != EMPTY){
			blackEval -= rankBonus / 2;
		}
		
		if((BB[SQ64(index)] & PAWN_BIT) != 0){
			blackEval += rankBonus / 2;
		}
		
		if(gamePhase <= PHASE_MIDDLE){
			blackEval += (rankBonus * abs(FilesBrd[SQ64(pos->kingSq[WHITE])] - file)) / 14;
		}
		else{
			blackEval += (rankBonus * abs(FilesBrd[SQ64(pos->kingSq[WHITE])] - file)) / 10;					
		}
		
		
		if(gamePhase == PHASE_PAWN_ENDING){
			
			promDist = rank; 
			
			if(pos->side == WHITE) promDist++;
			if(FilesBrd[SQ64(pos->kingSq[BLACK])] == file && RanksBrd[SQ64(pos->kingSq[BLACK])] < rank) promDist++;
			if(rank == RANK_7) promDist--;
			
			if(promDist < MAX(abs(RanksBrd[SQ64(pos->kingSq[WHITE])] - rank), abs(FilesBrd[SQ64(pos->kingSq[WHITE])] - file))){
				
				b_bestPromDist = MIN(b_bestPromDist, promDist);
			}
		}
		
		blackPassers ^= FILE_TO_BIT_MASK[file];
	}
	
	passerEval = whiteEval - blackEval;
	
	return passerEval;
}

/**
 * Fills the WB array with attacks from the white knight and
 * calculates and returns the mobility of the piece.
 * 
 * @param pos The position's pointer the white knight is in.
 * @param square The square it is on.
 * @return mobility_total The total mobility value of the piece.
 */
static s8 GenerateAttackWhiteKnight(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == wN);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s8 mobility_total = 0;
	u8 attackedSquare;
	
	// Loop through the 8 different deltas
	for(u8 i = 0; i < 8; i++){
		
		attackedSquare = square + KnDir[i];
		if(!SQOFFBOARD(attackedSquare)){
		
			// Add the attack
			WB[SQ64(attackedSquare)] |= MINOR_BIT;
			WB[SQ64(attackedSquare)]++;
			
			// If square is empty add mobility
			if(pos->pieces[attackedSquare] == EMPTY){
			
				mobility_all++;
				if((BB[SQ64(attackedSquare)] & PAWN_BIT) == 0){
					mobility_safe++;
				}
			}
		}				
	}
	
	// The total mobility is 2 times the safe mobility plus the unsafe mobility
	mobility_total = (2 * mobility_safe + mobility_all);
	
	// If the piece only can move to one safe square it's mobility is so restricted
	// that it is likely to be trapped so penalize this
	if(mobility_safe == 1){
		
		// A 'trapped' piece further up on the board is worse than closer to home
		// since it risks being captured further up
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5) /2;
	}
	
	// If the piece have no safe squares it is just as good as trapped so penalize
	// this even harder
	else if(mobility_safe == 0){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
}

/**
 * Fills the BB array with attacks from the black knight and
 * calculates and returns the mobility of the piece.
 * 
 * @param pos The position's pointer the black knight is in.
 * @param square The square it is on
 * @return mobility_total The total mobility value of the piece
 */
static s8 GenerateAttackBlackKnight(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == bN);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s8 mobility_total = 0;
	u8 attackedSquare;
	
	for(u8 i = 0; i < 8; i++){
	
		attackedSquare = square + KnDir[i];
		if(!SQOFFBOARD(attackedSquare)){
		
			BB[SQ64(attackedSquare)] |= MINOR_BIT;
			BB[SQ64(attackedSquare)]++;
			
			if(pos->pieces[(attackedSquare)] == EMPTY){
			
				mobility_all++;
				if((WB[SQ64(attackedSquare)] & PAWN_BIT) == 0){
				
					mobility_safe++;
				}
			}
		}
	}
	
	// The total mobility is 2 times the safe mobility plus the unsafe mobility
	mobility_total = (2 * mobility_safe + mobility_all);
	
	// If the piece only can move to one safe square it's mobility is so restricted
	// that it is likely to be trapped so penalize this
	if(mobility_safe == 1){
	
		// A 'trapped' piece further up on the board is worse than closer to home
		// since it risks being captured further up
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}
	
	// If the piece have no safe squares it is just as good as trapped so penalize
	// this even harder
	else if(mobility_safe == 0){
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
}

/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * It also detects pinned piecess.
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackWhiteBishop(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == wB);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(u8 i = 0; i < 4; i++){
	
		attackedSquare = square + BiDir[i];
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			WB[SQ64(attackedSquare)] |= MINOR_BIT;
			WB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((BB[SQ64(attackedSquare)] & PAWN_BIT) == 0) mobility_safe++;
			
			attackedSquare += BiDir[i];
		}
		
		// We exited the loop so check if we are still on the board
		// if we are we ran into a piece and can add the final attack
		if(!SQOFFBOARD(attackedSquare)){
		
			WB[SQ64(attackedSquare)] |= MINOR_BIT;
			WB[SQ64(attackedSquare)]++;
			
			
			attackedPiece = pos->pieces[attackedSquare];
			
			// X-ray attack. If we ran into an own queen we keep checking squares behind it
			// and add attacks to all empty squares. However we don't add mobility or attacks
			// on opponent pieces since the piece can not actually reach there yet.
			if(attackedPiece == wQ){
			
				attackedSquare += BiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					WB[SQ64(attackedSquare)] |= MINOR_BIT;
					WB[SQ64(attackedSquare)]++;
					attackedSquare += BiDir[i];
				}					
			}
			
			// If the attacked piece is a enemy knight, rook or queen
			// these are the type of pieces that can be pinned by a bishop
			else if(attackedPiece == bN || attackedPiece == bR || attackedPiece == bQ){
			
				// Keep on going and see if we run in to the enemy king
				// If we do the piece is pinned
				attackedSquare += BiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					attackedSquare += BiDir[i];
				}
				
				if(!SQOFFBOARD(attackedSquare)){
				
					attackedPiece = pos->pieces[attackedSquare];
					// If the attacked piece is a king queen or rook (less value than the bishop)
					// the previous attacked piece is pinned
					if(attackedPiece == bK || attackedPiece == bQ || attackedPiece == bR)	{				
					
						// Since this is the bishop pinning, we add a bonus to mobility
						// which translate to negative for the side with the pinned piece
						mobility_total += PINNED_PIECE;							
					}
				}	
			}
		}
	}
	
	// The total mobility is 2 times the safe mobility plus the unsafe mobility
	mobility_total += (2 * mobility_safe + mobility_all);
	
	// If the piece only can move to one safe square it's mobility is so restricted
	// that it is likely to be trapped so penalize this
	if(mobility_safe == 1){
	
		// A 'trapped' piece further up on the board is worse than closer to home
		// since it risks being captured further up
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}
	
	// If the piece have no safe squares it is just as good as trapped so penalize
	// this even harder
	else if(mobility_safe == 0){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
	
}


/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * It also detects pinned pieces.
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackBlackBishop(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == bB);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(u8 i = 0; i < 4; i++){
	
		attackedSquare = square + BiDir[i];
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			BB[SQ64(attackedSquare)] |= MINOR_BIT;
			BB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((WB[SQ64(attackedSquare)] & PAWN_BIT) == 0) mobility_safe++;
			
			attackedSquare += BiDir[i];
		}
		
		// We exited the loop so check if we are still on the board
		// if we are we ran into a piece and can add the final attack
		if(!SQOFFBOARD(attackedSquare)){
		
			BB[SQ64(attackedSquare)] |= MINOR_BIT;
			BB[SQ64(attackedSquare)]++;
			

			attackedPiece = pos->pieces[attackedSquare];
			
			// X-ray attack
			if(attackedPiece == bQ){
			
				attackedSquare += BiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					BB[SQ64(attackedSquare)] |= MINOR_BIT;
					BB[SQ64(attackedSquare)]++;
					attackedSquare += BiDir[i];
				}
			}
			
			// SEE if the attacked piece is a enemy knight, rook or queen
			// these are the type of pieces that can be pinned by a bishop				
			else if(attackedPiece == wN || attackedPiece == wR || attackedPiece == wQ){
			
				// Keep on going and see if we run in to the enemy king
				// If we do the piece is pinned
				attackedSquare += BiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
					attackedSquare += BiDir[i];
				}
				
				if(!SQOFFBOARD(attackedSquare)){
				
					attackedPiece = pos->pieces[attackedSquare];
					// If the attacked piece is a king, queen or rook (less value than the bishop)
					// the previous attacked piece is pinned
					if(attackedPiece == wK || attackedPiece == wQ || attackedPiece == wR){				
					
						// Since this is the bishop pinning, we add a bonus to mobility
						// which translate to negative for the side with the pinned piece
						mobility_total += PINNED_PIECE;							
					}
				}	
			}
		}
	}
	
	// The total mobility is 2 times the safe mobility plus the unsafe mobility
	mobility_total += (2 * mobility_safe + mobility_all);
	
	// If the piece only can move to one safe square it's mobility is so restricted
	// that it is likely to be trapped so penalize this
	if(mobility_safe == 1){
	
		// A 'trapped' piece further up on the board is worse than closer to home
		// since it risks being captured further up
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}
	
	// If the piece have no safe squares it is just as good as trapped so penalize
	// this even harder
	else if(mobility_safe == 0){
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
	
}

/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * It also detects pinned piecess.
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackWhiteRook(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == wR);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(u8 i = 0; i < 4; i++){
	
		attackedSquare = square + RkDir[i];
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			WB[SQ64(attackedSquare)] |= ROOK_BIT;
			WB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((BB[SQ64(attackedSquare)] & (PAWN_BIT | MINOR_BIT)) == 0) mobility_safe++;
			
			attackedSquare += RkDir[i];
		}
		
		if(!SQOFFBOARD(attackedSquare)){
		
			WB[SQ64(attackedSquare)] |= ROOK_BIT;
			WB[SQ64(attackedSquare)]++;
			
			attackedPiece = pos->pieces[attackedSquare];
			
			if(attackedPiece == wR || attackedPiece == wQ){
			
				attackedSquare += RkDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					WB[SQ64(attackedSquare)] |= ROOK_BIT;
					WB[SQ64(attackedSquare)]++;
					attackedSquare += RkDir[i];
				}
				
			}else if(attackedPiece == bN || attackedPiece == bB || attackedPiece == bQ){
				
				attackedSquare += RkDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
					attackedSquare += RkDir[i];
				}
				
				if(!SQOFFBOARD(attackedSquare)){
				
					attackedPiece = pos->pieces[attackedSquare];
					if(attackedPiece == bK || attackedPiece == bQ) mobility_total += PINNED_PIECE;							
					
				}	
			}
		}
	}
	
	mobility_total += (2 * mobility_safe + mobility_all);
	
	if(mobility_safe == 1){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}else if(mobility_safe == 0){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1)* 5);
	}
	
	return mobility_total;
	
}

/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * It also detects pinned piecess.
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackBlackRook(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == bR);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(s32 i = 0; i < 4; i++){
		
		attackedSquare = square + RkDir[i];
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			BB[SQ64(attackedSquare)] |= ROOK_BIT;
			BB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((WB[SQ64(attackedSquare)] & (PAWN_BIT | MINOR_BIT)) == 0) mobility_safe++;
			
			attackedSquare += RkDir[i];
		}
		
		if(!SQOFFBOARD(attackedSquare)){
		
			BB[SQ64(attackedSquare)] |= ROOK_BIT;
			BB[SQ64(attackedSquare)]++;
			
			attackedPiece = pos->pieces[attackedSquare];
			
			if(attackedPiece == bR || attackedPiece == bQ){
			
				attackedSquare += RkDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					BB[SQ64(attackedSquare)] |= ROOK_BIT;
					BB[SQ64(attackedSquare)]++;	
					attackedSquare += RkDir[i];
				}
				
			}else if(attackedPiece == wN || attackedPiece == wB || attackedPiece == wQ){
				
				attackedSquare += RkDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
					attackedSquare += RkDir[i];
				}
				
				if(!SQOFFBOARD(attackedSquare)){
				
					attackedPiece = pos->pieces[attackedSquare];
					if(attackedPiece == wK || attackedPiece == wQ) mobility_total += PINNED_PIECE;							
					
				}	
			}
		}
	}
	
	mobility_total += (2 * mobility_safe + mobility_all);
	
	if(mobility_safe == 1){
		mobility_total -= (( 7 - RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}
	else if(mobility_safe == 0){
		mobility_total -= (( 7 - RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
	
}

/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * No detection for pinned pieces for queens (only thing they can pin against is
 * king and this should be quite uncommon, temporary and not very dangerous).
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackWhiteQueen(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == wQ);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(u8 i = 0; i < 8; i++){
		
		attackedSquare = square + KiDir[i];    // Queen has the same deltals of King but it is a sliding piece
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			WB[SQ64(attackedSquare)] |= QUEEN_BIT;
			WB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((BB[SQ64(attackedSquare)] & (PAWN_BIT | MINOR_BIT | ROOK_BIT)) == 0 ){
			
				mobility_safe++;
			}	
			
			attackedSquare += KiDir[i];
		}
		
		if(!SQOFFBOARD(attackedSquare)){
		
			WB[SQ64(attackedSquare)] |= QUEEN_BIT;
			WB[SQ64(attackedSquare)]++;
			
			attackedPiece = pos->pieces[attackedSquare];
			
			if(attackedPiece == wQ || (attackedPiece == wR && i <= 3) || (attackedPiece == wB && i >= 4)){
			
				attackedSquare += KiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					WB[SQ64(attackedSquare)] |= QUEEN_BIT;
					WB[SQ64(attackedSquare)]++;
					attackedSquare += KiDir[i];
				}
			}
		}
	}
	
	mobility_total += (2 * mobility_safe + mobility_all);
	
	if(mobility_safe == 1){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}
	else if(mobility_safe == 0){
		mobility_total -= ((RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
	
}

/**
 * Works same as for knights but check all squares in the direction of the delta.
 * 
 * No detection for pinned pieces for queens (only thing they can pin against is
 * king and this should be quite uncommon, temporary and not very dangerous).
 * 
 * @param pos The position's pointer the piece is in.
 * @param square The square the piece is on.
 * @return mobility_total The total mobility score of the piece.
 */
static s16 GenerateAttackBlackQueen(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == bQ);
	
	u8 mobility_all = 0;
	u8 mobility_safe = 0;
	s16 mobility_total = 0;
	u8 attackedSquare;
	u8 attackedPiece;
	
	for(u8 i = 0; i < 8; i++){
	
		attackedSquare = square + KiDir[i];  // Queen has the same deltals of King but it is a sliding piece
		while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
		
			BB[SQ64(attackedSquare)] |= QUEEN_BIT;
			BB[SQ64(attackedSquare)]++;
			mobility_all++;
			
			if((WB[SQ64(attackedSquare)] & (PAWN_BIT | MINOR_BIT | ROOK_BIT)) == 0){
				mobility_safe++;
			}	
			
			attackedSquare += KiDir[i];
		}
		
		if(!SQOFFBOARD(attackedSquare)){
		
			BB[SQ64(attackedSquare)] |= QUEEN_BIT;
			BB[SQ64(attackedSquare)]++;
			
			attackedPiece = pos->pieces[attackedSquare];
			
			// if(attackedPiece == bR || attackedPiece == bQ)
			if(attackedPiece == bQ || (attackedPiece == bB && i >= 4) || (attackedPiece == bR && i <= 3)){
			
				attackedSquare += KiDir[i];
				while(!SQOFFBOARD(attackedSquare) && pos->pieces[attackedSquare] == EMPTY){
				
					BB[SQ64(attackedSquare)] |= QUEEN_BIT;
					BB[SQ64(attackedSquare)]++;
					attackedSquare += KiDir[i];
				}
			}
		}
	}
	
	mobility_total += (2 * mobility_safe + mobility_all);
	
	if(mobility_safe == 1){
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5) / 2;
	}else if(mobility_safe == 0){
		mobility_total -= ((7 - RanksBrd[SQ64(square)] + 1) * 5);
	}
	
	return mobility_total;
	
}

/**
 * Fills the WB array with attacks from the white king.
 * 
 * @param pos The position's pointer the white king is in.
 * @param square The square it is on.
 * @return mobility_total The total mobility value of the piece.
 */
static void GenerateAttackWhiteKing(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == wK);
	
	u8 attackedSquare;
	
	// Loop through the 8 different deltas
	for(u8 i = 0; i < 8; i++){
	
		attackedSquare = square + KiDir[i];
		if(!SQOFFBOARD(attackedSquare)){
		
			// Add the attack
			WB[SQ64(attackedSquare)] |= KING_BIT;
			WB[SQ64(attackedSquare)]++;
		}				
	}
}

/**
 * Fills the BB array with attacks from the black king.
 * 
 * @param pos The position's pointer the black king is in.
 * @param square The square it is on.
 * @return mobility_total The total mobility value of the piece.
 */
static void GenerateAttackBlackKing(const S_BOARD *pos, u8 square){
	
	ASSERT(SqIs120(square));
	ASSERT(SqOnBoard(square));
	ASSERT(pos->pieces[square] == bK);
	
	u8 attackedSquare;
	
	// Loop through the 8 different deltas
	for(u8 i = 0; i < 8; i++){
	
		attackedSquare = square + KiDir[i];
		if(!SQOFFBOARD(attackedSquare)){
		
			// Add the attack
			BB[SQ64(attackedSquare)] |= KING_BIT;
			BB[SQ64(attackedSquare)]++;
		}				
	}
}

/**
 * Counts the pieces attacking the squares around the white king.
 * 
 * @param pos The position's pointer to check.
 * @return totalAttack The total value from the attacking pieces (is negated before returned so it can be added to kingSafety).
 */
static s16 WhiteKingAttack(const S_BOARD *pos){

	s16 totalAttack = 0;
	u8 attackedCount = 0;
	u8 flag = 0;
	u8 kingIndex = pos->kingSq[WHITE];
	u8 attackedIndex;
	
	ASSERT(SqIs120(kingIndex));
	ASSERT(SqOnBoard(kingIndex));
	ASSERT(pos->pieces[kingIndex] == wK);

	// Inital attack count depending on where the king is located is not needed
	// in MCHE since this is handled by the piecetables
	
	// Start with squares two squares in front of the king
	// Here we only gather what type of pieces is attacking, we
	// do not increase the attackers count
	attackedIndex = kingIndex + 19;
	if(!SQOFFBOARD(attackedIndex)) flag |= BB[SQ64(attackedIndex)];
	
	attackedIndex = kingIndex + 20;
	if(!SQOFFBOARD(attackedIndex)) flag |= BB[SQ64(attackedIndex)];
	
	attackedIndex = kingIndex + 21;
	if(!SQOFFBOARD(attackedIndex)) flag |= BB[SQ64(attackedIndex)];
	
	
	// Now we check the squares to the left, right and behind the king
	// Here we increase the attackedCount for every square that is attacked
	// and one more if it is also only protected by the own king
	attackedIndex = kingIndex + 1;	// Right
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 1;	// Left
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 9;	// Left behind
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 10;	// Behind
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 11;	// Right behind
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	// Now we do the squares just in front of the king, it works
	// the same as before but we also add one count if no own piece is placed there
	attackedIndex = kingIndex + 9;	// Left front
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == BLACK) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex + 10;	// Front
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == BLACK) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex + 11;	// Right front
	if(!SQOFFBOARD(attackedIndex) && BB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= BB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == BLACK) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(WB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	// We now have all the information about the pieces attacking the squares
	// around the king so time to evaluate
	
	// We get the attack pattern by shifting three steps in 'flag' so we
	// only have the attacking pieces pattern left, then use the attackers mask
	// to mask out anything 'left' of the piece bits 
	// 
	// The king_attack_pattern is built so the number we get from a certain set of pieces
	//(the pattern) gives us an extra count, some combinations of pieces are
	// more dangerous than others
	attackedCount += KING_ATTACK_PATTERN[((flag >> 3) & ATTACKERS_MASK)]; 
	
	// Now we have the attacked count and can simply get the value of the
	// attack from the KING_ATTACK_EVAL table
	
	totalAttack = KING_ATTACK_EVAL[attackedCount];
	
	
	return -totalAttack;
}

/**
 * Counts the pieces attacking the squares around the black king.
 * 
 * @param pos The position's pointer to check.
 * @return totalAttack The total value from the attacking pieces (is negated before returned so it can be added to kingSafety).
 */
static s16 BlackKingAttack(const S_BOARD *pos){
	
	
	s16 totalAttack = 0;
	u8 attackedCount = 0;
	u8 flag = 0;
	u8 kingIndex = pos->kingSq[BLACK];
	u8 attackedIndex;
	
	ASSERT(SqIs120(kingIndex));
	ASSERT(SqOnBoard(kingIndex));
	ASSERT(pos->pieces[kingIndex] == bK);

	// Inital attack count depending on where the king is located is not needed
	// in MCHE since this is handled by the piecetables
	
	// Start with squares two squares in front of the king
	// Here we only gather what type of pieces is attacking, we
	// do not increase the attackers count
	attackedIndex = kingIndex - 19;
	if(!SQOFFBOARD(attackedIndex)) flag |= WB[SQ64(attackedIndex)];
	
	
	attackedIndex = kingIndex - 20;
	if(!SQOFFBOARD(attackedIndex)) flag |= WB[SQ64(attackedIndex)];
	
	
	attackedIndex = kingIndex - 21;
	if(!SQOFFBOARD(attackedIndex)) flag |= WB[SQ64(attackedIndex)];
	
	
	// Now we check the squares to the left, right and behind the king
	// Here we increase the attackedCount for every square that is attacked
	// and one more if it is also only protected by the own king
	attackedIndex = kingIndex + 1;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 1;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex + 9;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex + 10;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex + 11;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	// Now we do the squares just in front of the king, it works
	// the same as before but we also add one count if no own piece is placed there
	attackedIndex = kingIndex - 9;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == WHITE) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 10;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == WHITE) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedIndex = kingIndex - 11;
	if(!SQOFFBOARD(attackedIndex) && WB[SQ64(attackedIndex)] != 0){
	
		attackedCount++;
		flag |= WB[SQ64(attackedIndex)];
		if(pos->pieces[attackedIndex] == EMPTY || PieceCol[pos->pieces[attackedIndex]] == WHITE) attackedCount++; 	// Empty square or enemy piece just in front of the king
		if(BB[SQ64(attackedIndex)] == 129) attackedCount++; 	// 129 translates to 10000001 in binary form, meaning only one attacker, and that attacker is the king 
	}
	
	attackedCount += KING_ATTACK_PATTERN[((flag >> 3) & ATTACKERS_MASK)];
	totalAttack = KING_ATTACK_EVAL[attackedCount];
	
	
	return -totalAttack;
}

/**
 * Calculates things like pawn cover and pawn storms to evaluate the protection
 * of the white king.
 * 
 * @param pos The position's pointer we're examining.
 * @return totalDefense The total defense value of the white king.
 */
static s16 WhiteKingDefense(const S_BOARD *pos){

	s16 totalDefense = 0;
	u8 pawnRank;
	u8 king_index = pos->kingSq[WHITE];
	u8 king_file  = FilesBrd[SQ64(king_index)];
	u8 oking_file = FilesBrd[SQ64(pos->kingSq[BLACK])]; // Opponent's king

	// White pawn shield

	// Find the rank the pawn in front of the king is on
	pawnRank = (wPPos[king_file+1] & 0xFF);
	
	// If there is no pawn in front of the king, penalize with 36
	if(pawnRank == 0) totalDefense -= 36;
	
	// If there is a pawn in front of the king penalize with the number
	// of ranks it has advanced
	// e.g. advanced 2 ranks (on rank 4 (=3 on tboard))
	// 36 - (7-3)*(7-3) = 20 penalty 
	else totalDefense -= 36 - ((7 - pawnRank) * (7 - pawnRank));
	
	
	// Look for the pawn to the right in front of the king, make sure
	// we are not off board and do the same thing as above
	if(king_file < FILE_H){
	
		pawnRank = (wPPos[king_file+1+1] & 0xFF);
		if(pawnRank == 0) totalDefense -= 36;
		else totalDefense -= 36 - ((7 - pawnRank) * (7 - pawnRank));
		
	}
	
	if(king_file > FILE_A){
		
		pawnRank = (wPPos[king_file] & 0xFF);
		if(pawnRank == 0) totalDefense -= 36;
		else totalDefense -= 36 - ((7 - pawnRank) * (7 - pawnRank));
	}
	
	// Encourage proper placement of the bishop in fianchetto pawn structure
	
	// King on queenside
	if(king_file < FILE_D){
	
		// If the FILE_B pawn is on rank 3 and there is a bishop on B2, award
		// points for proper fianchetto
		if((wPPos[2] & 0xFF) == RANK_3 && pos->pieces[B2] == wB) totalDefense += 20;
		
	}else if(king_file > FILE_E){ 							// King on kingside
	
		// If the FILE_G pawn is on rank 3 and there is a bishop on G2, award
		// points for proper fianchetto
		if((wPPos[7] & 0xFF) == RANK_3 && pos->pieces[G2] == wB) totalDefense += 20;
	}
	
	
	// If the king is still on the original square, the F2 square is weak so
	// the pawn should not be moved
	if((king_index == E1 && pos->pieces[F2] != wP) || (king_index == D1 && pos->pieces[C2] != wP)) totalDefense -= 10;
	
	// If the kings are castled in different directions a pawn storm is benefitial
	// so reduce the defense score if the opponent has advanced his pawns on that side
	if(abs(king_file - oking_file) > 2){
	
		// Since we're using "7-" here we need to make sure there's actually a pawn there
		//(there was a bug where this check wasn't done and a non-existing pawn would give 5*7-0=35 instead of the correct 0)
		totalDefense -= 5 * ((((bPPos[king_file+1] & 0xFF00) >> 8)   == 0)? 0 : 7 - ((bPPos[king_file+1] & 0xFF00) >> 8));
		totalDefense -= 5 * ((((bPPos[king_file+1+1] & 0xFF00) >> 8) == 0)? 0 : 7 - ((bPPos[king_file+1+1] & 0xFF00) >> 8));
		totalDefense -= 5 * ((((bPPos[king_file] & 0xFF00) >> 8)     == 0)? 0 : 7 - ((bPPos[king_file] & 0xFF00) >> 8));
	}

	
	return totalDefense;
	
}

/**
 * Calculates things like pawn cover and pawn storms to evaluate the protection
 * of the black king.
 * 
 * @param pos The position's pointer we're examining.
 * @return totalDefense The total defense value of the black king.
 */
static s16 BlackKingDefense(const S_BOARD *pos){

	s16 totalDefense = 0;
	u8 pawnRank;
	u8 king_index = pos->kingSq[BLACK];
	u8 king_file  = FilesBrd[SQ64(king_index)];
	u8 oking_file = FilesBrd[SQ64(pos->kingSq[WHITE])]; // Opponent's king

	// Black pawn shield

	// Find the rank the pawn in front of the king is on
	pawnRank = (bPPos[king_file+1] & 0xFF);
	
	// If there is no pawn in front of the king, penalize with 36
	if(pawnRank == 0) totalDefense -= 36;
	
	// If there is a pawn in front of the king penalize with the number
	// of ranks it has advanced
	// e.g. advanced 2 ranks (on rank 5 (=4 on the board))
	// 36 - (4)*(4) = 20 penalty 
	else totalDefense -= 36 - ((pawnRank) * (pawnRank));
	
	// Look for the pawn to the right in front of the king, make sure
	// we are not off the board and do the same thing as above
	if(king_file < FILE_H){
	
		pawnRank = (bPPos[king_file+1+1] & 0xFF);
		if(pawnRank == 0) totalDefense -= 36;
		else totalDefense -= 36 - ((pawnRank) * (pawnRank));
		
	}
	if((king_file) > FILE_A){
	
		pawnRank = (bPPos[king_file] & 0xFF);
		if(pawnRank == 0) totalDefense -= 36;
		else totalDefense -= 36 - ((pawnRank) * (pawnRank));
	}
	
	// Encourage proper placement of the bishop in fianchetto pawn structure
	
	// King on queenside
	if(king_file < FILE_D){
	
		// If the FILE_B pawn is on rank 6 and there is a bishop on B2, award
		// points for proper fianchetto
		if((bPPos[2] & 0xFF) == RANK_6 && pos->pieces[B7] == bB) totalDefense += 20;
		
	}else if(king_file > FILE_E){						// King on kingside
	
		// If the FILE_G pawn is on rank 6 and there is a bishop on G2, award
		// points for proper fianchetto
		if((bPPos[7] & 0xFF) == RANK_6 && pos->pieces[G7] == bB) totalDefense += 20;
	}
	
	// If the king is still on the original square, the F2 square is weak so
	// the pawn should not be moved
	if((king_index == E8 && pos->pieces[F7] != bP) || (king_index == D8 && pos->pieces[C7] != bP)) totalDefense -= 10;
	
	// If the kings are castled in different directions a pawn storm is benefitial
	// so reduce the defense score if the opponent has advanced his pawns on that side
	if(abs(king_file - oking_file) > 2){
	
		totalDefense -= 5 * (((wPPos[king_file+1] & 0xFF00) >> 8) + ((wPPos[king_file+1+1] & 0xFF00) >> 8) + ((wPPos[king_file] & 0xFF00) >> 8));
	}

	
	return totalDefense;
	
}

/**
 * Determines if two or more pieces are 'hanging' in the position for white,
 * hanging means either that the piece is not defended or attacked
 * by a lesser valued piece (rook attacked by a pawn for example).
 * 
 * @param pos The position's pointer.
 * @return hungPiecePenalty.
 */
static s16 WhiteHungPiece(const S_BOARD *pos){

	u8 hungPiecesCount = 0;
	s16 hungPiecePenalty = 0;
	u8 square, i;
	
	// Knights
	for(i = 0; i < pos->pceNum[wN]; i++){
	
		square = pos->pList[wN][i];
		if((BB[SQ64(square)] > 0 && WB[SQ64(square)] == 0) || (BB[SQ64(square)] & PAWN_BIT) > 0) hungPiecesCount++; 
	}
	
	// Bishops
	for(i = 0; i < pos->pceNum[wB]; i++){
	
		square = pos->pList[wB][i];
		if((BB[SQ64(square)] > 0 && WB[SQ64(square)] == 0) || (BB[SQ64(square)] & PAWN_BIT) > 0) hungPiecesCount++; 
	}
	
	// Rooks
	for(i = 0; i < pos->pceNum[wR]; i++){
	
		square = pos->pList[wR][i];
		if((BB[SQ64(square)] > 0 && WB[SQ64(square)] == 0) || (BB[SQ64(square)] & PAWN_BIT) > 0 || (BB[SQ64(square)] & MINOR_BIT) > 0) hungPiecesCount++; 
	}
	
	// Queens
	for(i = 0; i < pos->pceNum[wQ]; i++){
	
		square = pos->pList[wQ][i];
		if((BB[SQ64(square)] > 0 && WB[SQ64(square)] == 0) || (BB[SQ64(square)] & PAWN_BIT) > 0 || (BB[SQ64(square)] & MINOR_BIT) > 0 || (BB[SQ64(square)] & ROOK_BIT) > 0) hungPiecesCount++; 
	}
	
	if(hungPiecesCount == 2) hungPiecePenalty -= HUNG_PIECE_PENALTY;
	else if(hungPiecesCount > 2) hungPiecePenalty -= 2*HUNG_PIECE_PENALTY;
	
	return hungPiecePenalty;
}

/**
 * Determines if two or more pieces are 'hanging' in the position for black,
 * hanging means either that the piece is not defended or attacked
 * by a lesser valued piece (rook attacked by a pawn for example).
 * 
 * @param pos The position's pointer.
 * @return hungPiecePenalty.
 */
static s16 BlackHungPiece(const S_BOARD *pos){

	u8 hungPiecesCount = 0;
	s16 hungPiecePenalty = 0;
	u8 square, i;
	
	// Knights
	for(i = 0; i < pos->pceNum[bN]; i++){
	
		square = pos->pList[bN][i];
		if((WB[SQ64(square)] > 0 && BB[SQ64(square)] == 0) || (WB[SQ64(square)] & PAWN_BIT) > 0) hungPiecesCount++; 
	}
	
	// Bishops
	for(i = 0; i < pos->pceNum[bB]; i++){
	
		square = pos->pList[bB][i];
		if((WB[SQ64(square)] > 0 && BB[SQ64(square)] == 0) || (WB[SQ64(square)] & PAWN_BIT) > 0) hungPiecesCount++; 
	}
	
	// Rooks
	for(i = 0; i < pos->pceNum[bR]; i++){
	
		square = pos->pList[bR][i];
		if((WB[SQ64(square)] > 0 && BB[SQ64(square)] == 0) || (WB[SQ64(square)] & PAWN_BIT) > 0 || (WB[SQ64(square)] & MINOR_BIT) > 0) hungPiecesCount++; 
	}
	
	// Queens
	for(i = 0; i < pos->pceNum[bQ]; i++){
	
		square = pos->pList[bQ][i];
		if((WB[SQ64(square)] > 0 && BB[SQ64(square)] == 0) || (WB[SQ64(square)] & PAWN_BIT) > 0 || (WB[SQ64(square)] & MINOR_BIT) > 0 || (WB[SQ64(square)] & ROOK_BIT) > 0) hungPiecesCount++; 
	}
	
	if(hungPiecesCount == 2) hungPiecePenalty -= HUNG_PIECE_PENALTY;
	else if(hungPiecesCount >2) hungPiecePenalty -= 2*HUNG_PIECE_PENALTY;
	
	return hungPiecePenalty;
}

/**
 * Caculate the total evaluation value for a given board position.
 * 
 * @param pos The position's pointer.
 * @return finalEval
 */
s16 EvalPosition(S_BOARD *pos){
	
	if(DrawByMaterial(pos,BOTH)) return 0;
	
	s16 finalEval = 0;
	if(ProbeEval(pos, &finalEval)){
		pos->EvalTable->evalhits++;
		return finalEval;
	}
	
	u8 attackedSquare, i;
	
	for(i = 0; i < 64; i++) WB[i] = BB[i] = 0;
	
	for(i = 0; i < 10; i++) wPPos[i] = bPPos[i] = 0;
	
	passers = 0;
	
	w_bestPromDist = 100; // Initialize to a high value so we can change easily below
	b_bestPromDist = 100;

	s8 w_mobility = 0;
	s8 b_mobility = 0;
	s16 w_material = pos->material[WHITE]; // TODO: Handle material incrementally in makeMove and unmakeMove
	s16 b_material = pos->material[BLACK];
	s8 w_piecePos = 0;
	s8 b_piecePos = 0;
	s8 w_tropism = 0;
	s8 b_tropism = 0;
	s8 tempo = 0;
	
	// Remember if there was a pawn on 2nd/7th rank,
	// used to determine if placing a rook on 7th should be rewarded
	u8 wPawnOnSecond = FALSE;
	u8 bPawnOnSeventh = FALSE;

	u8 index,file,rank;
	
	// Get the phase the game is in
	// TODO: Handle this incrementally in makeMove and unmakeMove
	gamePhase = GetGamePhase(pos);
	
	// Decide if we should use ordinary piece tables or endgame tables for evaluating piece positions
	u8 useEndingTables;
	if(gamePhase <= PHASE_MIDDLE) useEndingTables = FALSE;
	else useEndingTables = TRUE;
	
	// Evaluate trapped pieces
	s16 w_trappedEval = WhiteTrapped(pos);
	s16 b_trappedEval = BlackTrapped(pos);
	
	// Pawns
	for(i = 0; i < pos->pceNum[wP]; i++){
		
		index = pos->pList[wP][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == wP);
		
		// w_material += PAWN_VALUE; 													// Collect value
		if(gamePhase >= PHASE_ENDING) w_material += 20; 								// Pawns are worth a bit extra in the ending
		
		if(!useEndingTables) w_piecePos += wP_POS[SQ64(index)]; 						// Evaluate its position
		else w_piecePos += 0; 															// Position if it is an ending
		
		attackedSquare = index + 11; 													// Record where the pawn attacks
		if(!SQOFFBOARD(attackedSquare)){
			WB[SQ64(attackedSquare)]++;
			WB[SQ64(attackedSquare)] |= PAWN_BIT;			
		}
		
		attackedSquare = index + 9;
		if(!SQOFFBOARD(attackedSquare)){
			WB[SQ64(attackedSquare)]++;
			WB[SQ64(attackedSquare)] |= PAWN_BIT;			
		}
		
		rank = RanksBrd[SQ64(index)];
		file = FilesBrd[SQ64(index)];
		
		// Add it to the pawn array for more pawn evaluation later
		
		// No pawn on this file so far, so add it as both most forward and most backward pawn on the file
		if(wPPos[file + 1] == 0)wPPos[file+1] = 0 | rank | (rank << 8);
		
		// The new pawn is more backward then the old most backward pawn so add it
		else if((wPPos[file+1] & 0xFF) > rank) wPPos[file+1] = (wPPos[file+1] & 0xFF00) | rank;
		
		// The new pawn is more forward than the old most forward pawn so add it
		else if(((wPPos[file+1] & 0xFF00) >> 8) < rank) wPPos[file+1] = (wPPos[file+1] & 0xFF) | (rank << 8);
		
		if(rank == RANK_2) wPawnOnSecond = TRUE; // Atleast one pawn on the second rank so placing a rook/queen there might be worth it 
	}
	
	for(i = 0; i < pos->pceNum[bP]; i++){
		
		index = pos->pList[bP][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == bP);
		
		// b_material += PAWN_VALUE;
		if(gamePhase >= PHASE_ENDING) b_material += 20; // Pawns are worth a bit extra in the ending
		
		if(!useEndingTables) b_piecePos += wP_POS[MIRROR64(SQ64(index))];
		else b_piecePos += 0;
		
		attackedSquare = index - 11;
		if(!SQOFFBOARD(attackedSquare)) {
			BB[SQ64(attackedSquare)]++;
			BB[SQ64(attackedSquare)] |= PAWN_BIT;			
		}
		
		attackedSquare = index - 9;
		if(!SQOFFBOARD(attackedSquare)) {
			BB[SQ64(attackedSquare)]++;
			BB[SQ64(attackedSquare)] |= PAWN_BIT;			
		}
		
		rank = RanksBrd[SQ64(index)];
		file = FilesBrd[SQ64(index)];
		
		if(bPPos[file + 1] == 0) bPPos[file+1] = 0 | rank | (rank << 8); 
		else if((bPPos[file+1] & 0xFF) < rank) bPPos[file+1] = (bPPos[file+1] & 0xFF00) | rank;
		else if(((bPPos[file+1] & 0xFF00) >> 8) > rank) bPPos[file+1] = (bPPos[file+1] & 0xFF) | (rank << 8);
		
		if(rank == RANK_7) bPawnOnSeventh = TRUE; // Atleast one pawn on the seventh rank so placing a rook/queen there might be worth it
	}
	
	// The pawnPos arrays are now filled so we can evaluate the pawns
	
	// SEE if we have any information in the pawn hash
	// Don't probe if no pawns (i.e. pawn zobrist = 0)
	s8 wPStructure = 0;
	s8 bPStructure = 0;
	s16 passerEval = 0;
	
	if(!ProbePawnEval(pos, &wPStructure, &bPStructure, &passers)){
		wPStructure = WhitePawnEval(pos);
		bPStructure = BlackPawnEval(pos);
		StorePawnEval(pos, wPStructure, bPStructure, passers);
	}else{
		pos->EvalTable->pawnhits++;
		ASSERT(wPStructure == WhitePawnEval(pos));
		ASSERT(bPStructure == BlackPawnEval(pos));
	}
	
	passerEval = EvaluatePassers(pos);
	
	// We now know if either side has an unstoppable passer so reward it
	if(w_bestPromDist < b_bestPromDist) passerEval += 600;
	else if(b_bestPromDist < w_bestPromDist) passerEval -= 600;
	
	
	// Knights
	for(i = 0; i < pos->pceNum[wN]; i++){
		
		index = pos->pList[wN][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == wN);
		
		// w_material += KNIGHT_VALUE;
		if(!useEndingTables) w_piecePos += wN_POS[SQ64(index)];
		else w_piecePos += KNIGHT_POS_ENDING[SQ64(index)];
		
		// The knight is placed on one of the outpost squares
		if(wN_OUTPOST[SQ64(index)] != 0){
			
			// If the knight is protected by one pawn award the value in the array,
			// if protected by two pawns award double the value, and award nothing if it
			// is not protected by a pawn
			// Outposts squares arn'nt on the edge of pos The position's pointer so, we don't need to SQOFFBOARD checking
			if(pos->pieces[index - 9] == wP) w_piecePos += wN_OUTPOST[SQ64(index)];
			if(pos->pieces[index - 11] == wP) w_piecePos += wN_OUTPOST[SQ64(index)];
		}
		
		w_mobility += GenerateAttackWhiteKnight(pos, index);
		w_tropism += TROPISM_KNIGHT[Distance(pos->kingSq[BLACK], index)];
	}
	
	for(i = 0; i < pos->pceNum[bN]; i++){
		
		index = pos->pList[bN][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == bN);
		
		// b_material += KNIGHT_VALUE;
		if(!useEndingTables) b_piecePos += wN_POS[MIRROR64(SQ64(index))];
		else b_piecePos += KNIGHT_POS_ENDING[SQ64(index)];
		
		if(wN_OUTPOST[MIRROR64(SQ64(index))] != 0){

			if(pos->pieces[index + 9] == bP) b_piecePos += wN_OUTPOST[MIRROR64(SQ64(index))];
			if(pos->pieces[index + 11] == bP) b_piecePos += wN_OUTPOST[MIRROR64(SQ64(index))];
		}
		
		b_mobility += GenerateAttackBlackKnight(pos, index);
		b_tropism += TROPISM_KNIGHT[Distance(pos->kingSq[WHITE], index)];
	}
	
	
	// Bishops
	for(i = 0; i < pos->pceNum[wB]; i++){
		
		index = pos->pList[wB][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == wB);
		
		// w_material += BISHOP_VALUE;
		if(!useEndingTables) w_piecePos += wB_POS[SQ64(index)];
		else w_piecePos += BISHOP_POS_ENDING[SQ64(index)];
		
		w_mobility += GenerateAttackWhiteBishop(pos, index);
		w_tropism += TROPISM_BISHOP[Distance(pos->kingSq[BLACK], index)];
	}
	
	for(i = 0; i < pos->pceNum[bB]; i++){
		
		index = pos->pList[bB][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == bB);
		
		// b_material += BISHOP_VALUE;
		if(!useEndingTables) b_piecePos += wB_POS[MIRROR64(SQ64(index))];
		else b_piecePos += BISHOP_POS_ENDING[SQ64(index)];
		
		b_mobility += GenerateAttackBlackBishop(pos, index);
		b_tropism += TROPISM_BISHOP[Distance(pos->kingSq[WHITE], index)];
	}
	// Bishop pair bonus
	if(pos->pceNum[wB] >= 2) w_piecePos += BISHOP_PAIR;
	if(pos->pceNum[bB] >= 2) b_piecePos += BISHOP_PAIR;
	
	
	// Rooks
	for(i = 0; i < pos->pceNum[wR]; i++){
		
		index = pos->pList[wR][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == wR);
		
		file = FilesBrd[SQ64(index)];
		rank = RanksBrd[SQ64(index)];
		ASSERT(FileRankValid(file));
		ASSERT(FileRankValid(rank));
		
		// w_material += ROOK_VALUE;
		if(!useEndingTables) w_piecePos += wR_POS[SQ64(index)];
		else w_piecePos += 0;
		
		// Rook on file with only enemy pawns
		if(wPPos[file +1] == 0 && bPPos[file +1] != 0) w_piecePos += ROOK_ON_SEMI;
		// Rook on open file
		else if(wPPos[file +1] == 0) w_piecePos += ROOK_ON_OPEN;
		
		// Rook on seventh rank
		if(rank == RANK_7 && (bPawnOnSeventh || RanksBrd[SQ64(pos->kingSq[BLACK])] == RANK_8)) w_piecePos += ROOK_ON_SEVENTH;
		
		// TODO: Add bonus for rooks behind passed pawns
		if(((passers & 0xFF00) >> 8) & FILE_TO_BIT_MASK[file]){ // If there is a passer in this file 
			if((bPPos[file + 1] & 0xFF) < rank) w_piecePos += ROOK_BEHIND_PASSER; // Add bonus if the rook behined the passer
		}
		
		w_mobility += GenerateAttackWhiteRook(pos, index);
		w_tropism += TROPISM_ROOK[Distance(pos->kingSq[BLACK], index)];
		
	}
	
	for(i = 0; i < pos->pceNum[bR]; i++){
		
		index = pos->pList[bR][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == bR);
		
		file = FilesBrd[SQ64(index)];
		rank = RanksBrd[SQ64(index)];
		ASSERT(FileRankValid(file));
		ASSERT(FileRankValid(rank));
		
		// b_material += ROOK_VALUE;
		if(!useEndingTables) b_piecePos += wR_POS[MIRROR64(SQ64(index))];
		else b_piecePos += 0;
		
		if(bPPos[file +1] == 0 && wPPos[file +1] != 0) b_piecePos += ROOK_ON_SEMI;
		else if(bPPos[file +1] == 0) b_piecePos += ROOK_ON_OPEN;
		if(rank == RANK_2 && (wPawnOnSecond || RanksBrd[SQ64(pos->kingSq[WHITE])] == RANK_1)) b_piecePos += ROOK_ON_SEVENTH;
		
		// TODO: Add bonus for rooks behind passed pawns
		if((passers & 0xFF) & FILE_TO_BIT_MASK[file]){
			if((wPPos[file + 1] & 0xFF) > rank) b_piecePos += ROOK_BEHIND_PASSER;
		}
		
		b_mobility += GenerateAttackBlackRook(pos, index);
		b_tropism += TROPISM_ROOK[Distance(pos->kingSq[WHITE], index)];
	}
	
	
	// Queens
	for(i = 0; i < pos->pceNum[wQ]; i++){
		
		index = pos->pList[wQ][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == wQ);
		
		// w_material += QUEEN_VALUE;
		if(!useEndingTables) w_piecePos += wQ_POS[SQ64(index)];
		else w_piecePos += QUEEN_POS_ENDING[SQ64(index)];
		
		// Queen on 7th
		if(RanksBrd[SQ64(index)] == RANK_7 && (bPawnOnSeventh || RanksBrd[SQ64(pos->kingSq[BLACK])] == RANK_8)) w_piecePos += QUEEN_ON_SEVENTH;
		
		w_mobility += GenerateAttackWhiteQueen(pos, index);
		w_tropism += TROPISM_QUEEN[Distance(pos->kingSq[BLACK], index)];
	}
	
	for(i = 0; i < pos->pceNum[bQ]; i++) {
		
		index = pos->pList[bQ][i];
		ASSERT(SqIs120(index));
		ASSERT(SqOnBoard(index));
		ASSERT(pos->pieces[index] == bQ);
		
		// b_material += QUEEN_VALUE;
		if(!useEndingTables) b_piecePos += wQ_POS[MIRROR64(SQ64(index))];
		else b_piecePos += QUEEN_POS_ENDING[SQ64(index)];
		
		if(RanksBrd[SQ64(index)] == RANK_2 && (wPawnOnSecond || RanksBrd[SQ64(pos->kingSq[WHITE])] == RANK_1)) b_piecePos += QUEEN_ON_SEVENTH;
		
		b_mobility += GenerateAttackBlackQueen(pos, index);
		b_tropism += TROPISM_QUEEN[Distance(pos->kingSq[WHITE], index)];
	}
	
	
	// Kings
	if(!useEndingTables) w_piecePos += wK_POS[SQ64(pos->kingSq[WHITE])];
	else w_piecePos += KING_POS_ENDING[SQ64(pos->kingSq[WHITE])];
	GenerateAttackWhiteKing(pos, pos->kingSq[WHITE]);

	if(!useEndingTables) b_piecePos += wK_POS[MIRROR64(SQ64(pos->kingSq[BLACK]))];
	else b_piecePos += KING_POS_ENDING[SQ64(pos->kingSq[BLACK])];
	GenerateAttackBlackKing(pos, pos->kingSq[BLACK]);
	
	// King safety, only opening and middle game
	s16 w_kingAttacked;
	s16 b_kingAttacked;
	s16 w_kingDefense;
	s16 b_kingDefense;
	if(gamePhase <= PHASE_MIDDLE) {
		w_kingAttacked = WhiteKingAttack(pos); // Check for black pieces attacking the king
		b_kingAttacked = BlackKingAttack(pos);
		w_kingDefense = WhiteKingDefense(pos); // Check for general defense measures (pawn shield etc)
		b_kingDefense = BlackKingDefense(pos);
		
		if(pos->side == WHITE){
			if(b_kingAttacked <= -75) tempo += TEMPO;
			tempo += TEMPO;
		}else{
			if(w_kingAttacked <= -75) tempo -= TEMPO;
			tempo -= TEMPO;
		}
		
	}else{
		w_kingAttacked = 0;
		b_kingAttacked = 0;
		w_kingDefense = 0;
		b_kingDefense = 0;
	}
	
	s16 w_hungPiece;
	s16 b_hungPiece;
	
	if(pos->side == WHITE) {
		w_hungPiece = WhiteHungPiece(pos);
		b_hungPiece = 0; // Only penalize the side moving for hung pieces
	}else{
		b_hungPiece = BlackHungPiece(pos);
		w_hungPiece = 0; // Only penalize the side moving for hung pieces			
	}
	
	s16 totalEval = (w_material - b_material) + (w_trappedEval - b_trappedEval) +
				(w_piecePos - b_piecePos) + (w_mobility - b_mobility) +
				(wPStructure - bPStructure) + (w_kingAttacked - b_kingAttacked) +
				(w_kingDefense - b_kingDefense) + (w_tropism - b_tropism) +
				(w_hungPiece - b_hungPiece)+ tempo + passerEval;
	
	
	// Adjust the score for likelyhood of a draw
	finalEval = DrawProbability(pos, totalEval);
	
	// printing
	#ifdef EVAL_DEBUG
		s16 drawProbAdjust = totalEval - finalEval;
		
		s16 wTempoEval = tempo < 0? 0 : tempo;
		s16 bTempoEval = tempo < 0? tempo : 0;
		
		s16 totalWhite = w_material + w_trappedEval + w_piecePos + passerEval + w_mobility + wPStructure + w_kingAttacked +
			w_kingDefense + w_tropism + w_hungPiece + wTempoEval;
		s16 totalBlack = b_material + b_trappedEval + b_piecePos + passerEval + b_mobility + bPStructure + b_kingAttacked +
		b_kingDefense + b_tropism + b_hungPiece + bTempoEval;

		printf("                   White Black Total\n");
		printf("Material.......... %5d %5d %5d\n", w_material, b_material, (w_material - b_material));
		printf("Positioning....... %5d %5d %5d\n", w_piecePos, b_piecePos, (w_piecePos - b_piecePos));
		printf("Trapped........... %5d %5d %5d\n", w_trappedEval, b_trappedEval, (w_trappedEval - b_trappedEval));
		printf("Mobility.......... %5d %5d %5d\n", w_mobility, b_mobility, (w_mobility - b_mobility));
		printf("Pawn structure.... %5d %5d %5d\n", wPStructure, bPStructure, (wPStructure - bPStructure));
		printf("King attacked..... %5d %5d %5d\n", w_kingAttacked, b_kingAttacked, (w_kingAttacked - b_kingAttacked));
		printf("King defense...... %5d %5d %5d\n", w_kingDefense, b_kingDefense, (w_kingDefense - b_kingDefense));
		printf("Tropism........... %5d %5d %5d\n", w_tropism, b_tropism, (w_tropism - b_tropism));
		printf("Hung pieces....... %5d %5d %5d\n", w_hungPiece, b_hungPiece, (w_hungPiece - b_hungPiece));
		printf("Tempo............. %5d %5d %5d\n", wTempoEval, bTempoEval, tempo);
		printf("Passing pawns..... %5d %5d %5d\n", 0, 0, passerEval);
		printf("\nTotal eval........ %5d %5d %5d\n", totalWhite, totalBlack, totalEval);
		printf("Adjusted to draw..     -     - %5d\n", drawProbAdjust);
		printf("Final eval........     -     - %5d\n", finalEval);
	#endif

	StoreEval(pos, finalEval);
	
	if(pos->side == WHITE) return finalEval;
	else return -finalEval;
}

/**
 * Caculate the evaluation value for a given move depends on piece positions.
 * 
 * @param pos The position's pointer.
 * @param from The square that piece moves from.
 * @param to The square that piece moves to.
 * @return finalEval
 */
s8 EvalMove(const S_BOARD *pos, const u8 from, const u8 to){
	
	ASSERT(PieceValid(pos->pieces[from]));
	gamePhase = GetGamePhase(pos);
	switch(pos->pieces[from]){
		case wP: return ((wP_POS[SQ64(to)] - wP_POS[SQ64(from)]) / 10);
		case bP: return ((wP_POS[MIRROR64(SQ64(to))] - wP_POS[MIRROR64(SQ64(from))]) / 10);
		case wN: return ((wN_POS[SQ64(to)] - wN_POS[SQ64(from)]) / 10);
		case bN: return ((wN_POS[MIRROR64(SQ64(to))] - wN_POS[MIRROR64(SQ64(from))]) / 10);
		case wB: return ((wB_POS[SQ64(to)] - wB_POS[SQ64(from)]) / 10);
		case bB: return ((wB_POS[MIRROR64(SQ64(to))] - wB_POS[MIRROR64(SQ64(from))]) / 10);
		case wR: return ((wR_POS[SQ64(to)] - wR_POS[SQ64(from)]) / 10);
		case bR: return ((wR_POS[MIRROR64(SQ64(to))] - wR_POS[MIRROR64(SQ64(from))]) / 10);
		case wQ: return ((wQ_POS[SQ64(to)] - wQ_POS[SQ64(from)]) / 10);
		case bQ: return ((wQ_POS[MIRROR64(SQ64(to))] - wQ_POS[MIRROR64(SQ64(from))]) / 10);
		case wK: 
			if(gamePhase > PHASE_MIDDLE) return ((KING_POS_ENDING[SQ64(to)] - KING_POS_ENDING[SQ64(from)]) / 10);
			else return ((wK_POS[SQ64(to)] - wK_POS[SQ64(from)]) / 10);
		case bK:
			if(gamePhase > PHASE_MIDDLE) return ((KING_POS_ENDING[MIRROR64(SQ64(to))] - KING_POS_ENDING[MIRROR64(SQ64(from))]) / 10);
			else return ((wK_POS[MIRROR64(SQ64(to))] - wK_POS[MIRROR64(SQ64(from))]) / 10);
		
	}
} 



