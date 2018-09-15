# Chenglite
Minimalist UCI chess engine written by self learner from scratch
for linux only(for now)

# Current development state
implementing PV-table

# Features

	- 0x88 board representation
	- move generation via calculations
	- copy-make make move approach
	- material and piece placement evaluation
	- iterative deepening
	- nega max search algorithm
	- fixed depth quiescence search
	- alpha beta pruning
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
	Any feedbacks are appreciated at freesoft.for.people@gmail.com
