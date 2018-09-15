/*******************************************************************************
 *********************** Chenglite v1.0 by Maksim Korzh ************************
 *******************************************************************************
 *                                                                             *
 *      Minimalist UCI chess engine written by self learner from scratch       *
 *                                                                             *                   
 *******************************************************************************
 *         ************************ FEATURES ************************          *
 *******************************************************************************
 *                                                                             *
 *  - 0x88 board representation                                                *
 *  - move generation via calculations                                         *
 *  - copy-make make move approach                                             *
 *  - material and piece placement evaluation                                  *
 *  - nega max brute force search algorithm                                    *
 *  - fixed depth quiescence search                                            *
 *  - UCI protocol with depth search only                                      *
 *                                                                             *
 *                                                                             * 
 *******************************************************************************
 *         ****************** Intents and purposes ******************          *
 *******************************************************************************
 *                                                                             *
 *  Chenglite is intended to be a source of inspiration for the beginners and  *
 *  hobby programmers dreaming to write their own bug free chess programs. At  *
 *  first it's not that easy to take the idea that your engine is going to be  *
 *  quite weak and slow, but don't let it be the reason to stop further work,  *
 *  try to dive right into the developing process instead and you will notice  *
 *  pretty soon the true reason why are you doing this.                        *
 *                                                                             *
 *                                                                             *
 *******************************************************************************
 *         ********* My chess engine development philosophy *********          *
 *******************************************************************************
 *  - try each time to do the same thing you've done before,                   *
 *    but in less code                                                         *
 *                                                                             *
 *  - try to get the best performance                                          *
 *    out of a given algorithms                                                *
 *                                                                             *
 *  - don't add new features until being sure you've got everything out        *
 *    of the previously added ones                                             *
 *                                                                             *
 *  - don't try to write the strongest chess engine in the world,              *
 *    remember that Stockfish and Leela chess has been developed               *
 *    by numerous top level programmers                                        *
 *                                                                             *
 *  - don't hurry, chess engine development requires more accuracy             *
 *    rather than high speed coding skills                                     *
 *                                                                             *
 *  - enjoy the process, this is the most principle thing to                   *
 *    consider                                                                 *
 *                                                                             *
 *                                                                             *
 *******************************************************************************
 ********** Chenglite Copyleft (No Copiright) 2018 to Maksim Korzh *************
 *******************************************************************************/

#include <stdio.h>
#include "sys/time.h"
#include "string.h"


// Definitions
#define  initPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define  trickyPos "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 " // 48 possible moves

enum side { w, b };
enum pieces { emSq, wP, wN,	wB,	wR,	wQ,	wK,	offBoard = 8, bP, bN, bB, bR, bQ, bK };
enum castling { K = 1, Q = 2, k = 4, q = 8 };

enum squares {
	a1 = 0,   b1, c1, d1, e1, f1, g1, h1,
	a2 = 16,  b2, c2, d2, e2, f2, g2, h2,
	a3 = 32,  b3, c3, d3, e3, f3, g3, h3,
	a4 = 48,  b4, c4, d4, e4, f4, g4, h4,
	a5 = 64,  b5, c5, d5, e5, f5, g5, h5,
	a6 = 80,  b6, c6, d6, e6, f6, g6, h6,
	a7 = 96,  b7, c7, d7, e7, f7, g7, h7,
	a8 = 112, b8, c8, d8, e8, f8, g8, h8, noSq = -99
};

enum moveFlags { all, captures };

char *unicodePieces[15] = {
	".", "\u2659", "\u2658", "\u2657", "\u2656", "\u2655", "\u2654", "-",
	" ", "\u265F", "\u265E", "\u265D", "\u265C", "\u265B", "\u265A"
};

char promotedPieceChar[15] = {
	[wN] = 'n', [wB] = 'b', [wR] = 'r', [wQ] = 'q',
	[bN] = 'n', [bB] = 'b', [bR] = 'r', [bQ] = 'q',	
};

int pieceChars[67] = {
	[32] = wP, [30] = wN, [18] = wB, [34] = wR, [33] = wQ, [27] = wK,
	[64] = bP, [62] = bN, [50] = bB, [66] = bR, [65] = bQ, [59] = bK
};

// attack directions
const int pawnAttacks[4] = {15, 17, -15, -17};
const int knightAttacks[8] = {31, 33, 14, 18, -31, -33, -14, -18};
const int kingAttacks[8] = {1, 15, 16, 17, -1, -15, -16, -17};
const int bishopAttacks[4] = {15, 17, -15, -17};
const int rookAttacks[4] = {1, 16, -1, -16};

// tracking whether kings or rooks moved
const int castling[128] = 
{
	13, 15, 15, 15, 12, 15, 15, 14, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	15, 15, 15, 15, 15, 15, 15, 15, 8, 8, 8, 8, 8, 8, 8, 8,
	 7, 15, 15, 15,  3, 15, 15, 11, 8, 8, 8, 8, 8, 8, 8, 8
};

// material weight of pieces
const int materialWeight[15] = 
{
	0, 100, 300, 350, 525, 1000, 10000, 0,
	0, -100, -300, -350, -525, -1000, -10000
};

