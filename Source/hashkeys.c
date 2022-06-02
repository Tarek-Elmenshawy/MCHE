/**
 * @file hashkeys.c
 * THis file is responsible for generating Zobrist/Hash Keys.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

/**
 * Returns a unique posKey for each board posiyions 
 * by using Piece, Side and Castle keys arrays.
 *
 * @param pos The position's pointer.
 * @return 64bits posKey
 */
u64 GeneratePosKey(const S_BOARD *pos){
	
	//To generate psKey we need to pieces, side to mave, en passent square and castle permisssion
	u8 sq = 0;
	u64 finalKey = 0ULL;
	u8 piece = EMPTY;
	
	//pieces
	for(sq = 0; sq < 64; sq++){
		piece = pos->pieces[SQ120(sq)];
		if(piece != EMPTY){
			ASSERT(piece >= wP && piece <= bK);
			finalKey ^= PieceKeys[piece][(sq)];
		}
	}
	
	//side
	if(pos->side == WHITE) finalKey ^= SideKey;
	
	//en passent
	if(pos->enPass != NO_SQ){
		ASSERT(pos->enPass >= 0 && pos->enPass < BRD_SQ_NUM);
		ASSERT(FilesBrd[SQ64(pos->enPass)] != OFF_BOARD);
		ASSERT(RanksBrd[SQ64(pos->enPass)] == RANK_3 || RanksBrd[SQ64(pos->enPass)] == RANK_6);
		finalKey ^= PieceKeys[EMPTY][SQ64(pos->enPass)];
	}
	
	//castle permission
	ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);
	finalKey ^= CastleKeys[pos->castlePerm];
	
	return finalKey;
}

/**
 * Returns a unique posKey for each pawn structure.
 * by using Piece keys array.
 * 
 * @param pos The position's pointer.
 * @return 64bits pawnKey
 */
u64 GeneratePawnKey(const S_BOARD *pos){
	
	u8 i, sq;
	u64 finalKey = 0ULL;

	//pieces
	for(i = 0; i < pos->pceNum[wP]; i++){
		sq = pos->pList[wP][i];
		finalKey ^= PieceKeys[wP][SQ64(sq)];
	}
	
	for(i = 0; i < pos->pceNum[bP]; i++){
		sq = pos->pList[bP][i];
		finalKey ^= PieceKeys[bP][SQ64(sq)];
	}
	return finalKey;
}




