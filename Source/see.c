/**
 * @file see.c
 * This file handles Static Exchage Evaluation techniuqe.
 *
 * That is if a certain square is attacked numerous times by both sides we play
 * out the sequence of captures and see what the gains were.
 * 
 * I have used ideas from the open source engines Glaurung and Scorpio here.
 * 
 * @author Tarek Elmenshawy (tarek.elmenshawy1997@gmail.com)
 * Date: April 2020
 */

#include"defs.h"

static u8 move_to; 											// Holds the square where the captures take place
static u8 move_from; 											// Holds the square from the initial attacker
static u8 b_attackers_total; 									// Keeps track of the number of white attackers
static u8 w_attackers_total; 									// Keeps track of the number of black attackers



/// The values are full pawns and not centipawns, and we don't need to worry
/// about nuances like giving queens a slightly higher value than 9 pawns
const u8 piece_values[13] = {0, 1, 3, 3, 5, 9, 99, 1, 3, 3, 5, 9, 99};
static u8 w_attackers[16] = {0};
static u8 b_attackers[16] = {0};
static s8 scores[32] = {0}; // Holds the values after each capture


/**
 * Iterates through all empty squares from the startIndex in the direction
 * of the given delta. If we run into a piece we check if it is the same
 * type as 'piece'. If it is we increment the number of attackers (for the
 * right side depending on the color of the found piece) and record the
 * index that is standing on in the right array.
 * 
 * @param startIndex
 *            Where we start looking from
 * @param delta
 *            What delta this piece uses
 * @param piece
 *            What type of piece uses this delta
 * @param pos
 *            The postion's pointer 
 */
static void AddSlider(u8 startIndex, s8 delta, u8 piece, const S_BOARD *pos){
	
	ASSERT(SqOnBoard(startIndex));
	// Initialize the square by moving to the next square from startIndex in the delta direction
	u8 square = startIndex + delta;

	
	// Follow the delta until we exit the board or run into a piece
	while(!SQOFFBOARD(square) && pos->pieces[square] == EMPTY){
		square += delta;
	}
	
	// If we didn't leave the board and we are not trying to add the initial attacker
	if(!SQOFFBOARD(square) && square != move_from){
		
		// Catch both queen and the type of piece we submitted, if it
		// matches save the index and increment the count
		if(pos->pieces[square] == wQ || pos->pieces[square] == piece){
			w_attackers[w_attackers_total] = square;
			w_attackers_total++;
		}
		// Same but for black pieces, we negate 'piece' to get the black equivalent
		else if (pos->pieces[square] == bQ || pos->pieces[square] == piece + 6) {
			b_attackers[b_attackers_total] = square;
			b_attackers_total++;
		}

	}

}

/**
 * Take a start index somewhere on the board and compare it to the to-index
 * we got from the move in SEE(move), from this we find a delta and then
 * call AddSlider with the proper delta and pieces
 * 
 * Since only sliding pieces can be hiding behind other pieces we don't have
 * to worry about the other kinds (king/knight)
 * 
 * @param startIndex
 *            Where to start looking for the hidden piece
 * @param pos
 *            The postion's pointer 
 */
static void AddHidden(u8 startIndex, const S_BOARD *pos){
	
	// Find out what kind of pieces can move in this delta
	u8 pieceType = AttackArray[move_to - startIndex + 77];
	s8 delta = move_to - startIndex;

	// If rook is one of types, call AddSlider with the right delta and rook
	// as piece type
	// same if bishop is one of the types
	switch(pieceType){
		
		case ATTACK_KQR:
		case ATTACK_QR:
			if(delta % 10 == 0){
				if(delta > 0) delta = -10;
				else delta = 10;
			}
			else{
				if(delta > 0) delta = -1;
				else delta = 1;
			}
			AddSlider(startIndex, delta, wR, pos);
			break;
			
		case ATTACK_KQBwP:
		case ATTACK_KQBbP:
		case ATTACK_QB:
			if(delta % 11 == 0){
				if(delta > 0) delta = -11;
				else delta = 11;
			}
			else{
				if(delta > 0) delta = -9;
				else delta = 9;
			}
			AddSlider(startIndex, delta, wB, pos);
			break;
			
	}

}