// piece olacement evaluation tables
const int Pawns[128] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
    10,  10,   0, -10, -10,   0,  10,  10,    0, 0, 0, 0, 0, 0, 0, 0,
     5,   0,   0,   5,   5,   0,   0,   5,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  20,  20,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     5,   5,   5,  10,  10,   5,   5,   5,    0, 0, 0, 0, 0, 0, 0, 0,
    10,  10,  10,  20,  20,  10,  10,  10,    0, 0, 0, 0, 0, 0, 0, 0,
    20,  20,  20,  30,  30,  20,  20,  20,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

const int Knights[128] = 
{
     0, -10,   0,   0,   0,   0, -10,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   5,   5,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  10,  10,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  20,  20,  10,   5,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     5,  10,  15,  20,  20,  15,  10,   5,    0, 0, 0, 0, 0, 0, 0, 0,
     5,  10,  10,  20,  20,  10,  10,   5,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

const int Bishops[128] = 
{
     0,   0, -10,   0,   0, -10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,  10,  10,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  15,  15,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,  10,  20,  20,  20,  20,  10,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,  10,  15,  20,  20,  15,  10,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  15,  15,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,  10,  10,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

const int Rooks[128] =
{
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
    25,  25,  25,  25,  25,  25,  25,  25,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   5,  10,  10,   5,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

const int Kings[128] = 
{
     5,   5,  20, -10, -10,   5,  30,   5,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0, -10, -10,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

// Mirror evaluation tables for opposite side
const int Mirror[128] =
{
	a8, b8, c8, d8, e8, f8, g8, h8,    0, 0, 0, 0, 0, 0, 0, 0,
	a7, b7, c7, d7, e7, f7, g7, h7,    0, 0, 0, 0, 0, 0, 0, 0,
	a6, b6, c6, d6, e6, f6, g6, h6,    0, 0, 0, 0, 0, 0, 0, 0,
	a5, b5, c5, d5, e5, f5, g5, h5,    0, 0, 0, 0, 0, 0, 0, 0,
	a4, b4, c4, d4, e4, f4, g4, h4,    0, 0, 0, 0, 0, 0, 0, 0,
	a3, b3, c3, d3, e3, f3, g3, h3,    0, 0, 0, 0, 0, 0, 0, 0,
	a2, b2, c2, d2, e2, f2, g2, h2,    0, 0, 0, 0, 0, 0, 0, 0,
	a1, b1, c1, d1, e1, f1, g1, h1,    0, 0, 0, 0, 0, 0, 0, 0
};

#define MirrorSq(sq) Mirror[sq]

static int mvv_lva[15][15] =
{
	0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,   0,
	0, 105, 205, 305, 405, 505, 605, 0, 0, 105, 205, 305, 405, 505, 605,
	0, 104, 204, 304, 404, 504, 604, 0, 0, 104, 204, 304, 404, 504, 604,
	0, 103, 203, 303, 403, 503, 603, 0, 0, 103, 203, 303, 403, 503, 603,
	0, 102, 202, 302, 402, 502, 602, 0, 0, 102, 202, 302, 402, 502, 602,
	0, 101, 201, 301, 401, 501, 601, 0, 0, 101, 201, 301, 401, 501, 601,
	0, 100, 200, 300, 400, 500, 600, 0, 0, 100, 200, 300, 400, 500, 600,
	0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0, 0, 0,   0,   0,   0,   0,   0,   0,
	0, 105, 205, 305, 405, 505, 605, 0, 0, 105, 205, 305, 405, 505, 605,
	0, 104, 204, 304, 404, 504, 604, 0, 0, 104, 204, 304, 404, 504, 604,
	0, 103, 203, 303, 403, 503, 603, 0, 0, 103, 203, 303, 403, 503, 603,
	0, 102, 202, 302, 402, 502, 602, 0, 0, 102, 202, 302, 402, 502, 602,
	0, 101, 201, 301, 401, 501, 601, 0, 0, 101, 201, 301, 401, 501, 601,
	0, 100, 200, 300, 400, 500, 600, 0, 0, 100, 200, 300, 400, 500, 600
};

// move, movelist, board, search structs
typedef struct { int move; int score; } MOVE;
typedef struct { MOVE moves[256]; int moveCount; } MOVELIST;

typedef struct
{ 
	int position[128]; 

	int side; 
	int enPassant; 
	int castle; 

	int kingSq[2];
	
	int ply;
}

CHESSBOARD;


typedef struct
{
	long nodes;
	double fhf, fh;
	
	int bestMove;
	int pvTable[64];
	
	int historyScore[15][128];
	int killerMoves[2][64];
}

SEARCH;

/********************************************
 ************** Square macros ***************
 ********************************************/

// 0x88 math
#define IsOnBoard(sq) (!(sq & 0x88))
#define fr2sq(file, rank) (rank * 16 - file)
#define parse2sq(file, rank) ((rank - 1) * 16 + file)
#define GetFile(sq) (sq & 7)
#define GetRank(sq) (sq >> 4)
#define rank_7 (fromSq >= a7 && fromSq <= h7)
#define rank_2 (fromSq >= a2 && fromSq <= h2)

// Convertions
#define GetFileChar(sq) (GetFile(sq) + 'a')
#define GetRankChar(sq) (GetRank(sq) + '1')
#define GetUnicodePiece(piece) unicodePieces[piece]
#define Char2Piece(char) pieceChars[char - '0']
	
// Char type
#define isDigit(char) (char >= '0' && char <= '9')
#define isPieceChar(piece) ((*fen >= 'a' && *fen <= 'z') || ((*fen >= 'A' && *fen <= 'Z')))


/********************************************
 ************** Board macros ****************
 ********************************************/

// White or black
#define isBlack(toSq) (board->position[toSq] >= bN && board->position[toSq] <= bQ)
#define isWhite(toSq) (board->position[toSq] >= wN && board->position[toSq] <= wQ)
#define isBlackPiece(toSq) (board->position[toSq] >= bP && board->position[toSq] <= bK)
#define isWhitePiece(toSq) (board->position[toSq] >= wP && board->position[toSq] <= wK)


// Quick board access
#define pos(sq)		board->position[sq]
#define side		board->side
#define enPassant	board->enPassant
#define castle		board->castle
#define kingSq(col)	board->kingSq[col]
#define ply			board->ply

// Board loops
#define LoopBoard for(int sq = 0; sq < 128; ++sq)
#define RankLoop for(int rank = 8; rank >= 1; rank--)
#define FileLoop for(int file = 16; file >= 1; file--)

// Board methods
#define SetSq(sq, piece) (pos(sq) = piece)
#define GetSq(sq) pos(sq)
#define PrintSquare(sq) \
	if(sq == -99) printf("no");\
	else printf("%c%c", GetFileChar(sq), GetRankChar(sq));
#define PrintPromotedPiece(piece) printf("%c", promotedPieceChar[piece])

// Init board
#define ResetPosition(board) \
	LoopBoard { IsOnBoard(sq) ? SetSq(sq, emSq) : SetSq(sq, offBoard); }

#define ResetStats(board) \
	side = 0; enPassant = noSq; castle = 0; ply = 0;
	
#define ResetBoard(board) \
	ResetPosition(board); ResetStats(board)

// Print board
#define PrintPosition(board) \
	printf("\n"); \
	RankLoop { printf("  %d", rank); \
	FileLoop { printf("  %s", GetUnicodePiece(GetSq(fr2sq(file, rank)))); }\
	printf("\n"); }
	
#define PrintStats(board) \
	printf("\n     a  b  c  d  e  f  g  h\n\n"); \
	printf("     Side:            %s\n", side ? "black" : "white"); \
	printf("     EnPassant:          "); \
	PrintSquare(enPassant); \
	printf("\n"); \
	printf("     Castling:         %c%c%c%c\n", \
		castle & K ? 'K' : '-',  \
		castle & Q ? 'Q' : '-',  \
		castle & k ? 'k' : '-',  \
		castle & q ? 'q' : '-'); \
	printf("\n\n");
	
#define PrintBoard(board) PrintPosition(board); PrintStats(board)


/********************************************
 *************** Move macros ****************
 ********************************************/
  
 /* Move format
 
 	0000 0000 0000 0000 0111 1111		source square		0x7f
 	0000 0000 0011 1111 1000 0000		target square		0x3f80
 	0000 0011 1100 0000 0000 0000		promoted piece		0x3c000
 	0000 0100 0000 0000 0000 0000		capture flag		0x40000
 	0000 1000 0000 0000 0000 0000		pawn start flag		0x80000	
 	0001 0000 0000 0000 0000 0000		en passant flag		0x100000
 	0010 0000 0000 0000 0000 0000		castling flag		0x200000
 
 */

#define SetMove(f, t, prom, cap, pawn, e, cas) \
	((f) | (t << 7) | (prom << 14) | (cap << 18) | (pawn << 19) | (e << 20) | (cas << 21))
	
#define GetMoveSource(move) (move & 0x7f)
#define GetMoveTarget(move) ((move >> 7) & 0x7f)
#define GetMovePromPiece(move) ((move >> 14) & 0xf)
#define GetMoveCaptureFlag(move) ((move >> 18) & 1)
#define GetMovePawnStartFlag(move) ((move >> 19) & 1)
#define GetMoveEnPassantFlag(move) ((move >> 20) & 1)
#define GetMoveCastleFlag(move) ((move >> 21) & 1)

#define PrintMove(move) \
	printf("  "); \
	PrintSquare(GetMoveSource(move)); \
	PrintSquare(GetMoveTarget(move)); \
	PrintPromotedPiece(GetMovePromPiece(move)); \
	/*printf("	  cap: %d  pawnStart: %d  enPass: %d  castle: %d", \
	GetMoveCaptureFlag(move), \
	GetMovePawnStartFlag(move), \
	GetMoveEnPassantFlag(move), \
	GetMoveCastleFlag(move));*/
	
#define LoopMoves for(int moveCount = 0; moveCount < list->moveCount; ++moveCount)

#define PrintMoveList(list) \
	LoopMoves { PrintMove(list->moves[moveCount].move); \
	printf("	SCORE: %d\n", list->moves[moveCount].score); } \
	printf("\n  Total moves: %d\n\n", list->moveCount);


/********************************************
 ************* Move generator ***************
 ********************************************/

static inline int IsSquareAttacked(CHESSBOARD *board, int sq, int attSide)
{
	// by pawns
	if(!attSide)
	{
		if(!((sq - 15) & 0x88) && (GetSq(sq - 15) == wP))
			return 1;
			
		if(!((sq - 17) & 0x88) && (GetSq(sq - 17) == wP))
			return 1;
	}
	
	else
	{
		if(!((sq + 15) & 0x88) && (GetSq(sq + 15) == bP))
			return 1;
			
		if(!((sq + 17) & 0x88) && (GetSq(sq + 17) == bP))
			return 1;
	}

	// by knights
	for(int i = 0; i < 8; ++i)
	{
		int dir = sq + knightAttacks[i];
		int delta = GetSq(dir);
		
		if(!(dir & 0x88))
		{
			if(attSide ? delta == bN : delta == wN)
				return 1;
		}
	}
	
	
	// by bishops and queens
	for(int i = 0; i < 4; ++i)
	{
		int dir = sq + bishopAttacks[i];
		
		while(!(dir & 0x88))
		{
			int delta = GetSq(dir);
			
			if(attSide ? (delta == bB) || (delta == bQ) : (delta == wB) || (delta == wQ))
				return 1;
			
			else if(delta != 0)
				break;
			
			dir += bishopAttacks[i];
		}
	}
	
	
	// by rooks and queens
	for(int i = 0; i < 4; ++i)
	{
		int dir = sq + rookAttacks[i];
		
		while(!(dir & 0x88))
		{
			int delta = GetSq(dir);
			
			if(attSide ? (delta == bR) || (delta == bQ) : (delta == wR) || (delta == wQ))
				return 1;
			
			else if(delta != 0)
				break;
			
			dir += rookAttacks[i];
		}
	}
	
	
	// by kings
	for(int i = 0; i < 8; ++i)
	{
		int dir = sq + kingAttacks[i];
		int delta = GetSq(dir);
		
		if(!(dir & 0x88))
		{
			if(attSide ? delta == bK : delta == wK)
				return 1;
		}
	}

	return 0;
}


static inline void AddMove(CHESSBOARD *board, SEARCH *info, MOVELIST *list, int move)
{
	list->moves[list->moveCount].move = move;
	
	if(GetMoveCaptureFlag(move))
		list->moves[list->moveCount].score = mvv_lva[GetSq(GetMoveSource(move))][GetSq(GetMoveTarget(move))] + 10000;
	
	if(!GetMoveCaptureFlag(move))
	{
		if(info->killerMoves[w][ply] == move)
			list->moves[list->moveCount].score = 9000;
	
		else if(info->killerMoves[b][ply] == move)
			list->moves[list->moveCount].score = 8000;
		
		else
			list->moves[list->moveCount].score = info->historyScore[GetSq(GetMoveSource(move))][GetMoveTarget(move)];
	}
		
	list->moveCount++;
}

static inline void GenerateMoves(CHESSBOARD *board, SEARCH *info, MOVELIST *list)
{	
	list->moveCount = 0;
	
	LoopBoard
	{	
		if(!(sq & 0x88))
		{
			// skip empty squares
			if(!GetSq(sq))
				continue;
				
			int fromSq = sq;
			
			if(!side)
			{	
				if(GetSq(fromSq) == wP)
				{
					// pawn quiet move
					if(!((fromSq + 16) & 0x88) && !GetSq(fromSq + 16))
					{
						if(rank_7 && !GetSq(fromSq + 16))
						{
							AddMove(board, info, list, SetMove(fromSq, fromSq + 16, wN, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq + 16, wB, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq + 16, wR, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq + 16, wQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(board, info, list, SetMove(fromSq, fromSq + 16, 0, 0, 0, 0, 0));
					
							if(rank_2 && !GetSq(fromSq + 32))
								AddMove(board, info, list, SetMove(fromSq, fromSq + 32, 0, 0, 1, 0, 0));
							
						}
					}
				
					// pawn capture move
					for(int i = 0; i < 4; ++i)
					{
						int dir = fromSq + pawnAttacks[i];
					
						// en passant move
						if(pawnAttacks[i] > 0 && !(dir & 0x88))
						{
							if(enPassant != noSq)
							{
								if(dir == enPassant)
									AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 1, 0));
							}
						}
					
						if((pawnAttacks[i] > 0) && !(dir & 0x88) && isBlackPiece(dir))
						{
							if(rank_7)
							{
								AddMove(board, info, list, SetMove(fromSq, dir, wN, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, wB, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, wR, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, wQ, 1, 0, 0, 0));
							
							}
						
							else
							{
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							}
						}
					}
				}
			
				// castling
				if(GetSq(fromSq) == wK)
				{
					if(castle & K)
					{
						if(!GetSq(f1) && !GetSq(g1))
						{
							if(!IsSquareAttacked(board, e1, b) && !IsSquareAttacked(board, f1, b))
								AddMove(board, info, list, SetMove(e1, g1, 0, 0, 0, 0, 1));
						}
					}
					
					if(castle & Q)
					{
						if(!GetSq(d1) && !GetSq(c1) && !GetSq(b1))
						{
							if(!IsSquareAttacked(board, e1, b) && !IsSquareAttacked(board, d1, b))
								AddMove(board, info, list, SetMove(e1, c1, 0, 0, 0, 0, 1));
						}
					}
				}
			}
			
			else
			{	
				if(GetSq(fromSq) == bP)
				{
					// pawn quiet move
					if(!((fromSq - 16) & 0x88) && !GetSq(fromSq - 16))
					{
						if(rank_2 && !GetSq(fromSq - 16))
						{
							AddMove(board, info, list, SetMove(fromSq, fromSq - 16, bN, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq - 16, bB, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq - 16, bR, 0, 0, 0, 0));
							AddMove(board, info, list, SetMove(fromSq, fromSq - 16, bQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(board, info, list, SetMove(fromSq, fromSq - 16, 0, 0, 0, 0, 0));
					
							if(rank_7 && !GetSq(fromSq - 32))
								AddMove(board, info, list, SetMove(fromSq, fromSq - 32, 0, 0, 1, 0, 0));
						}
					}
				
					// pawn capture move
					for(int i = 0; i < 4; ++i)
					{
						int dir = fromSq + pawnAttacks[i];
					
						// en passant move
						if(pawnAttacks[i] < 0 && !(dir & 0x88))
						{
							if(enPassant != noSq)
							{
								if(dir == enPassant)
									AddMove(board, info, list, SetMove(fromSq, dir, 0, 0, 0, 1, 0));
							}
						}
					
						if((pawnAttacks[i] < 0) && !(dir & 0x88) && isWhitePiece(dir))
						{
							if(rank_2)
							{
								AddMove(board, info, list, SetMove(fromSq, dir, bN, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, bB, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, bR, 1, 0, 0, 0));
								AddMove(board, info, list, SetMove(fromSq, dir, bQ, 1, 0, 0, 0));
							
							}
						
							else
							{
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							}
						}
					}
				}
				
				// castling
				if(GetSq(fromSq) == bK)
				{	
					if(castle & k)
					{
						if(!GetSq(f8) && !GetSq(g8))
						{
							if(!IsSquareAttacked(board, e8, w) && !IsSquareAttacked(board, f8, w))
								AddMove(board, info, list, SetMove(e8, g8, 0, 0, 0, 0, 1));
						}
					}
				
					if(castle & q)
					{
						if(!GetSq(d8) && !GetSq(c8) && !GetSq(b8))
						{
							if(!IsSquareAttacked(board, e8, w) && !IsSquareAttacked(board, d8, w))
								AddMove(board, info, list, SetMove(e8, c8, 0, 0, 0, 0, 1));
						}
					}
				}
			}
			
			// knights
			if(side ? GetSq(fromSq) == bN : GetSq(fromSq) == wN)
			{
				for(int i = 0; i < 8; ++i)
				{
					int dir = sq + knightAttacks[i];
					int delta = GetSq(dir);
		
					if(!(dir & 0x88))
					{
						if(side ? (!delta || isWhitePiece(dir)) : (!delta || isBlackPiece(dir)))
						{
							if(!delta)
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
							else
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
						}
					}
				}
			}
			
			// bishops and queens
			if(	side ? 
					(GetSq(fromSq) == bB) || (GetSq(fromSq) == bQ) :
					(GetSq(fromSq) == wB) || (GetSq(fromSq) == wQ)
			)
			
			{
				for(int i = 0; i < 4; ++i)
				{
					int dir = sq + bishopAttacks[i];
		
					while(!(dir & 0x88))
					{
						int delta = GetSq(dir);
						
						// if hits own piece
						if(side ? isBlackPiece(dir) : isWhitePiece(dir))
							break;
						
						// if hits opponent's piece		
						else if(side ? isWhitePiece(dir) : isBlackPiece(dir))
						{
							AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						// on empty square
						else if(!delta)
						{		
							AddMove(board, info, list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
						}
			
						dir += bishopAttacks[i];
					}
				}
			}
			
			// rooks and queens
			if( side ? 
					(GetSq(fromSq) == bR) || (GetSq(fromSq) == bQ) :
					(GetSq(fromSq) == wR) || (GetSq(fromSq) == wQ)
			)
			
			{
				for(int i = 0; i < 4; ++i)
				{
					int dir = sq + rookAttacks[i];
		
					while(!(dir & 0x88))
					{
						int delta = GetSq(dir);
						
						// if hits own piece
						if(side ? isBlackPiece(dir) : isWhitePiece(dir))
							break;
								
						// if hits opponent's piece
						else if(side ? isWhitePiece(dir) : isBlackPiece(dir))
						{
							AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						// on empty square
						else if(!delta)
						{		
							AddMove(board, info, list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
						}
			
						dir += rookAttacks[i];
					}
				}
			}
			
			// kings
			if(side ? GetSq(fromSq) == bK : GetSq(fromSq) == wK)
			{
				for(int i = 0; i < 8; ++i)
				{
					int dir = sq + kingAttacks[i];
					int delta = GetSq(dir);
		
					if(!(dir & 0x88))
					{
						if(side ? (!delta || isWhitePiece(dir)) : (!delta || isBlackPiece(dir)))
						{
							if(!delta)
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
							else
								AddMove(board, info, list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
						}
					}
				}
			}
		}	
	}
}


/********************************************
 **************** Make move *****************
 ********************************************/

#define TakeBack(board, boardStored) board[0] = boardStored[0];
#define InCheck(board, sideToMove) \
	IsSquareAttacked(board, sideToMove ? kingSq(b) : kingSq(w), sideToMove ^ 1)

static inline int MakeMove(CHESSBOARD *board, int move, int capFlag)
{
	// if capFlag make only captures else make all

	if(!capFlag)
	{
		ply++;
	
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];

		int fromSq = GetMoveSource(move);
		int toSq = GetMoveTarget(move);

		// move piece
		GetSq(toSq) = GetSq(fromSq);
		GetSq(fromSq) = emSq;
	
		// promotions
		if(GetMovePromPiece(move))
		{
			GetSq(toSq) = GetMovePromPiece(move);
			GetSq(fromSq) = emSq;
		}
		
		// en passant flag
		if(GetMoveEnPassantFlag(move))
		{
			side ? 
				(GetSq(enPassant + 16) = 0) :
				(GetSq(enPassant - 16) = 0);
			
			enPassant = noSq;
		}
	
		enPassant = noSq;
	
		// pawn start flag
		if(GetMovePawnStartFlag(move))
		{
			side ?
				(enPassant = toSq + 16) :
				(enPassant = toSq - 16);
		}
	
		// castling flag
		if(GetMoveCastleFlag(move))
		{
			switch(toSq)
			{
				case g1:
					GetSq(f1) = GetSq(h1);
					GetSq(h1) = emSq;
					break;
				
				case c1:
					GetSq(d1) = GetSq(a1);
					GetSq(a1) = emSq;
					break;
				
				case g8:
					GetSq(f8) = GetSq(h8);
					GetSq(h8) = emSq;
					break;
				
				case c8:
					GetSq(d8) = GetSq(a8);
					GetSq(a8) = emSq;
					break;
			}
		}
	
		// update castling permission
		castle &= castling[fromSq];
		castle &= castling[toSq];
	
		// update kingSq
		if(GetSq(GetMoveTarget(move)) == wK || GetSq(GetMoveTarget(move)) == bK)
			kingSq(side) = GetMoveTarget(move);	
	
		// change side
		side ^= 1;
	
		// take back if king is in check
		if(InCheck(board, side ^ 1))
		{
			TakeBack(board, boardStored);
			return 0;
		}
	
		else
			return 1;
	}
	
	else
	{
		if(GetMoveCaptureFlag(move))
			MakeMove(board, move, all);
		else
			return 0;
	}
}


/********************************************
 *************** Evaluation *****************
 ********************************************/

static inline int EvaluatePosition(CHESSBOARD *board)
{
	int score = 0;
	
	for(int sq = 0; sq < 128; ++sq)
	{
		if(!(sq & 0x88) && GetSq(sq))
		{
			// evaluate material
			score += materialWeight[GetSq(sq)];
		
			// evaluate piece placement
			switch(GetSq(sq))
			{
				case wP:
					score += Pawns[sq];
					break;
					
				case wN:
					score += Knights[sq];
					break;
					
				case wB:
					score += Bishops[sq];
					break;
					
				case wR:
					score += Rooks[sq];
					break;
					
					
				case wK:
					score += Kings[sq];
					break;
					
					
				case bP:
					score -= Pawns[MirrorSq(sq)];
					break;
					
				case bN:
					score -= Knights[MirrorSq(sq)];
					break;
					
				case bB:
					score -= Bishops[MirrorSq(sq)];
					break;
					
				case bR:
					score -= Rooks[MirrorSq(sq)];
					break;
					
				case bK:
					score -= Kings[MirrorSq(sq)];
					break;
			}
		}
	}
	
	if(!side)
		return score;
		
	else
		return -score;
}


/********************************************
 **************** Search ********************
 ********************************************/

static int QuiescenceSearch(int alpha, int beta, CHESSBOARD *board, SEARCH *info)
{
	int legalMoves = 0;
	int eval = EvaluatePosition(board);
	info->nodes++;
	
	if(eval >= beta)
		return beta;
			
	if(eval > alpha)
		alpha = eval;
		
	MOVELIST list[1];
	GenerateMoves(board, info, list);
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		/* move ordering */
		
		// sort moves in descending order
		for (int nextMove = moveNum + 1; nextMove < list->moveCount; ++nextMove)
		{
			if (list->moves[moveNum].score < list->moves[nextMove].score)
			{
				int tempScore = list->moves[moveNum].score;
				int tempMove = list->moves[moveNum].move;
				
				list->moves[moveNum].score = list->moves[nextMove].score;
				list->moves[nextMove].score = tempScore;
				
				list->moves[moveNum].move = list->moves[nextMove].move;
				list->moves[nextMove].move = tempMove;
			}
		}
		
		if(!MakeMove(board, list->moves[moveNum].move, captures))
			continue;
		
		legalMoves++;
		int score = -QuiescenceSearch(-beta, -alpha, board, info);
		TakeBack(board, boardStored);
		
		if(score >= beta)
		{
			if(legalMoves == 1)
				info->fhf++;
				
			info->fh++;	
			return beta;
		}
			
		if(score > alpha)
			alpha = score;
	}
	
	return alpha;
}


static int NegaMaxSearch(int alpha, int beta, CHESSBOARD *board, SEARCH *info, int depth)
{
	int bestMove = 0;
	int oldAlpha = alpha;
	int score = -50000;	  
	int legalMoves = 0;
	
	info->nodes++;

	if(InCheck(board, side))
		depth++;

	if(!depth)
		return QuiescenceSearch(alpha, beta, board, info);
		
	MOVELIST list[1];
	GenerateMoves(board, info, list);
	
	
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		/* move ordering */
		
		// sort moves in descending order
		for (int nextMove = moveNum + 1; nextMove < list->moveCount; ++nextMove)
		{
			if (list->moves[moveNum].score < list->moves[nextMove].score)
			{
				int tempScore = list->moves[moveNum].score;
				int tempMove = list->moves[moveNum].move;
				
				list->moves[moveNum].score = list->moves[nextMove].score;
				list->moves[nextMove].score = tempScore;
				
				list->moves[moveNum].move = list->moves[nextMove].move;
				list->moves[nextMove].move = tempMove;
			}
		}
		
		if(info->bestMove)
		{
			for(int i = 0; i < list->moveCount; ++i)
			{
				// make the best move of previous iteration leftmost 
				/*if(list->moves[i].move == info->bestMove)
				{
					list->moves[i].move = list->moves[0].move;
					list->moves[0].move = info->bestMove;	
				}*/
			
				// put killer move after
				/*if(list->moves[i].move == info->killerMove)
				{
					list->moves[i].move = list->moves[0].move;
					list->moves[0].move = info->killerMove;	
				}*/
			}
		}
				
		if(!MakeMove(board, list->moves[moveNum].move, all))
			continue;
		
		//PrintMoveList(list);
		//getchar();
		
		legalMoves++;
		score = -NegaMaxSearch(-beta, -alpha, board, info, depth - 1);
		TakeBack(board, boardStored);
		
		if(score >= beta)
		{
			if(legalMoves == 1)
				info->fhf++;
				
			info->fh++;
			
			if(!GetMoveCaptureFlag(list->moves[moveNum].move))
			{
				info->killerMoves[b][ply] = info->killerMoves[w][ply];
				info->killerMoves[w][ply] = list->moves[moveNum].move;
			}
				
			return beta; // fail hard beta-cutoff, mating score 100000 is cut here
		}
			
		if(score > alpha)
		{
			alpha = score;			
			bestMove = list->moves[moveNum].move;
			
			if(!(GetMoveCaptureFlag(list->moves[moveNum].move)))
				info->historyScore[GetSq(GetMoveSource(bestMove))][GetMoveTarget(bestMove)] += depth;
		}
	}
		
	if(!legalMoves)
	{
		if(InCheck(board, side))
			return -49000; // on checkmate
			
		else
			return 0; // on stalemate
	}
	
	if(alpha != oldAlpha)
	{
		info->bestMove = bestMove;
		
		
	}
	
	return alpha;
}


static inline void SearchPosition(CHESSBOARD *board, SEARCH *info, int depth)
{	
	// iterative deepening
	for(int currentDepth = 1; currentDepth <= depth; currentDepth++)
	{
		int score = NegaMaxSearch(-50000, 50000, board, info, currentDepth);
		printf("info score cp %d depth %d nodes %ld\n", score, currentDepth, info->nodes);
		
		if(score == 49000)
			break;
	}
	
	printf("bestmove ");
	PrintMove(info->bestMove);
	printf("\n");
	
	printf("Move ordering: %.2f\n",(info->fhf/info->fh));	
}


/*
	H.G.Muller's implementation of PV-table

	Move pvStack[10000];
	Move *pvPtr = pvStack;
	
	int Search()
	{
	  Move *pvStart = pvPtr; // start of own PV
	  *pvStart = 0; // initialize empty PV at the top of the PV stack
	  
	  
	  // where you increase alpha because of a better move:
	  pvPtr = pvStart + 1;
	  while((*pvStart++ = *pvPtr++)) ; // copy PV of daughter, including the terminating 0
	  *pvStart = move; // the move we just searched is now the first of the new PV
	  
	  // when returning
	  pvPtr = pvStart; // pop PV (but leave data in place, so daughter can pick it up)
	}
*/

 
/********************************************
 ************* Parse FEN/Move ***************
 ********************************************/

void ParseFen(CHESSBOARD *board, char *fen)
{
	ResetBoard(board);
	
	RankLoop
	{
		FileLoop
		{
			int sq = fr2sq(file, rank);
			
			// parse position
			if(IsOnBoard(sq))
			{
				if(isPieceChar(*fen))
				{
					if(*fen == 'K')
						kingSq(w) = sq;
						
					else if(*fen == 'k')
						kingSq(b) = sq;	
					
					SetSq(sq, Char2Piece(*fen));
					*fen++;
				}
				
				if(isDigit(*fen))
				{
					int count = *fen - '0';
				
					if(!GetSq(sq))
						file++;
					
					file -= count;
					*fen++;
				}
			
				if(*fen == '/')
				{
					*fen++;
					file--;
				}
			}
		}
	}
	
	*fen++;
	
	// parse stats
	side = (*fen == 'w') ? w : b; fen += 2;
	
	while(*fen != ' ')
	{
		switch(*fen)
		{
			case 'K': castle |= K; break;
			case 'Q': castle |= Q; break;
			case 'k': castle |= k; break;
			case 'q': castle |= q; break;
			
			case '-': castle = 0; } fen++;
	}
	
	fen++;
	
	if(*fen != '-')
	{
		int file = fen[0] - 'a';
		int rank = fen[1] - '0';
		enPassant = parse2sq(file, rank);
	}
}

int ParseMove(CHESSBOARD *board, SEARCH *info, char *moveStr)
{
	MOVELIST list[1];
	GenerateMoves(board, info, list);

	int parseFrom = (moveStr[0] - 'a') + (moveStr[1] - '0' - 1) * 16;
	int parseTo = (moveStr[2] - 'a') + (moveStr[3] - '0' - 1) * 16;
	int promPiece = 0;
	
	int move;
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		move = list->moves[moveNum].move;
		
		if(GetMoveSource(move) == parseFrom && GetMoveTarget(move) == parseTo)
		{
			promPiece = GetMovePromPiece(move);
			
			if(promPiece)
			{	
				if((promPiece == wN || promPiece == bN) && moveStr[4] == 'n')
					return move;
					
				else if((promPiece == wB || promPiece == bB) && moveStr[4] == 'b')
					return move;
					
				else if((promPiece == wR || promPiece == bR) && moveStr[4] == 'r')
					return move;
					
				else if((promPiece == wQ || promPiece == bQ) && moveStr[4] == 'q')
					return move;
					
				continue;
			}
		
			return move;
		}
	}
	
	return 0;
}


/********************************************
 ******************* UCI ********************
 ********************************************/
 
#define inputBuffer (400 * 6)

void UciLoop(CHESSBOARD *board, SEARCH *info)
{
	char line[inputBuffer];

	printf("id name Chenglite\n");
	printf("id author Maksim Korzh\n");
	printf("uciok\n");
	
	while(1)
	{
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);
		
		if(!fgets(line, inputBuffer, stdin))
			continue;
			
		if(line[0] == '\n')
			continue;
			
		if (!strncmp(line, "uci", 3))
		{
			printf("id name Chenglite\n");
			printf("id author Maksim Korzh\n");
			printf("uciok\n");
		}
		
		else if(!strncmp(line, "isready", 7))
		{
			printf("readyok\n");
			continue;
		}
		
		else if (!strncmp(line, "ucinewgame", 10))
		{
			ParseFen(board, initPos);
			PrintBoard(board);
		}
		
		else if(!strncmp(line, "position startpos moves", 23))
		{
			ParseFen(board, initPos);
			PrintBoard(board);
			
			char *moves = line;
			moves += 23;
			
			int countChar = -1;
			
			while(*moves)
			{
				if(*moves == ' ')
				{
					*moves++;
					MakeMove(board, ParseMove(board, info, moves), all);
				}
				
				*moves++;
			}
			
			PrintBoard(board);
		}
		
		else if(!strncmp(line, "position startpos", 17))
		{
			ParseFen(board, initPos);
			//PrintBoard(board);
		}
		
		else if(!strncmp(line, "position fen", 12))
		{
			char *fen = line;
			fen += 13;
			
			ParseFen(board, fen);
			
			char *moves = line;
			
			while(strncmp(moves, "moves", 5))
			{
				*moves++;
				
				if(*moves == '\0')
					break;
			}
			
			moves += 4;
			
			if(*moves == 's')
			{
				int countChar = -1;
			
				while(*moves)
				{
					if(*moves == ' ')
					{
						*moves++;
						MakeMove(board, ParseMove(board, info, moves), all);
					}
				
					*moves++;
				}
			}
			
			PrintBoard(board);
		}
		
		else if (!strncmp(line, "go depth", 8))
		{
			char *go = line;
			go += 9;
			
			int depth = *go - '0';
			
			SearchPosition(board, info, depth);
		}
		
		else if(!strncmp(line, "quit", 4))
			break;
			
		else
			SearchPosition(board, info, 4);
	}
}


/********************************************
 ***************** Perft ********************
 ********************************************/

long nodes;

int GetTimeMs()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

static void Perft(CHESSBOARD *board, SEARCH *info, int depth, int moveFlag)
{
	if(depth == 0)
	{
		nodes++;
		return;
	}
	
	MOVELIST list[1];
	GenerateMoves(board, info, list);
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
	
		if(!MakeMove(board, list->moves[moveNum].move, moveFlag))
			continue;
			
		Perft(board, info, depth - 1, moveFlag);
		TakeBack(board, boardStored);
	}
}


void PerftTest(CHESSBOARD *board, SEARCH *info, int depth, int moveFlag)
{
	PrintBoard(board);
	
	printf("\n  Perft to depth: %d\n\n",depth);
		
	nodes = 0;
	
	int start = GetTimeMs();
	
	MOVELIST list[1];
	GenerateMoves(board, info, list);
	
	int move;
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		move = list->moves[moveNum].move;
		
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		if(!MakeMove(board, move, moveFlag))
			continue;	
		
		long cumnodes = nodes;
		
		Perft(board, info, depth - 1, moveFlag);
		TakeBack(board, boardStored);
		
		long oldnodes = nodes - cumnodes;
		
		printf("  move %d:	", moveNum + 1);
		PrintMove(move);
		printf(": %ld\n", oldnodes);
    }
	
	printf("\n  Test Complete : %ld nodes visited in %dms\n\n", nodes, GetTimeMs() - start);
	
	return;
}

void InitSearch(SEARCH *info)
{
	info->nodes = 0;
	info->fhf = 0;
	info->fh = 0;
	
	for(int i = 0; i < 15; ++i)
	{
		for(int j = 0; j < 128; ++j)
		{
			info->historyScore[i][j] = 0;
		}
	}
	
	for(int i = 0; i < 2; ++i)
	{
		for(int j = 0; j < 64; ++j)
		{
			info->killerMoves[i][j] = 0;
		}
	}
}

/********************************************
 **************** Chenglite *****************
 ********************************************/

int main()
{
	CHESSBOARD board[1];
	SEARCH info[1];
	InitSearch(info);
	
	ParseFen(board, "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1");
	PrintBoard(board);
	
	
	
	
	SearchPosition(board, info, 5);
	
	
	//UciLoop(board, info);
	
	return 0;
}






