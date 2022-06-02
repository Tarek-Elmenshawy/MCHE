# MCHE
Momentum's Chess Engine (MCHE), version 1.0, April 2020.

MCHE is the result of a chess programming reseach. it is not considered as one of the strong chess engines, but it is still stroger than some others. MCHE is written in C.

## Inplemented Featchers
- Search Techniques
	* Iterative Deepening
	* Alpha-Beta Search
	* Null-Move Search
	* Principle Variation Search(PV)
	* Late Move Reduction(LMR)
	* Quiescent Serach
- Moves Ordering
	* PV Move
	* Capturing Moves By Static Exchange Evaluation(SEE)
	* Killer Moves(Beta cut off)
	* History Heuristic(Beating alpha)
- Evaluation
	* Material
	* Piece Position By Piece-Square Table
	* Piece Mobility
	* King Safty
	* Pawn Structure

- Hash Table
	* Trasposition Table with (Two-tier System) replacement strategy

- Communication Protocols
	* UCI Protocol
	* XBoard Protocol

## Settings
- Hash      (default 32)  --> Size of main transposition table (in MB)
- EvalHash  (default 16)  --> Size of eval hash table (in MB)
- PawnHash  (default 8)   --> Size of pawn hash table (in MB)
