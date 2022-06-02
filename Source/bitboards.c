/**
 * @file bitboards.c
 * This file is responsible for bit processing.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */
 
#include"stdio.h"
#include"defs.h"


/// The BitTable[] is taken from chess programming wiki and used in PopBit().
static const u8 BitTable[64] = {
	63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	58, 20, 37, 17, 36, 8
};

/** 
 * Returns the index of the first setted(1) bit from LSB to MSB and resets it into 0.
 *
 * NOTE: PopBit() is taken from chess programming wiki.
 *
 * @param bb The pointer of the bit board.
 */
u8 PopBit(u64 *bb){
	u64 b = *bb ^ (*bb - 1);
	u32 fold = (unsigned) ((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}


/**
 * Returns the nnumber of setted bits.
 *
 * @param b The bit board.
 */
u8 CountBits(u64 b){
	u8 c = 0;
	for(c = 0; b; c++, b &= (b -1));
	return c;
}


/**
 * To print u64 variable as board on console.
 *
 * @param bb The bit board.
 */
void PrintBitBoard(u64 bb){
	
	u64 shiftMe = 1ULL;
	s8 rank, file , sq, sq64;
	
	for(rank = RANK_8; rank >= RANK_1; rank--){
		for(file = FILE_A; file <= FILE_H; file++){
			sq = FR2SQ(file, rank);
			sq64 = SQ64(sq);
			if((shiftMe << sq64) & bb) printf(" X ");
			else printf(" - ");
		}
		printf("\n");
	}
	printf("\n\n");
	
}