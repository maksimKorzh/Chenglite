# Chenglite
# Chengine
Beginner's UCI chess engine for linux

	Features:

		- 0x88 board representation;
		- move generation via calculations
		- copy-make make move approach
		- material and piece placement evaluation;
		- nega max brute force search algorithm;
		- fixed depth quiescence search;
		- UCI protocol with depth search only;
		
	Chenglite is the successor of Chengine(my very first engine).
	The code has been heavily rewritten. Many functions turned
	into macros. I got rid of seperate file to improve the performance
	and make most functions declared static as well as inline
	The search has been improved a lot by adding a quiescence search
	to reduce the horizon effect and stop engine giving pieces.
	
	

# Intents and purposes

	Chenglite is intended to be a source of inspiration for the
	beginners and hobby programmers dreaming to write their own
	bug free chess programs. The problem with my previous very
	first successful attempt to write a chess engine - Chengine -
	was that I couldn't make myself to keep maintaining it due
	to how the code was orginized. Now when all the code has been
	cleaned up and slightly encapsulated via macros I feel the
	great desire to keep the developing going on and on. At first
	it wasn't that easy to take the idea that your engine is going
	to be pretty weak and slow due to the original design reasons,
	but diving straight into the developing process made me understand
	the true reason why am I doing this - the idea is very simple -
	trying to get the most out of the given design.
	
