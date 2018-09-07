#include <stdio.h>
#include "sys/time.h"
#include "string.h"


// Positions
#define  initPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define  trickyPos "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 " // 48 possible moves
#define  whitePawnMoves "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1" // 26 possible moves
#define  blackPawnMoves "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1" // 26 possible moves
#define  castleMoves "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1 "

// Definitions
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

const int pawnAttacks[4] = {15, 17, -15, -17};
const int knightAttacks[8] = {31, 33, 14, 18, -31, -33, -14, -18};
const int kingAttacks[8] = {1, 15, 16, 17, -1, -15, -16, -17};
const int bishopAttacks[4] = {15, 17, -15, -17};
const int rookAttacks[4] = {1, 16, -1, -16};

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


typedef struct { int move; int score; } MOVE;
typedef struct { MOVE moves[256]; int moveCount; } MOVELIST;
typedef struct { int position[128]; int side; int enPassant; int castle; int kingSq[2]; } CHESSBOARD;


/********************************************
 *************** Misc macros ****************
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
	side = 0; enPassant = noSq; castle = 0
	
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
 
 // Move format
 
 /*
 
 	0000 0000 0000 0000 0111 1111		source square		0x7f
 	0000 0000 0011 1111 1000 0000		target square		0x3f80
 	0000 0011 1100 0000 0000 0000		promoted piece		0x3c000
 	0000 0100 0000 0000 0000 0000		capture flag		0x40000
 	0000 1000 0000 0000 0000 0000		pawn start flag		0x80000	
 	0001 0000 0000 0000 0000 0000		en passant flag		0x100000
 	0010 0000 0000 0000 0000 0000		castling flag		0x200000
 
 */

#define LoopMoves for(int moveCount = 0; moveCount < list->moveCount; ++moveCount)

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
	printf("	  cap: %d  pawnStart: %d  enPass: %d  castle: %d", \
	GetMoveCaptureFlag(move), \
	GetMovePawnStartFlag(move), \
	GetMoveEnPassantFlag(move), \
	GetMoveCastleFlag(move));
	
#define PrintMoveList(list) \
	LoopMoves { PrintMove(list->moves[moveCount].move); \
	printf("	SCORE: %d\n", list->moves[moveCount].score); } \
	printf("\n  Total moves: %d\n\n", list->moveCount);


/********************************************
 ************* Move generator ***************
 ********************************************/

static int IsSquareAttacked(CHESSBOARD *board, int sq, int attSide)
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

static inline void AddMove(MOVELIST *list, int move)
{
	list->moves[list->moveCount].move = move;
	list->moves[list->moveCount].score = 0;
	list->moveCount++;
}

