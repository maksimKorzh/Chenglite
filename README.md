# Chenglite
Minimalist UCI chess engine written by self learner from scratch




# Features

	- 0x88 board representation
	- move generation via calculations
	- copy-make make move approach
	- material and piece placement evaluation
	- nega max search algorithm
	- fixed depth quiescence search
	- alpha beta prunning
	- UCI protocol with depth search only


# About

	Chenglite is the successor of Chengine - my very first chess
	engine available at https://github.com/maksimKorzh/chengine.
	
	The code has been heavily rewritten. Many functions turned
	into macros. I got rid of seperate files to improve the 
	performance and make most functions declared static as well
	as inline. The game performance has been improved a lot by
	adding a quiescence search to reduce the horizon effect and
	stop engine giving pieces away.	


# Intents and purposes

	Chenglite is intended to be a source of inspiration for the
	beginners and hobby programmers dreaming to write their own
	bug free chess programs. The problem with my previous very
	first successful attempt to write a chess engine - Chengine -
	was that I couldn't make myself keep maintaining it any more due
	to how the code was orginized. Now when all the code has been
	cleaned up and slightly encapsulated via macros I feel the
	great desire to keep the developing going on and on. At first
	it wasn't that easy to take the idea that your engine is going
	to be pretty weak and slow due to the original design reasons,
	but diving straight into the developing process made me understand
	the true reason why am I doing this - the idea is very simple -
	trying to get the most out of the given design.
	
	