/**
 * Find all attackers of the to-square in the 'move', including hidden
 * attackers (pieces that are 'hiding' behind other pieces that can capture
 * once the intervening piece does its capture) and simulate the capture
 * sequence to determine what is gained/lost from the capture.
 * 
 * @param move
 *            The capture we want to check
 * @param pos
 *            The postion's pointer 
 * @return capture_value What was gained/lost from the capture sequence
 */
s32 SEE(const S_BOARD *pos, s32 move){
	
	ASSERT(MoveValid(move, pos));
	ASSERT(CAPTURED(move) != EMPTY || move & MFLAGEP);
	
	// A simple check to see we are actually trying a capturing move
	// if the move is not a capture there is not capture sequence
	// and we won't go any further
	if(!(CAPTURED(move) != EMPTY || move & MFLAGEP)) return 0;

	// Initialize global variables
	move_from = FROMSQ(move);
	ASSERT(SqOnBoard(move_from));
	ASSERT(PieceValid(pos->pieces[move_from]));
	
	move_to = TOSQ(move);
	ASSERT(SqOnBoard(move_to));
	ASSERT(PieceValid(pos->pieces[move_to] || move & MFLAGEP));
	
	w_attackers_total = 0;
	b_attackers_total = 0;

	u8 capture_value = 0; 									// Will be set to the total gain of the caputre sequence
	u8 tempAttack;										 	// Temporary variable used below
	u8 sideToMove; 											// Keeps track of what side is moving
	u8 attacked_piece_value; 								// Keeps track of the value of the piece that is standing on the attacked square after each capture                                  

	u8 pceNum;

	/* Add attacking pawns */

	// If the square diagonally down to the right is a white pawn, and the
	// pawn is not the initial attacker, add it
	if(!SQOFFBOARD(move_to - 9) && pos->pieces[move_to - 9] == wP && move_from != (move_to - 9)){
		w_attackers[w_attackers_total] = move_to - 9;
		w_attackers_total++;
	}
	
	// Same but diagonally down to the left
	if(!SQOFFBOARD(move_to - 11) && pos->pieces[move_to - 11] == wP && move_from != (move_to - 11)){
		w_attackers[w_attackers_total] = move_to - 11;
		w_attackers_total++;
	}
	
	// Diagonally up to the left
	if(!SQOFFBOARD(move_to + 9) && pos->pieces[move_to + 9] == bP && move_from != (move_to + 9)){
		b_attackers[b_attackers_total] = move_to + 9;
		b_attackers_total++;
	}
	
	// Diagonally up to the right
	if(!SQOFFBOARD(move_to + 11) && pos->pieces[move_to + 11] == bP && move_from != (move_to + 11)){
		b_attackers[b_attackers_total] = move_to + 11;
		b_attackers_total++;
	}
	

	/* Add attacking knights */

	// White knights
	// If there is a piece and it is not the initial attacker
	for(pceNum = 0; pceNum < pos->pceNum[wN]; pceNum++){
		
		tempAttack = pos->pList[wN][pceNum];
		if(tempAttack != move_from){
			// If it is possible for the knight to attack the move_to square
			// add it to attackers
			if(AttackArray[move_to - tempAttack + 77] == ATTACK_N) {
				w_attackers[w_attackers_total] = tempAttack;
				w_attackers_total++;
			}
		}
	}
	
	// Black knights
	for(pceNum = 0; pceNum < pos->pceNum[bN]; pceNum++){
		
		tempAttack = pos->pList[bN][pceNum];
		
		if(tempAttack != move_from){
			// If it is possible for the knight to attack the move_to square
			// add it to attackers
			if(AttackArray[move_to - tempAttack + 77] == ATTACK_N) {
				b_attackers[b_attackers_total] = tempAttack;
				b_attackers_total++;
			}
		}
	}

	/* Add attacking kings */

	// White king
	// If the king wasn't the initial attacker
	if(pos->kingSq[WHITE] != move_from){
		
		// If it is possible for the king to attack the move_to square add it to attackers
		tempAttack = AttackArray[move_to - pos->kingSq[WHITE] + 77];
		
		if (tempAttack == ATTACK_KQR || tempAttack == ATTACK_KQBwP || tempAttack == ATTACK_KQBbP){
			w_attackers[w_attackers_total] = pos->kingSq[WHITE];
			w_attackers_total++;
		}
	}
	
	// Black king
	if(pos->kingSq[BLACK] != move_from){
		
		// If it is possible for the king to attack the move_to square add it to attackers
		tempAttack = AttackArray[move_to - pos->kingSq[BLACK] + 77];
		
		if (tempAttack == ATTACK_KQR || tempAttack == ATTACK_KQBwP || tempAttack == ATTACK_KQBbP){
			b_attackers[b_attackers_total] = pos->kingSq[BLACK];
			b_attackers_total++;
		}
	}

	/* Add attacking sliders */

	// Sliders that move diagonally
	AddSlider(move_to,  11, wB, pos);
	AddSlider(move_to,   9, wB, pos);
	AddSlider(move_to,  -9, wB, pos);
	AddSlider(move_to, -11, wB, pos);

	// Sliders that move straight
	AddSlider(move_to,  10, wR, pos);
	AddSlider(move_to, -10, wR, pos);
	AddSlider(move_to,   1, wR, pos);
	AddSlider(move_to,  -1, wR, pos);

	// All obvious attackers are now added to the arrays
	//**************************************************

	// Now we start with 'making' the initial move so we can find out if
	// there is a hidden piece behind it, that is a piece that is able
	// to capture on the move_to square if the inital piece captures
	// We do this to get the inital move out of the way since it will
	// always happen first and should not be ordered

	// Important: Below we don't actually carry out the moves on the board
	// for each capture
	// we simply simulate it by toggling the sideToMove variable and
	// setting attacked_piece_value to the value of the piece that 'captured'
	// any reference to a piece on the move_to square below is simply the
	// piece there after a simulated capture

	attacked_piece_value = piece_values[pos->pieces[move_from]]; 		// A new piece is now 'standing' on the attacked square (the move_to square)
	sideToMove = pos->side ^ 1; 										// Toggle the side to move since we simulated a move here
	
	
	AddHidden(move_from, pos); 											// We now add any hidden attacker that was behind the inital attacker
	
	if(move & MFLAGEP){
		if(pos->side == WHITE){
			scores[0] =  piece_values[bP];
			AddHidden(move_to - 10, pos); 
		}else{
			scores[0] =  piece_values[wP];
			AddHidden(move_to + 10, pos); 
		}
	}else scores[0] =  piece_values[CAPTURED(move)]; 					// We now have a first for the capture sequence, this is the value of the initally captured piece

	u8 scoresIndex = 1; 												// Keeps track of where in the sequence we are
	u8 w_attackers_count = 0; 											// Keeps track of how many white pieces we have analyzed below
	u8 b_attackers_count = 0; 											// Keeps track of how many black pieces we have analyzed below
	u8 lowestValueIndex; 												// Temporary variable to keep track of the index of the least valuable spot in the sequence
	u8 lowestValue; 													// Temporary variable to keep track of the value of the least valuable spot in the sequence
	u8 tempSwap; 														// Temporary variable used for swapping places in the attackers arrays


	// Start looping, when we run out of either black or white pieces to analyze we break out
	// Inside we always capture with the least valuable piece left first
	while(1){
		
		// If we have run out of pieces for the side to move we are finished and break out
		if((sideToMove == WHITE && w_attackers_count == w_attackers_total) || (sideToMove == BLACK && b_attackers_count == b_attackers_total)) {
			break;
		}

		// Set the next step in the sequence to the value of the piece now
		// on the move_to square - the previous score in the sequence
		scores[scoresIndex] = attacked_piece_value - scores[scoresIndex - 1];
		scoresIndex++; 													// Move to the next step in the sequence
		
		
		if(sideToMove == WHITE){
			lowestValueIndex = w_attackers_count; 						// Get the index for the piece we're now analyzing
			lowestValue = piece_values[pos->pieces[w_attackers[w_attackers_count]]]; // Get the value for that piece

			// Loop from the next attacker to the total number of attackers
			for(u8 i = w_attackers_count + 1; i < w_attackers_total; i++){
				// If the value for this piece is less then the currently
				// lowest value of a piece in the sequence
				// update the lowestValueIndex and lowestValue so they
				// reflect the now lowest valued piece
				if(piece_values[pos->pieces[w_attackers[i]]] < lowestValue){
					lowestValueIndex = i;
					lowestValue = piece_values[pos->pieces[w_attackers[i]]];
				}
			}

			// If the lowestValueIndex got updated above we have a new
			// lowest value and we swap it
			if(lowestValueIndex != w_attackers_count){
				// Swap the places
				tempSwap = w_attackers[lowestValueIndex];
				w_attackers[lowestValueIndex] = w_attackers[w_attackers_count];
				w_attackers[w_attackers_count] = tempSwap;
			}

			// We have now analyzed the piece on the w_attackers_count in the w_attackers array
			// so we can now look for hidden attackers behind it
			AddHidden(w_attackers[w_attackers_count], pos);

			// We now simulate the move for the analyzed piece
			attacked_piece_value = lowestValue; 						// A new piece is now 'standing' on the attacked square (the move_to square)
			sideToMove = BLACK; 										// Since it was white to move, it is now black's turn
			w_attackers_count++; 										// On the next pass of the loop (when it's white's turn) we check the next attacking piece
			
		}else{															// Black to move, works exactly as above only for black attackers
			
			lowestValueIndex = b_attackers_count;
			lowestValue = piece_values[pos->pieces[b_attackers[b_attackers_count]]];
			
			for(s32 i = b_attackers_count + 1; i < b_attackers_total; i++){
				
				if(piece_values[pos->pieces[b_attackers[i]]] < lowestValue){
					lowestValueIndex = i;
					lowestValue = piece_values[pos->pieces[b_attackers[i]]];
				}
			}
			
			if(lowestValueIndex != b_attackers_count){
				tempSwap = b_attackers[lowestValueIndex];
				b_attackers[lowestValueIndex] = b_attackers[b_attackers_count];
				b_attackers[b_attackers_count] = tempSwap;
			}
			
			AddHidden(b_attackers[b_attackers_count], pos);
			attacked_piece_value = lowestValue;
			sideToMove = WHITE;
			b_attackers_count++;
			
		}
	}
	
	
	// Loop through the scores array, starting from the end (scoresIndex kept track of how many entries it has)
	// This loop moves the smallest value to the front of the list
	while(scoresIndex > 1){
		// Since scoresIndex holds the total number of sequences we start
		// with decrementing it so we get the first place in the array
		scoresIndex--;
		
		// If the place before the scoresIndex is greater than the current place
		if(scores[scoresIndex - 1] > -scores[scoresIndex]){
			// Set the place before scoresIndex to the value of scoresIndex
			scores[scoresIndex - 1] = -scores[scoresIndex];
		}
	}

	// The value of the capture sequence is now in the front of the scores list
	capture_value = scores[0];
	// Return the value of the capture in centipawns
	return (capture_value * 100);
}

