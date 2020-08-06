# Chenglite
Simple UCI chess engine written by self learner from scratch.

Chenglite is not aimed to be a competetive engine, so please observe it
as a beginner's attempt to write a chess program and please don't ask me
why it doesn't support tons of features. It aimed to beat the only one
program - tiny chess by Oscar Toledo and it did it, nothing more for now.
The UCI protocol is very raw, so it might crash somewhere. Please keep it
all in mind while testing.

# Release

Chenglite v1.0 for linux x64 is relaesed! https://github.com/maksimKorzh/Chenglite/releases

Chenglite v1.0 for windows x64 is relaesed! https://github.com/maksimKorzh/Chenglite/releases

# Current development state
Chenglite's development is now stopped due to I've reached the point of satisfaction. The
next step is to heavily reduce the code size while keeping the speed, strength and features.

# Contribtions and forks
At this moment I leave Chenglite for the community in hope it would be useful and instructive
for beginners just like me. I'd be really happy if you fork the project for your own purposes
or start contributing to make it become whatever you want. I've create the "community branch"
for your pool request(if any).

# Features

	- 0x88 board representation
	- move generation via calculations
	- copy-make make move approach
	- material and piece placement evaluation
	- triangular PV table
	- nega max search algorithm
	- quiescence search
	- alpha beta pruning
	- move ordering(follow PV, MVV_LVA for captures, search history, killerMoves)
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
	  
	- don't hurry, chess engine development requires more accuracy
	  rather than high speed coding skills
	  
	- enjoy the process, this is the most principle thing to
	  consider
	

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
	will play with fixed depth search of 5 plies.
	
	
# Credits

	I want to thank the great chess engine development community
	for providing reasonable resources, espacially to:
		
		-   Bluefever Software's VICE video tutorial series
			https://www.youtube.com/playlist?list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg
		
		-   Bruce Moreland for his 0x88 board representaion explanations
			https://web.archive.org/web/20071026130204/http://www.seanet.com/~brucemo/topics/0x88.htm
			
		-   Chess Programming Wiki
			https://www.chessprogramming.org/Main_Page
			
		-   H.G.Muller and Marco Belli for their kind explanations of numerous
		    points related to alpha beta search and PV storing
			
			
# Feedback
	Any feedbacks are appreciated at freesoft.for.people@gmail.com