# Achievements
	
	[Event "Computer chess game"]
	[Site "maksim-System-Product-Name"]
	[Date "2018.09.09"]
	[Round "?"]
	[White "Chenglite"]
	[Black "toledo"]
	[Result "1-0"]
	[BlackElo "2000"]
	[ECO "D01"]
	[Opening "Richter-Veresov Attack"]
	[Time "18:13:23"]
	[WhiteElo "2000"]
	[Termination "normal"]
	[PlyCount "73"]
	[WhiteType "program"]
	[BlackType "program"]

	1. d4 {+0.30/3 0} d5 {(d7d5) +0.00/6 1} 2. Nc3 {+0.20/3 0} e6 {(e7e6)
	-0.00/6 2} 3. Nf3 {+0.30/3 0} Nf6 {(g8f6) +0.00/6 3} 4. Bf4 {+0.25/3 0} c5
	{(c7c5) +0.00/6 3} 5. e3 {+0.45/3 1} Nc6 {(b8c6) +0.00/6 3} 6. Bd3 {+0.45/3
	3} Qb6 {(d8b6) +0.01/6 5} 7. Rb1 {+0.40/3 4} cxd4 {(c5d4) +0.09/6 4} 8.
	exd4 {-0.35/3 3} Bb4 {(f8b4) +0.00/6 18} 9. a3 {+0.30/3 4} Bxc3+ {(b4c3)
	-0.00/6 10} 10. bxc3 {+0.65/3 0} Qa5 {(b6a5) -0.00/6 12} 11. Rb3 {+0.55/3
	3} Bd7 {(c8d7) -0.00/6 9} 12. O-O {+0.60/3 5} b6 {(b7b6) -0.00/6 7} 13. Ne5
	{+1.00/3 5} Rc8 {(a8c8) -0.00/6 6} 14. Nxd7 {+1.50/3 9} Kxd7 {(e8d7)
	+0.29/6 3} 15. Qa1 {+1.45/3 4} Rhg8 {(h8g8) -0.00/6 7} 16. Rd1 {+1.50/3 4}
	a6 {(a7a6) -0.00/6 5} 17. Qb2 {+1.55/3 4} g5 {(g7g5) -0.00/6 5} 18. Be3
	{+1.60/3 6} b5 {(b6b5) -0.01/6 4} 19. Qc1 {+1.65/3 3} h6 {(h7h6) -0.01/6 6}
	20. f4 {+1.70/3 3} gxf4 {(g5f4) -0.00/6 4} 21. Bxf4 {+1.75/3 2} Qa4 {(a5a4)
	+0.01/6 14} 22. Bxh6 {+2.60/3 10} Na5 {(c6a5) +0.08/6 16} 23. Rb4 {+5.65/3
	5} Nb3 {(a5b3) -0.23/6 5} 24. cxb3 {+5.85/3 6} Qa5 {(a4a5) -0.01/6 3} 25.
	Rf1 {+5.80/3 3} Nh5 {(f6h5) -0.10/6 21} 26. Rxf7+ {+6.80/3 8} Ke8 {(d7e8)
	-0.00/6 4} 27. Ra7 {+6.90/3 8} Rb8 {(c8b8) -0.01/6 6} 28. Qd1 {+7.05/3 5}
	Nf6 {(h5f6) -0.01/6 11} 29. Bf4 {+7.05/3 4} Rc8 {(b8c8) -0.10/6 5} 30.
	Bxb5+ {+9.15/3 5} Qxb5 {(a5b5) -0.14/6 6} 31. Rxb5 {+9.15/3 2} axb5 {(a6b5)
	+0.40/6 6} 32. Qe2 {+9.90/3 0} Ne4 {(f6e4) -0.28/6 4} 33. Qxb5+ {+14.70/3
	1} Kf8 {(e8f8) -0.50/6 4} 34. Bh6+ {+14.80/3 1} Rg7 {(g8g7) -0.79/6 0} 35.
	Qd7 {+17.90/3 1} Rc7 {(c8c7) -74.40/6 1} 36. Ra8+ {+M500/3 0} Rc8 {(c7c8)
	-76.45/6 0} 37. Rxc8# {+M500/3 1} 1-0
	
	[Event "Computer chess game"]
	[Site "maksim-System-Product-Name"]
	[Date "2018.09.09"]
	[Round "?"]
	[White "toledo"]
	[Black "Chenglite"]
	[Result "1/2-1/2"]
	[BlackElo "2000"]
	[ECO "A20"]
	[Opening "English"]
	[Time "18:21:23"]
	[Variation "KID, 1...e5"]
	[WhiteElo "2000"]
	[Termination "normal"]
	[PlyCount "80"]
	[WhiteType "program"]
	[BlackType "program"]

	1. c4 {(c2c4) +0.00/6 1} e5 {+0.15/3 0} 2. d4 {(d2d4) -0.00/6 0} Nc6
	{+0.20/3 0} 3. dxe5 {(d4e5) -0.00/6 0} Nxe5 {+0.20/3 0} 4. e4 {(e2e4)
	-0.00/6 5} Bd6 {+0.10/3 0} 5. f4 {(f2f4) -0.01/6 4} Nc6 {-0.10/3 1} 6. e5
	{(e4e5) -0.00/6 5} Bc5 {+0.05/3 1} 7. Nh3 {(g1h3) +0.00/6 9} Nge7 {+0.35/3
	1} 8. a3 {(a2a3) +0.00/6 7} O-O {+0.55/3 1} 9. b4 {(b2b4) -0.01/6 4} Bd4
	{+1.00/3 1} 10. Ra2 {(a1a2) +0.01/6 4} d5 {+0.85/3 1} 11. Qf3 {(d1f3)
	+0.01/6 8} Bf5 {+1.55/3 5} 12. Nd2 {(b1d2) -0.01/6 8} a6 {+1.10/3 5} 13. g4
	{(g2g4) -0.01/6 4} Be6 {+1.20/3 7} 14. f5 {(f4f5) +0.00/6 4} Bd7 {+1.20/3
	11} 15. cxd5 {(c4d5) -0.01/6 5} Nxe5 {+1.10/3 7} 16. Qe4 {(f3e4) -0.00/6
	12} Bc3 {+0.90/3 8} 17. Bb2 {(c1b2) +0.00/6 12} Bxb2 {+0.90/3 4} 18. Rxb2
	{(a2b2) -0.28/6 3} f6 {+0.75/3 5} 19. Be2 {(f1e2) +0.01/6 8} c6 {+0.75/3 3}
	20. d6 {(d5d6) +0.00/6 3} Nc8 {+0.55/3 10} 21. Qd4 {(e4d4) +0.01/6 6} h5
	{+0.55/3 3} 22. gxh5 {(g4h5) +0.01/6 5} Bxf5 {+0.95/3 3} 23. d7 {(d6d7)
	+0.09/6 14} Nb6 {+1.70/3 9} 24. Nf4 {(h3f4) +0.09/6 18} Nbxd7 {+1.30/3 6}
	25. Rg1 {(h1g1) -0.00/6 21} Nb6 {+1.70/3 6} 26. Rxg7+ {(g1g7) -0.04/6 33}
	Kxg7 {+5.40/3 0} 27. Qc5 {(d4c5) +0.00/6 20} Nd5 {+5.70/3 6} 28. Nxd5
	{(f4d5) -0.07/6 17} cxd5 {+5.70/3 9} 29. b5 {(b4b5) -0.01/6 13} Nd3+
	{+6.05/3 6} 30. Bxd3 {(e2d3) -0.10/6 5} Bxd3 {+6.05/3 5} 31. bxa6 {(b5a6)
	+0.01/6 6} Bxa6 {+5.90/3 2} 32. Qe3 {(c5e3) +0.01/6 8} Re8 {+9.05/3 0} 33.
	Ne4 {(d2e4) +0.22/6 11} Rxe4 {+13.45/3 1} 34. Rg2+ {(b2g2) +0.36/6 15} Kf8
	{+13.65/3 0} 35. Re2 {(g2e2) +0.37/6 12} Bxe2 {+14.45/3 0} 36. Qh6+ {(e3h6)
	+0.01/6 4} Kg8 {+14.70/3 0} 37. Qg6+ {(h6g6) -0.49/6 0} Kh8 {+14.45/3 0}
	38. Qh6+ {(g6h6) -0.29/6 2} Kg8 {+14.70/3 0} 39. Qg6+ {(h6g6) -0.49/6 1}
	Kh8 {+14.45/3 0} 40. Qh6+ {(g6h6) -0.29/6 2} Kg8 {+14.70/3 0 3-fold
	repetition} 1/2-1/2


