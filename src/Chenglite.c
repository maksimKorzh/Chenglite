#include <stdio.h>
#include "sys/time.h"
#include "string.h"


// Positions
#define  initPos "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define  trickyPos "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 " // 48 possible moves
#define whitePawnMoves "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1" // 26 possible moves
#define blackPawnMoves "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1" // 26 possible moves
#define castleMoves "r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1 "

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
typedef struct { int position[128]; int side; int enPassant; int castle; int kingSq[2]} CHESSBOARD;


/********************************************
 *************** Misc macros ****************
 ********************************************/

// 0x88 math
#define IsOnBoard(sq) (!(sq & 0x88))
#define fr2sq(file, rank) (rank * 16 - file)
#define parse2sq(file, rank) ((rank - 1) * 16 + file)
#define GetFile(sq) (sq & 7)
#define GetRank(sq) (sq >> 4)

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
	if(attSide == w)
	{
		if(!((sq - 15) & 0x88) && (board->position[sq - 15] == wP))
			return 1;
			
		if(!((sq - 17) & 0x88) && (board->position[sq - 17] == wP))
			return 1;
	}
	
	else
	{
		if(!((sq + 15) & 0x88) && (board->position[sq + 15] == bP))
			return 1;
			
		if(!((sq + 17) & 0x88) && (board->position[sq + 17] == bP))
			return 1;
	}

	// by knights
	for(int i = 0; i < 8; ++i)
	{
		int dir = sq + knightAttacks[i];
		int delta = board->position[dir];
		
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
			int delta = board->position[dir];
			
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
			int delta = board->position[dir];
			
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
		int delta = board->position[dir];
		
		if(!(dir & 0x88))
		{
			if(attSide ? delta == bK : delta == wK)
				return 1;
		}
	}

	return 0;
}

static void AddMove(MOVELIST *list, int move)
{
	list->moves[list->moveCount].move = move;
	list->moves[list->moveCount].score = 0;
	list->moveCount++;
}