# Achievements
	
	Chenglite has finally beat Oscar Toledo's winboard tiny chess:
	
	This is the PGN file:
	
	[Event "Computer chess game"]
	[Site "maksim-System-Product-Name"]
	[Date "2018.09.07"]
	[Round "?"]
	[White "toledo"]
	[Black "Chenglite"]
	[Result "0-1"]
	[BlackElo "2000"]
	[ECO "A20"]
	[Opening "English"]
	[Time "15:33:51"]
	[Variation "KID, 1...e5"]
	[WhiteElo "2000"]
	[Termination "normal"]
	[PlyCount "76"]
	[WhiteType "program"]
	[BlackType "program"]

	1. c4 {(c2c4) +0.00/6 1} e5 {+0.20/1} 2. d4 {(d2d4) -0.00/6 1} Bd6 {+0.15/1
	0} 3. c5 {(c4c5) -0.01/6 1} Be7 {+0.15/1} 4. dxe5 {(d4e5) -0.01/6 1} Bxc5
	{+0.15/1 0} 5. Be3 {(c1e3) -0.01/6 8} d6 {+0.05/1 0} 6. exd6 {(e5d6)
	-0.07/6 4} cxd6 {+0.15/1 0} 7. Nd2 {(b1d2) -0.01/6 7} Be6 {+0.25/1 0} 8.
	Qc2 {(d1c2) -0.01/6 6} Nd7 {+0.40/1 0} 9. Qc3 {(c2c3) -0.00/6 13} Ngf6
	{+0.60/1 0} 10. Ngf3 {(g1f3) -0.00/6 17} O-O {+0.85/1 0} 11. Bxc5 {(e3c5)
	+0.00/6 13} dxc5 {+0.90/1 0} 12. e4 {(e2e4) -0.00/6 9} Rc8 {+0.65/1 0} 13.
	Bb5 {(f1b5) -0.00/6 14} Re8 {+0.50/1 0} 14. a4 {(a2a4) -0.00/6 15} Bd5
	{+0.65/1 0} 15. Bxd7 {(b5d7) -0.01/6 8} Qxd7 {+1.20/1 0} 16. e5 {(e4e5)
	+0.02/6 9} Ng4 {+1.30/1 0} 17. h3 {(h2h3) +0.00/6 12} Nf6 {+1.35/1 0} 18.
	O-O {(e1g1) -0.00/6 17} Ne4 {+0.85/1 0} 19. Nxe4 {(d2e4) -0.01/6 10} Bxe4
	{+0.85/1 0} 20. Nd2 {(f3d2) +0.00/6 18} Bd5 {+0.90/1 0} 21. Rfc1 {(f1c1)
	-0.00/6 17} Qe6 {+0.90/1 0} 22. f4 {(f2f4) -0.01/6 17} Qd7 {+0.80/1 0} 23.
	g4 {(g2g4) -0.00/6 12} Qe6 {+0.90/1 0} 24. Rab1 {(a1b1) +0.00/6 11} Qd7
	{+0.90/1 0} 25. Ra1 {(b1a1) +0.01/6 7} Qe6 {+0.90/1 0} 26. b4 {(b2b4)
	+0.00/6 11} f6 {+1.00/1 0} 27. bxc5 {(b4c5) -0.08/6 6} fxe5 {+1.05/1 0} 28.
	fxe5 {(f4e5) +0.00/6 8} Qxe5 {+1.05/1 0} 29. Ra3 {(a1a3) +0.02/6 23} Rcd8
	{+1.10/1 0} 30. Qb4 {(c3b4) +0.02/6 30} Qc7 {+1.10/1 0} 31. Nc4 {(d2c4)
	+0.02/6 26} Re2 {+1.20/1 0} 32. Nd6 {(c4d6) +0.31/6 28} Rg2+ {+4.35/1 0}
	33. Kf1 {(g1f1) +0.31/6 27} Rxd6 {+4.35/1 0} 34. Rd3 {(a3d3) +0.02/6 18}
	Qf7+ {+4.60/1 0} 35. Qf4 {(b4f4) +1.41/6 19} Qxf4+ {+19.80/1 0} 36. Ke1
	{(f1e1) +74.26/6 8} Qxc1+ {+20.95/1 0} 37. Rd1 {(d3d1) +74.75/6 0} Re6+
	{+25.10/1 0} 38. Kf1 {(e1f1) +76.80/6 1} Qf4# {+M500/1 0} 0-1
	
	[Event "Computer chess game"]
	[Site "maksim-System-Product-Name"]
	[Date "2018.09.07"]
	[Round "?"]
	[White "Chenglite"]
	[Black "toledo"]
	[Result "1-0"]
	[BlackElo "2000"]
	[ECO "D01"]
	[Opening "Richter-Veresov Attack"]
	[Time "17:39:02"]
	[WhiteElo "2000"]
	[Termination "normal"]
	[PlyCount "105"]
	[WhiteType "program"]
	[BlackType "program"]

	1. d4 {+0.25/3 0} d5 {(d7d5) +0.00/6 1} 2. Nc3 {+0.20/3 0} e6 {(e7e6)
	-0.00/6 1} 3. Nf3 {+0.30/3 1} Nf6 {(g8f6) +0.00/6 1} 4. Bf4 {+0.20/3 1} c5
	{(c7c5) +0.00/6 2} 5. e3 {+0.40/3 9} Nc6 {(b8c6) +0.00/6 1} 6. Bd3 {+0.40/3
	25} Qb6 {(d8b6) +0.01/6 3} 7. Rb1 {+0.35/3 78} cxd4 {(c5d4) +0.09/6 2} 8.
	exd4 {-0.40/3 88} Bb4 {(f8b4) +0.00/6 9} 9. a3 {+0.30/3 49} Bxc3+ {(b4c3)
	-0.00/6 5} 10. bxc3 {+0.60/3 8} Qa5 {(b6a5) -0.00/6 6} 11. Rb3 {+0.50/3 40}
	Bd7 {(c8d7) -0.00/6 5} 12. O-O {+0.55/3 48} b6 {(b7b6) -0.00/6 4} 13. Ne5
	{+1.00/3 43} Rc8 {(a8c8) -0.00/6 3} 14. Nxd7 {+1.45/3 100} Kxd7 {(e8d7)
	+0.29/6 1} 15. Qa1 {+1.40/3 49} Rhg8 {(h8g8) -0.00/6 3} 16. Rd1 {+1.45/3
	39} a6 {(a7a6) -0.00/6 2} 17. Qb2 {+1.50/3 51} g5 {(g7g5) -0.00/6 2} 18.
	Be3 {+1.60/3 80} b5 {(b6b5) -0.01/6 2} 19. Qc1 {+1.65/3 64} h6 {(h7h6)
	-0.01/6 3} 20. f4 {+1.65/3 48} gxf4 {(g5f4) -0.00/6 2} 21. Bxf4 {+1.70/3
	78} Qa4 {(a5a4) +0.01/6 8} 22. Bxh6 {+2.60/3 112} Na5 {(c6a5) +0.08/6 9}
	23. Rb4 {+5.65/3 90} Nb3 {(a5b3) -0.23/6 3} 24. cxb3 {+5.80/3 172} Qa5
	{(a4a5) -0.01/6 2} 25. Qf4 {+5.75/3 57} Ng4 {(f6g4) -0.10/6 9} 26. Qxf7+
	{+7.10/3 143} Kd6 {(d7d6) -0.12/6 1} 27. Bf4+ {+7.75/3 170} e5 {(e6e5)
	-0.11/6 1} 28. dxe5+ {+8.80/3 354} Kc6 {(d6c6) -0.14/6 2} 29. Qe6+ {+8.80/3
	223} Kb7 {(c6b7) -0.12/6 1} 30. h3 {+9.55/3 346} Nxe5 {(g4e5) -0.21/6 7}
	31. Bxe5 {+9.55/3 444} Qb6+ {(a5b6) -0.01/6 6} 32. Qxb6+ {+9.75/3 8} Kxb6
	{(b7b6) +0.83/6 1} 33. c4 {+9.80/3 9} dxc4 {(d5c4) -0.00/6 1} 34. bxc4
	{+9.95/3 11} Rce8 {(c8e8) -0.01/6 2} 35. Bd4+ {+10.85/3 5} Ka5 {(b6a5)
	-0.01/6 0} 36. cxb5 {+10.85/3 3} axb5 {(a6b5) +0.09/6 2} 37. Bxb5 {+10.85/3
	1} Re6 {(e8e6) -0.00/6 1} 38. Bd7 {+14.65/3 1} Rxg2+ {(g8g2) -0.40/6 1} 39.
	Kxg2 {+14.70/3 0} Rg6+ {(e6g6) -0.00/6 1} 40. Bg4 {+14.95/3 0} Rg5 {(g6g5)
	-0.00/6 1} 41. Kg1 {+15.10/3 0} Ka6 {(a5a6) -0.00/6 0} 42. Rb6+ {+15.20/3
	0} Ka5 {(a6a5) -0.00/6 0} 43. Rb7 {+15.25/3 0} Ka4 {(a5a4) -0.00/6 0} 44.
	Ra7+ {+15.30/3 0} Kb3 {(a4b3) -0.00/6 1} 45. Bf6 {+15.30/3 0} Rc5 {(g5c5)
	-0.01/6 1} 46. Be6+ {+15.35/3 0} Kc2 {(b3c2) -0.49/6 1} 47. Rad7 {+20.50/3
	0} Rc4 {(c5c4) -0.49/6 1} 48. Be5 {+20.55/3 0} Kb3 {(c2b3) -0.49/6 1} 49.
	R1d3+ {+20.60/3 0} Ka4 {(b3a4) -0.49/6 0} 50. Bxc4 {+20.65/3 0} Ka5 {(a4a5)
	-74.65/6 0} 51. R3d5+ {+M500/3 0} Kb6 {(a5b6) -74.46/6 0} 52. Bb5 {+M500/3
	0} Ka5 {(b6a5) -76.50/6 0} 53. Bc7# {+M500/3 0} 1-0

	
# Chess engine development philosophy
	
	- try each time to do the same thing you've done before,
	  but in less code
	
	- try to get the best performance 
	  out of a given algorithms
	  
	- don't add new features until sure you got everything out
	  of the previously added
	  
	- don't try to write the strongest chess engine in the world,
	  remember that Stockfish and Leela chess has been developed
	  by numerous top level programmers
	  
	- don't hurry, chess engine development requires more accuracy
	  rather than high speed coding skills
	  
	- enjoy the process, this is the most principle thing to
	  consider.

# How to compile

	Compile it with GCC via "gcc -Ofast Chenglite -o Chenglite" to get
	the most performance. 
	- cd into src folder
	- type "make" and that's it
	
	the output file would be "chengine"
	with no extension
	
# GUI

	Tested with Arena for linux
	
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
	I appriciate any feedbacks at freesoft.for.people@gmail.com
