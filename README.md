# chess-engine
Simple Chess Engine

CHEST v 1.1 released as of January 9th, 2019.

Developed by Ian Thomas

*************CHEST v 1.1*********************

Description:
 - CHEST is a simple chess engine built from an
   AlphaBeta search algorithm with an evaluation
   based on piece square tables and pawn bitboards. 
   The search algorithm makes use of iterative 
   deepening to make the most use of the time 
   given to search as well as implements a 
   quiescense search to eliminate the horizon effect, 
   which is the scenario in which an engine searches to a
   specific depth, neglecting a possible capture
   by the other side immediately afterwards.
   Version 1.1 makes use of null move pruning in
   which if a null move by the engine's side results
   in a beta cutoff that the opposing side cannot
   improve on, this node simply returns beta and
   that node is effectively terminated.
   Currently, version 1.1 has no opening book
   or advanced endgame structure, which are currently
   its biggest weaknesses. Version 1.1 does not
   implement a hash table for multiple principal
   variations but only employs an array of 
   principal variations.

Features:
	- Recognizes both UCI and WinBoard protocols
	- no hash table
	- for list of WinBoard functionality type
	  xboard --> protover
	  in the console
	- recognizes 50 move rule and 3 move repetition
	- Console mode - play against CHEST v 1.1 in
	  the console with full functionality