void GenerateMoves(CHESSBOARD *board, MOVELIST *list)
{	
	list->moveCount = 0;
	
	for(int sq = 0; sq < 128; ++sq)
	{
		if(!(sq & 0x88))
		{
			int fromSq = sq;
			
			if(side == w)
			{	
				if(board->position[fromSq] == wP)
				{
					// pawn quiet move
					if(!((fromSq + 16) & 0x88) && board->position[fromSq + 16] == emSq)
					{
						if((fromSq >= a7 && fromSq <= h7) && board->position[fromSq + 16] == emSq)
						{
							AddMove(list, SetMove(fromSq, fromSq + 16, wN, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wB, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wR, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq + 16, wQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(list, SetMove(fromSq, fromSq + 16, 0, 0, 0, 0, 0));
					
							if((fromSq >= a2 && fromSq <= h2) && board->position[fromSq + 32] == emSq)
							{
								AddMove(list, SetMove(fromSq, fromSq + 32, 0, 0, 1, 0, 0));
							}
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
								{
									//assert(enPassant > dir);
									AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 1, 0));
								}
							}
						}
					
						if((pawnAttacks[i] > 0) && !(dir & 0x88) && isBlackPiece(dir))
						{
							if(fromSq >= a7 && fromSq <= h7)
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
				if(board->position[fromSq] == wK)
				{
					if(castle & K)
					{
						if(board->position[f1] == emSq && board->position[g1] == emSq)
						{
							if(!IsSquareAttacked(board, e1, b) && !IsSquareAttacked(board, f1, b))
								AddMove(list, SetMove(e1, g1, 0, 0, 0, 0, 1));
						}
					}
					
					if(castle & Q)
					{
						if(board->position[d1] == emSq && board->position[c1] == emSq && board->position[b1] == emSq)
						{
							if(!IsSquareAttacked(board, e1, b) && !IsSquareAttacked(board, d1, b))
								AddMove(list, SetMove(e1, c1, 0, 0, 0, 0, 1));
						}
					}
				}
			}
			
			else
			{	
				if(board->position[fromSq] == bP)
				{
					// pawn quiet move
					if(!((fromSq - 16) & 0x88) && board->position[fromSq - 16] == emSq)
					{
						if((fromSq >= a2 && fromSq <= h2) && board->position[fromSq - 16] == emSq)
						{
							AddMove(list, SetMove(fromSq, fromSq - 16, bN, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bB, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bR, 0, 0, 0, 0));
							AddMove(list, SetMove(fromSq, fromSq - 16, bQ, 0, 0, 0, 0));
						}
					
						else
						{
							AddMove(list, SetMove(fromSq, fromSq - 16, 0, 0, 0, 0, 0));
					
							if((fromSq >= a7 && fromSq <= h7) && board->position[fromSq - 32] == emSq)
							{
								AddMove(list, SetMove(fromSq, fromSq - 32, 0, 0, 1, 0, 0));
							}
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
							if(fromSq >= a2 && fromSq <= h2)
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
				if(board->position[fromSq] == bK)
				{	
					if(castle & k)
					{
						if(board->position[f8] == emSq && board->position[g8] == emSq)
						{
							if(!IsSquareAttacked(board, e8, w) && !IsSquareAttacked(board, f8, w))
								AddMove(list, SetMove(e8, g8, 0, 0, 0, 0, 1));
						}
					}
				
					if(castle & q)
					{
						if(board->position[d8] == emSq && board->position[c8] == emSq && board->position[b8] == emSq)
						{
							if(!IsSquareAttacked(board, e8, w) && !IsSquareAttacked(board, d8, w))
								AddMove(list, SetMove(e8, c8, 0, 0, 0, 0, 1));
						}
					}
				}
			}
			
			// knights
			if(side ? board->position[fromSq] == bN : board->position[fromSq] == wN)
			{
				for(int i = 0; i < 8; ++i)
				{
					int dir = sq + knightAttacks[i];
					int delta = board->position[dir];
		
					if(!(dir & 0x88))
					{
						if(side ? (delta == emSq || isWhitePiece(dir)) : (delta == emSq || isBlackPiece(dir)))
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
					(board->position[fromSq] == bB) || (board->position[fromSq] == bQ) :
					(board->position[fromSq] == wB) || (board->position[fromSq] == wQ)
			)
			
			{
				for(int i = 0; i < 4; ++i)
				{
					int dir = sq + bishopAttacks[i];
		
					while(!(dir & 0x88))
					{
						int delta = board->position[dir];
			
						if(side ? isBlackPiece(dir) : isWhitePiece(dir))
							break;
								
						else if(side ? isWhitePiece(dir) : isBlackPiece(dir))
						{
							AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						else if(delta == emSq)
						{		
							AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
						}
			
						dir += bishopAttacks[i];
					}
				}
			}
			
			// rooks and queens
			if( side ? 
					(board->position[fromSq] == bR) || (board->position[fromSq] == bQ) :
					(board->position[fromSq] == wR) || (board->position[fromSq] == wQ)
			)
			
			{
				for(int i = 0; i < 4; ++i)
				{
					int dir = sq + rookAttacks[i];
		
					while(!(dir & 0x88))
					{
						int delta = board->position[dir];
			
						if(side ? isBlackPiece(dir) : isWhitePiece(dir))
							break;
								
						else if(side ? isWhitePiece(dir) : isBlackPiece(dir))
						{
							AddMove(list, SetMove(fromSq, dir, 0, 1, 0, 0, 0));
							break;
						}
			
						else if(delta == emSq)
						{		
							AddMove(list, SetMove(fromSq, dir, 0, 0, 0, 0, 0));
						}
			
						dir += rookAttacks[i];
					}
				}
			}
			
			// kings
			if(side ? board->position[fromSq] == bK : board->position[fromSq] == wK)
			{
				side ? kingSq(b) = sq : kingSq(w) = sq;
				
				for(int i = 0; i < 8; ++i)
				{
					int dir = sq + kingAttacks[i];
					int delta = board->position[dir];
		
					if(!(dir & 0x88))
					{
						if(side ? (delta == emSq || isWhitePiece(dir)) : (delta == emSq || isBlackPiece(dir)))
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

int InCheck(CHESSBOARD *board, int sideToMove)
{
	int kingSquare = noSq;
	side ? kingSquare = kingSq(b) : kingSquare = kingSq(w); 

	/*for(int sq = 0; sq < 128; ++sq)
	{
		if(sideToMove ? board->position[sq] == bK : board->position[sq] == wK)
		{
			kingSq = sq;
			break;
		}
	}*/
	
	return IsSquareAttacked(board, kingSq, sideToMove ^ 1);
} 

int MakeMove(CHESSBOARD *board, int move)
{
	CHESSBOARD boardStored[1];
	boardStored[0] = board[0];

	int fromSq = GetMoveSource(move);
	int toSq = GetMoveTarget(move);
	//int side = board->side;

	// move piece
	board->position[toSq] = board->position[fromSq];
	board->position[fromSq] = emSq;
	
	// promotions
	if(GetMovePromPiece(move))
	{
		board->position[toSq] = GetMovePromPiece(move);
		board->position[fromSq] = emSq;
	}
		
	// en passant flag
	if(GetMoveEnPassantFlag(move))
	{
		side ? 
			(board->position[enPassant + 16] = 0) :
			(board->position[enPassant - 16] = 0);
			
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
				board->position[f1] = board->position[h1];
				board->position[h1] = emSq;
				break;
				
			case c1:
				board->position[d1] = board->position[a1];
				board->position[a1] = emSq;
				break;
				
			case g8:
				board->position[f8] = board->position[h8];
				board->position[h8] = emSq;
				break;
				
			case c8:
				board->position[d8] = board->position[a8];
				board->position[a8] = emSq;
				break;
		}
	}
	
	// update castling permission
	castle &= castling[fromSq];
	castle &= castling[toSq];
	
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
 ***************** Perft ********************
 ********************************************/

long nodes;

int GetTimeMs()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec*1000 + t.tv_usec/1000;
}

void Perft(CHESSBOARD *board, int depth)
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
		
		//getchar();
		//PrintBoard(board);
		//PrintMoveList(list);
			
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
 **************** Search ********************
 ********************************************/

int NegaMaxSearch(CHESSBOARD *board, int depth)
{
	if(InCheck(board, side))
		depth++;

	if ( depth == 0 )
	{
		return 0; //EvaluatePosition(board);
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


/********************************************
 **************** Chenglite *****************
 ********************************************/

int main()
{
	CHESSBOARD board[1];
	
	ParseFen(board, trickyPos);
	
	PerftTest(board, 5);
	
		
	return 0;
}















