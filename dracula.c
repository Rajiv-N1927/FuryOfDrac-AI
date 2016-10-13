// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "Map.h"
#include "DracView.h"
#include "Queue.h"

#define MAX_NO_CONN 9
#define HP_THRESHOLD 25
#define HP_SEA_THRESHOLD 14 
//FOR THE SHORTEST PATH
int isUnique(int *arr, int obj);
int shortestPath(DracView gameState, int sea, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int returnPath(int src, int dest, LocationID *pathFound, int *pathToAdd);
int checkIfInTrail(DracView gameState, LocationID myLoc);
int validateDest(Map map, LocationID *trail, LocationID dest);
//FOR THE MOVEMENT DECISION
int checkPosInNTrail(LocationID loc);

void decideDraculaMove(DracView gameState)
{
	LocationID bestPos = -1;
	PlayerID me = PLAYER_DRACULA;
	LocationID trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, me, trail);
	LocationID myPos = whereIs(gameState, me);

	int roundNo = giveMeTheRound(gameState);

	LocationID hunterOne = whereIs(gameState, PLAYER_LORD_GODALMING);
	LocationID hunterTwo = whereIs(gameState, PLAYER_DR_SEWARD);
	LocationID hunterThree = whereIs(gameState, PLAYER_VAN_HELSING);
	LocationID hunterFour = whereIs(gameState, PLAYER_MINA_HARKER);

	/* int x;

	for ( x = 0; x < TRAIL_SIZE; x++ ) {
		printf("\n[Pos]:%d->%s\n", x, idToName(trail[x]) );
	} */ 

	int curHealth = howHealthyIs(gameState, me);

	if (roundNo == 0) { // starting position 

				if (hunterOne   != ALICANTE &&
					hunterTwo   != ALICANTE &&
					hunterThree != ALICANTE &&
					hunterFour  != ALICANTE  ) {

						bestPos = ALICANTE; 

		} else if ( hunterOne   != SWANSEA &&
			 		hunterTwo   != SWANSEA &&
					hunterThree != SWANSEA &&
			 		hunterFour  != SWANSEA  ) {

						bestPos = SWANSEA; 

		} else if ( hunterOne   != GENOA &&
			 		hunterTwo   != GENOA &&
					hunterThree != GENOA &&
			 		hunterFour  != GENOA  ) {

						bestPos = GENOA; 

		} else if ( hunterOne   != BORDEAUX &&
			 		hunterTwo   != BORDEAUX &&
					hunterThree != BORDEAUX &&
			 		hunterFour  != BORDEAUX  ) {

						bestPos = BORDEAUX;  

		} else if ( hunterOne   != ZAGREB &&
			 		hunterTwo   != ZAGREB &&
					hunterThree != ZAGREB &&
			 		hunterFour  != ZAGREB  ) {

						bestPos = ZAGREB; 

		}											

	} else if (curHealth >= HP_THRESHOLD) {

		int numLocations = 0; 
		int decided = 0; 
		int moveDecision; 
		LocationID * possibleMoves;

		possibleMoves = whereCanIgo(gameState, &numLocations, TRUE, TRUE); 
		// LocationID rawPossible[numLocations]; // for very end if need to DB/HIDE
		// LocationID possibleMovesCopy[numLocations]; // includes to-be-removed locations 

		if (numLocations >= 1) { // as long as there was a possible  move 

			int moveCounter;

			LocationID *hunterOneMoves;
			LocationID *hunterTwoMoves;
			LocationID *hunterThreeMoves;
			LocationID *hunterFourMoves;

			int noHunterOneMoves, noHunterTwoMoves, noHunterThreeMoves, noHunterFourMoves;
			int counterOne, counterTwo, counterThree, counterFour; 

			hunterOneMoves = whereCanTheyGo(gameState,   &noHunterOneMoves,     PLAYER_LORD_GODALMING, TRUE, TRUE, TRUE); 
			hunterTwoMoves = whereCanTheyGo(gameState,   &noHunterTwoMoves,     PLAYER_DR_SEWARD,      TRUE, TRUE, TRUE); 
			hunterThreeMoves = whereCanTheyGo(gameState, &noHunterThreeMoves,   PLAYER_VAN_HELSING,    TRUE, TRUE, TRUE); 
			hunterFourMoves = whereCanTheyGo(gameState,  &noHunterFourMoves,    PLAYER_MINA_HARKER,    TRUE, TRUE, TRUE); 

			for (moveCounter = 0; moveCounter < numLocations; moveCounter++) { // preventing moving drac near a hunter

				for (counterOne = 0; counterOne < noHunterOneMoves; counterOne++) { 

					if (hunterOneMoves[counterOne] == possibleMoves[moveCounter]) { 

						possibleMoves[moveCounter] = 0; 

					}

				}

				for (counterTwo = 0; counterTwo < noHunterTwoMoves; counterTwo++) { 

					if (hunterTwoMoves[counterTwo] == possibleMoves[moveCounter]) { 

						possibleMoves[moveCounter] = 0; 

					}

				}

				for (counterThree = 0; counterThree < noHunterThreeMoves; counterThree++) { 

					if (hunterThreeMoves[counterThree] == possibleMoves[moveCounter]) { 

						possibleMoves[moveCounter] = 0; 

					}

				}

				for (counterFour = 0; counterFour < noHunterFourMoves; counterFour++) { 

					if (hunterFourMoves[counterFour] == possibleMoves[moveCounter]) { 

						possibleMoves[moveCounter] = 0; 

					}

				}											

			}

			for (moveDecision = 0; moveDecision < numLocations; moveDecision++) { 

				if (possibleMoves[moveDecision] != 0 && decided == 0) { 

					bestPos = possibleMoves[moveDecision];
					decided++; 
					break; 

				}

			}

			if (decided == 0) { // failed to find ideal spot where hunter can't go 

				possibleMoves = whereCanIgo(gameState, &numLocations, TRUE, TRUE);

				for (moveDecision = 0; moveDecision < numLocations; moveDecision++) { 

					if (possibleMoves[moveDecision] != 0 && decided == 0) { // this refreshed array hasn't removed hunter spots 

						bestPos = possibleMoves[moveDecision];
						decided++; 
						break; 

					}

				}			

			}

		} 

		if (decided == 0) { // by this point it means our drac can't find any move not in its trail 

			LocationID moveTrail[TRAIL_SIZE]; 
			giveMeTheMoves(gameState, PLAYER_DRACULA, moveTrail);
			int didHide = 0; 
			int didDB = 0; 

			for (moveDecision = 0; moveDecision < TRAIL_SIZE; moveDecision++) { 

				if (moveTrail[moveDecision] == HIDE) { 

					didHide++; 

				}

				if (moveTrail[moveDecision] >= DOUBLE_BACK_1 && moveTrail[moveDecision] <= DOUBLE_BACK_5) { 

					didHide++; 

				}				

			}

			if (didHide == 0) { 

				bestPos = HIDE; 

			} else if (didDB == 0) { 

				bestPos = DOUBLE_BACK_1; // can make this more sophistiated later 

			} else { 

				// no moves available... 
				bestPos = TELEPORT; 

			}


		}

	} else { // need to go through this more thoroughly 

		//GOTTA GO BACK TO THE CASTLE BOIS
		LocationID locToGo[NUM_MAP_LOCATIONS];
		// int trailLengthToGo = 0;
		if ( myPos != CASTLE_DRACULA && curHealth <= HP_THRESHOLD ) {
			// printf("EXB\n");
			if (curHealth > HP_SEA_THRESHOLD) {
				shortestPath( gameState, TRUE, CASTLE_DRACULA, locToGo);
			} else {
				shortestPath( gameState, FALSE, CASTLE_DRACULA, locToGo);
			}

			bestPos = locToGo[1];
		} else {
			// printf("EXH\n");
			//int Tlength = shortestPath( gameState, TRUE, BORDEAUX, locToGo);
			
			LocationID * possibleMoves;
			int numLocations = 0; 

			possibleMoves = whereCanIgo(gameState, &numLocations, TRUE, TRUE); 
			/* int x; 
			for ( x = 0; x < Tlength; x++ ) {
				printf("\n[Pos]:%d->%s\n", x, idToName(locToGo[x]));
			} */ 
			if (numLocations > 0) bestPos = possibleMoves[0];
		}

	}
	/* printf("=============CURRENT STATS=============\n");
	printf("\n[Registered Pos]->%s\n", idToName(bestPos));
	printf("[HEALTH]: %d\n\n---------------------------------------\n", curHealth); */ 
	if (bestPos == -1) { // this check needs work 

			LocationID moveTrail[TRAIL_SIZE]; 
			giveMeTheMoves(gameState, PLAYER_DRACULA, moveTrail);
			int moveDecision; 
			int didHide = 0; 
			int didDB = 0; 

			for (moveDecision = 0; moveDecision < TRAIL_SIZE; moveDecision++) { 

				if (moveTrail[moveDecision] == HIDE) { 

					didHide++; 

				}

				if (moveTrail[moveDecision] >= DOUBLE_BACK_1 && moveTrail[moveDecision] <= DOUBLE_BACK_5) { 

					didHide++; 

				}		

			}		
			
			if (didHide == 0) { 

				bestPos = HIDE; 

			} else if (didDB == 0) { 

				bestPos = DOUBLE_BACK_1; // can make this more sophistiated later 

			} else { 

				// no moves available... 
				bestPos = TELEPORT; 

			}

	}

	registerBestPlay(idToAbbrev(bestPos),"lol");

}
//CHECK THE HARD CODED PATH
// int locPos(int *arr, LocationID obj) {
// 		int x;
// 		for ( x = 0; arr[x] != obj; x++ ) {
// 			if ( x >= NUM_MAP_LOCATIONS ) return 0;
// 		}
// 		return x;
// }