# My chess engine development philosophy
	
	- try each time to do the same thing you've done before,
	  but in less code
	
	- try to get the best performance 
	  out of a given algorithms
	  
	- don't add new features until being sure you've got everything out
	  of the previously added ones
	  
	- don't try to write the strongest chess engine in the world,
	  remember that Stockfish and Leela chess has been developed
	  by numerous top level programmers
	  
	- don't hurry, chess engine development requires more accuracy
	  rather than high speed coding skills
	  
	- enjoy the process, this is the most principle thing to
	  consider


# Forks and contribution

	At the moment Chenglite looks like now more like a chess 
	engine template rather than a complete program, so I would
	be really happy if you fork the project to use it as a basis
	for your own engine, in case if you're interested in writing
	only the AI routines.
	
	I've also created a "community edition" branch for those who 
	wants to contribute to the project, so please use it for pool
	requests. I'd like Chenglite to become a community project, so
	everybody who wants to join is welcome, no matter whether you're
	professional developer or just another hobby programmer like me.
	
	
# Release

	Chenglite v1.0 is relaesed! https://github.com/maksimKorzh/Chenglite/releases


# How to compile

	Compile it with GCC via "gcc -Ofast Chenglite -o Chenglite" to get
	the most performance. Or use the Makefile.
	
	On linux:
		- git clone https://github.com/maksimKorzh/Chenglite.git 
		- cd into src folder
		- type "make"
		
	The output file would be "Chenglite" with no extension. Enjoy!
	
	
# GUI

	Tested with Arena for linux.
	
	Use fixed depth search to determine playing strength
	On any other but fixed depth search mode Chenglite
	will play with fixed depth search of 4 plies with
	fixed 4 plies quiescence search, which takes the
	engine quite a lot of time to make a move.
	
	
# Credits

	I want to thank the great chess engine development community
	for providing reasonable resources, espacially to:
		
		-   Bluefever Software's VICE video tutorial series
			https://www.youtube.com/playlist?list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg
		
		-   Bruce Moreland for his 0x88 board representaion explanations
			https://web.archive.org/web/20071026130204/http://www.seanet.com/~brucemo/topics/0x88.htm
			
		-   Chess Programming Wiki
			https://www.chessprogramming.org/Main_Page
			
		-   Oscar Toledo for providing the same class engine to play with
		    http://www.nanochess.org/chess3.html
			
			
# Feedback
	Any feedbacks are appriciated at freesoft.for.people@gmail.com