static inline void GenerateMoves(CHESSBOARD *board, MOVELIST *list)
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
							AddMove(list, SetMove(fromSq, fromSq + 16, wN, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wB, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wR, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(list, SetMove(fromSq, fromSq + 16, 0, 0, 0, 0, 0));
					
							if(rank_2 && !GetSq(fromSq + 32))
								AddMove(list, SetMove(fromSq, fromSq + 32, 0, 0, 1, 0, 0));
							
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
									AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 1, 0));
							}
						}
					
						if((pawnAttacks[i] > 0) && !(dir & 0x88) && isBlackPiece(dir))
						{
							if(rank_7)
							{
								AddMove(list, SetMove(fromSq, dir, wN, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, wB, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, wR, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, wQ, 1, 0, 0, 0));
							
							}
						
							else
							{
								AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
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
								AddMove(list, SetMove(e1, g1, 0, 0, 0, 0, 1));
						}
					}
					
					if(castle & Q)
					{
						if(!GetSq(d1) && !GetSq(c1) && !GetSq(b1))
						{
							if(!IsSquareAttacked(board, e1, b) && !IsSquareAttacked(board, d1, b))
								AddMove(list, SetMove(e1, c1, 0, 0, 0, 0, 1));
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
							AddMove(list, SetMove(fromSq, fromSq - 16, bN, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bB, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bR, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(list, SetMove(fromSq, fromSq - 16, 0, 0, 0, 0, 0));
					
							if(rank_7 && !GetSq(fromSq - 32))
								AddMove(list, SetMove(fromSq, fromSq - 32, 0, 0, 1, 0, 0));
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
									AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 1, 0));
							}
						}
					
						if((pawnAttacks[i] < 0) && !(dir & 0x88) && isWhitePiece(dir))
						{
							if(rank_2)
							{
								AddMove(list, SetMove(fromSq, dir, bN, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, bB, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, bR, 1, 0, 0, 0));
								AddMove(list, SetMove(fromSq, dir, bQ, 1, 0, 0, 0));
							
							}
						
							else
							{
								AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
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
								AddMove(list, SetMove(e8, g8, 0, 0, 0, 0, 1));
						}
					}
				
					if(castle & q)
					{
						if(!GetSq(d8) && !GetSq(c8) && !GetSq(b8))
						{
							if(!IsSquareAttacked(board, e8, w) && !IsSquareAttacked(board, d8, w))
								AddMove(list, SetMove(e8, c8, 0, 0, 0, 0, 1));
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
								AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
							else
								AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
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
							AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						// on empty square
						else if(!delta)
						{		
							AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
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
							AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						// on empty square
						else if(!delta)
						{		
							AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
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
								AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
							else
								AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
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

static int MakeMove(CHESSBOARD *board, int move)
{
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
	
	// in check
	if(InCheck(board, side ^ 1))
	{
		TakeBack(board, boardStored);
		return 0;
	}
	
	else
		return 1;
}


/********************************************
 *************** Evaluation *****************
 ********************************************/

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

const int materialWeight[15] = 
{
	0, 100, 350, 350, 525, 1000, 10000, 0,
	0, -100, -350, -350, -525, -1000, -10000
};

const int Pawns[128] = 
{
     0,	  0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
    10,	 10,   0, -10, -10,   0,  10,  10,    0, 0, 0, 0, 0, 0, 0, 0,
     5,	  0,   0,	5,   5,   0,   0,   5,    0, 0, 0, 0, 0, 0, 0, 0,
     0,	  0,  10,  20,  20,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     5,	  5,   5,  10,  10,   5,   5,   5,    0, 0, 0, 0, 0, 0, 0, 0,
    10,	 10,  10,  20,  20,  10,  10,  10,    0, 0, 0, 0, 0, 0, 0, 0,
    20,	 20,  20,  30,  30,  20,  20,  20,    0, 0, 0, 0, 0, 0, 0, 0,
     0,	  0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
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
     0,   0,   0,   0,   0,   0,   0,   0,	  0, 0, 0, 0, 0, 0, 0, 0
};

const int Bishops[128] = 
{
     0,   0, -10,   0,   0, -10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,  10,  10,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  15,  15,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,  10,  15,  20,  20,  15,  10,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,  10,  15,  20,  20,  15,  10,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,  10,  15,  15,  10,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,  10,  10,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0, 	  0, 0, 0, 0, 0, 0, 0, 0
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
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
     0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0,
	 0,   0,   0,   0,   0,   0,   0,   0,    0, 0, 0, 0, 0, 0, 0, 0
};

int EvaluatePosition(CHESSBOARD *board)
{
	int score = 0;
	
	for(int sq = 0; sq < 128; ++sq)
	{
		if(!(sq & 0x88) && GetSq(sq))
		{
			// evaluate material
			score += materialWeight[GetSq(sq)];
		
			// evaluate piece placement
			/*switch(GetSq(sq))
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
			}*/
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

int NegaMaxSearch(CHESSBOARD *board, int depth)
{
	if(InCheck(board, side))
		depth++;

	if ( depth == 0 )
	{
		return EvaluatePosition(board);
	}
	
	int legalMoves = 0;
	int bestScore = -50000;
	
	MOVELIST list[1];
	GenerateMoves(board, list);
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		if(!MakeMove(board, list->moves[moveNum].move))
			continue;
		
		legalMoves++;
		int score = -NegaMaxSearch(board, depth - 1);
		
		if(score > bestScore)
			bestScore = score;
			
		TakeBack(board, boardStored);
	}
	
	// checkmate detection
	if(!legalMoves && InCheck(board, side))
		return -100000;
	else
		return bestScore;
}

void SearchPosition(CHESSBOARD *board, int depth)
{
	int bestScore = -50000;
	int legalMoves = 0;

	MOVELIST list[1];
	GenerateMoves(board, list);
	
	MOVE bestMove;
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		if(!MakeMove(board, list->moves[moveNum].move))
			continue;
			
		legalMoves++;	
		int score = -NegaMaxSearch(board, depth - 1);
		
		if(score > bestScore)
		{
			bestScore = score;
		
			
			bestMove.move = list->moves[moveNum].move;
			bestMove.score = bestScore;	
		}
		
		TakeBack(board, boardStored);
	}
	
	if(legalMoves)
	{
		printf("info score cp %d depth %d\n", bestMove.score, depth);
	
		printf("bestmove ");
		PrintMove(bestMove.move);
		printf("\n");
	}	
}


/********************************************
 ******************* UCI ********************
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

int ParseMove(CHESSBOARD *board, char *moveStr)
{
	MOVELIST list[1];
	GenerateMoves(board, list);

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

#define inputBuffer (400 * 6)

void UciLoop(CHESSBOARD *board)
{
	char line[inputBuffer];

	printf("id name chengine\n");
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
			printf("id name chengine\n");
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
					MakeMove(board, ParseMove(board, moves));
				}
				
				*moves++;
			}
			
			PrintBoard(board);
		}
		
		else if(!strncmp(line, "position startpos", 17))
		{
			ParseFen(board, initPos);
			PrintBoard(board);
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
			//printf("moves: \" %c \"", *moves);
			
			if(*moves == 's')
			{
				int countChar = -1;
			
				while(*moves)
				{
					if(*moves == ' ')
					{
						*moves++;
						MakeMove(board, ParseMove(board, moves));
					}
				
					*moves++;
				}
				
				PrintBoard(board);
			}
			
			else
				PrintBoard(board);
		}
		
		else if (!strncmp(line, "go depth", 8))
		{
			char *go = line;
			go += 9;
			
			int depth = *go - '0';
			
			SearchPosition(board, depth);
		}
		
		else if(!strncmp(line, "quit", 4))
			break;
			
		else
			SearchPosition(board, 4);
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

static void Perft(CHESSBOARD *board, int depth)
{
	if(depth == 0)
	{
		nodes++;
		return;
	}
	
	MOVELIST list[1];
	GenerateMoves(board, list);
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
	
		if(!MakeMove(board, list->moves[moveNum].move))
			continue;
			
		Perft(board, depth - 1);
		TakeBack(board, boardStored);
	}
}


void PerftTest(CHESSBOARD *board, int depth)
{
	PrintBoard(board);
	
	printf("\n  Perft to depth: %d\n\n",depth);
		
	nodes = 0;
	
	int start = GetTimeMs();
	
	MOVELIST list[1];
	GenerateMoves(board, list);
	
	int move;
	
	for(int moveNum = 0; moveNum < list->moveCount; ++moveNum)
	{
		move = list->moves[moveNum].move;
		
		CHESSBOARD boardStored[1];
		boardStored[0] = board[0];
		
		if(!MakeMove(board, move))
			continue;	
		
		long cumnodes = nodes;
		
		Perft(board, depth - 1);
		TakeBack(board, boardStored);
		
		long oldnodes = nodes - cumnodes;
		
		printf("  move %d:	", moveNum + 1);
		PrintMove(move);
		printf(": %ld\n", oldnodes);
    }
	
	printf("\n  Test Complete : %ld nodes visited in %dms\n\n", nodes, GetTimeMs() - start);
	
	return;
}


/********************************************
 **************** Chenglite *****************
 ********************************************/

int main()
{
	CHESSBOARD board[1];
	UciLoop(board);
	
	return 0;
}