int checkPosInNTrail(LocationID loc) {
	int check;
	LocationID nTrail[] = {UNKNOWN_LOCATION, MARSEILLES, MEDITERRANEAN_SEA,
		ALICANTE, CLERMONT_FERRAND, SARAGOSSA, BORDEAUX, TOULOUSE};
	for (check = 0; check < 7; check++) {
		if ( nTrail[check] == loc ) return TRUE;
	}
	return FALSE;
}
int validateDest(Map map, LocationID *trail, LocationID dest) {
	if ( onTrail(trail, dest) ) {
		while( onTrail(trail, dest) ) {
			int test = 0; LocationID *check; int *x = &test;
			check = reachableLocations(map, x, dest, FALSE, 0, TRUE, TRUE);
			int col;
			for ( col = 0; col < *x; col++ ) {
				if ( !onTrail(trail, check[col]) ) dest = check[col]; break;
			}
		}
	}
	return dest;
}
//SHORTEST PATH
int isUnique(int *arr, int obj) {
	if (arr[obj] != -1 ) {
		//printf("%d\n", arr[obj]);
		return FALSE;
	}
	else {
		return TRUE;
	}
}


//INCLUDES BOTH ROAD AND SEA CONNS
int shortestPath(DracView gameState, int sea, int dest, int *path) {
	//Intialise Everything needed i.e. Map, the curr Pos, and the trail
	Map map = newMap();
	int src = whereIs(gameState, PLAYER_DRACULA);
	int found = FALSE;
	QHead dracQ = initQ();
	addQ(dracQ, src);
	LocationID vex[NUM_MAP_LOCATIONS], trail[TRAIL_SIZE]; //The set for vetex
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail); //Get Dracs trail
	//Check the destination is not in the path
	dest = validateDest(map, trail, dest);
	//Initialise the vertex arrays
	int i;
	for( i = 0; i < NUM_MAP_LOCATIONS; i++ ) {
		vex[i] = -1;
	}
	//The src point to itself
	vex[src] = src;
	int toSearch = -1, col = -1;
  while ( vex[dest] == -1 && QSize(dracQ) != 0 ) {
		toSearch = deQ(dracQ);
    //FIND THE REACHABLE LOCATIONS
		int test = 0; int *x = &test;
		LocationID *check;
    check = reachableLocations(map, x, toSearch, TRUE, 0, TRUE, sea);
    //Loop through the check array to get locations
    for ( col = 0; col < *x; col++ ) {
			//Need to work on this
			if( isUnique(vex, check[col]) == FALSE ) continue;
			if ( !onTrail(trail, check[col]) ) {
				vex[check[col]] = toSearch;
				addQ(dracQ, check[col]);
    		if( vex[dest] != -1 ) break;
			}
		}

  } if ( QSize(dracQ) != 0 ) { //If not path has been found
		found = returnPath(src, dest, vex, path);
	}
	free(map);
	disposeQ(dracQ);
	return found;
}

int sizePath( int src, int dest, LocationID *pathFound ) {
	int i, j;
	for( i = pathFound[dest], j = 0; i != src; i = pathFound[i], j++ );
	return j;
}

int returnPath(int src, int dest, LocationID *pathFound, int *pathToAdd) {
	int x = sizePath(src, dest, pathFound), i, j;
	pathToAdd[0] = src;
	pathToAdd[x+1] = dest;
	for( i = pathFound[dest], j = x; i != src; i = pathFound[i], j-- ) {
		pathToAdd[j] = i;
	}
	return x+2;
}
