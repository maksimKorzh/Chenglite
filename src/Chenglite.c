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

int pieceChars[67] = {
	[32] = wP, [30] = wN, [18] = wB, [34] = wR, [33] = wQ, [27] = wK,
	[64] = bP, [62] = bN, [50] = bB, [66] = bR, [65] = bQ, [59] = bK
};

const int pawnAttacks[4] = {15, 17, -15, -17};
const int knightAttacks[8] = {31, 33, 14, 18, -31, -33, -14, -18};
const int kingAttacks[8] = {1, 15, 16, 17, -1, -15, -16, -17};
const int bishopAttacks[4] = {15, 17, -15, -17};
const int rookAttacks[4] = {1, 16, -1, -16};


typedef struct { int move; int score; } MOVE;
typedef struct { MOVE moves[256]; int count; } MOVELIST;
typedef struct { int position[128]; int side; int enPassant; int castle; } CHESSBOARD;


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

// Quick board access
#define pos(sq)		board->position[sq]
#define side		board->side
#define enPassant	board->enPassant
#define castle		board->castle

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


/********************************************
 ************* Move generator ***************
 ********************************************/

static int IsSquareAttacked(CHESSBOARD *board, int sq, int attSide)
{
	// by pawns
	if(!attSide)
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
			if(side ? delta == bN : delta == wN)
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
			
			if(side ? (delta == bB) || (delta == bQ) : (delta == wB) || (delta == wQ))
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
			
			if(side ? (delta == bR) || (delta == bQ) : (delta == wR) || (delta == wQ))
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
			if(side ? delta == bK : delta == wK)
				return 1;
		}
	}

	return 0;
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
	
	ParseFen(board, initPos);
	PrintBoard(board);
	
	
	
	return 0;
}















