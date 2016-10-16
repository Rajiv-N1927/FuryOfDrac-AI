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
#define SEA_THRESH 18
//FOR THE SHORTEST PATH
int isUnique(int *arr, int obj);
int shortestPath(DracView gameState, int sea, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int returnPath(int src, int dest, LocationID *pathFound, int *pathToAdd);
int checkIfInTrail(DracView gameState, LocationID myLoc);
int validateDest(DracView gameState, LocationID *trail, LocationID dest, int sea);
//Hunter getter functions
int HuntInPath(DracView gameState, LocationID loc);
int HuntHistory(DracView gameState, LocationID loc);
//FOR THE MOVEMENT DECISION
int checkPosInNTrail(LocationID loc);
//If cannot find a good path check for Hides and double backs
int chooseDB(DracView gameState, LocationID posChoice, LocationID *locs);

void decideDraculaMove(DracView gameState)
{
	LocationID bestPos = -1;
	PlayerID me = PLAYER_DRACULA;
	LocationID trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, me, trail);
	LocationID myPos = whereIs(gameState, me);
	for ( int x = 0; x < TRAIL_SIZE; x++ ) {
		printf("\n[Pos]:%d->%s\n", x, idToName(trail[x]) );
	}
	int curHealth = howHealthyIs(gameState, me);

	if( curHealth >= HP_THRESHOLD && checkPosInNTrail(myPos) ) {

		switch(myPos) {
			case UNKNOWN_LOCATION:
	      bestPos = MARSEILLES;
	      break;

			case MARSEILLES:
				bestPos = MEDITERRANEAN_SEA; //1
				break;

			case MEDITERRANEAN_SEA:
				bestPos = ALICANTE; //2
				break;

			case ALICANTE:
				bestPos = SARAGOSSA; //3
				break;

			case SARAGOSSA:
				bestPos = BORDEAUX; //4
				break;

			case BORDEAUX:
				bestPos = CLERMONT_FERRAND ; //5
				break;

			case CLERMONT_FERRAND:
				bestPos = TOULOUSE;
				break;

			case TOULOUSE:
				bestPos = MARSEILLES; //6
				break;

		  default :
		    //bestPos = // shortestPath back to BORDEAUX;
		    break;
		}

	} else {

		//GOTTA GO BACK TO THE CASTLE BOIS
		LocationID locToGo[NUM_MAP_LOCATIONS];
		/*If current health is greater than 75% of the threshold health then
		use the sea or else don't*/
		int sea = ((curHealth > SEA_THRESH && !isSea(myPos)) ? TRUE : FALSE);
		// int trailLengthToGo = 0;
		if ( myPos != CASTLE_DRACULA && curHealth <= HP_THRESHOLD ) {
			printf("Shortest Path back to CASTLE_DRACULA\n");
			int Tlength = shortestPath( gameState, sea, CASTLE_DRACULA, locToGo);
			if ( Tlength > 0 ) {
				for ( int x = 0; x < Tlength; x++ ) {
					printf("\n[Pos]:%d->%s\n", x, idToName(locToGo[x]));
				}
				bestPos = locToGo[1];
			} else bestPos = myPos;
		} else {
			printf("\nShortest Path back to BORDEAUX\n");
			printf("Checking sea [%s]\n", (sea == 1 ? "yes" : "no"));
			int Tlength = shortestPath( gameState, sea, BORDEAUX, locToGo);
			if ( Tlength > 0 ) {
				for ( int x = 0; x < Tlength; x++ ) {
					printf("\n[Pos]:%d->%s\n", x, idToName(locToGo[x]));
				}
				bestPos = locToGo[1];
			} else bestPos = myPos;
		}

	}
	printf("=============CURRENT STATS=============\n");
	printf("\n[Registered Pos]->%s"":%s""\n", idToName(bestPos), idToAbbrev(bestPos));
	printf("[HEALTH]: %d\n\
[THRESH]: %d\n---------------------------------------\n", curHealth, SEA_THRESH);
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
//CHECKING IF IN THE HARD CODED POSITIONS
int checkPosInNTrail(LocationID loc) {
	int check;
	LocationID nTrail[] = {UNKNOWN_LOCATION, MARSEILLES, MEDITERRANEAN_SEA,
		ALICANTE, CLERMONT_FERRAND, SARAGOSSA, BORDEAUX, TOULOUSE};
	for (check = 0; check < 7; check++) {
		if ( nTrail[check] == loc ) return TRUE;
	}
	return FALSE;
}
//IF WE HAVE TO DOUBLE BACK OR HIDE
//Need to think about this function thoroughly
/*Make sure there is no hunter on the trail and check if trail already has
Double backs and hides*/
int chooseDB(DracView gameState, LocationID posChoice, LocationID *locs) {
	int numLocs = 0, pos;
	LocationID trail[TRAIL_SIZE];
	giveMeTheTrail(gameState, PLAYER_DRACULA, trail);
	for ( pos = 0; pos < TRAIL_SIZE; pos++ ) {
		if ( HuntInPath(gameState, posChoice) == FALSE ) {
			locs[numLocs] = posChoice;
			numLocs++;
		}
	}
	return numLocs;
}
//MAKE SURE THE DESTINATION IS VALID
//Additional things to worry about, hunter positions and if not sea
int validateDest(DracView gameState, LocationID *trail, LocationID dest, int sea ) {
	Map map = newMap();
	if ( onTrail(trail, dest) ) {
		int test; LocationID *check;
		check = reachableLocations(map, &test, dest, FALSE, 0, TRUE, TRUE);
		int col;
		while( onTrail(trail, dest) ) {
			for ( col = 0; col < test; col++ ) {
				if ( !sea && isSea(check[col])) continue;
				if ( !onTrail(trail, check[col]) && dest != check[col]) {
					dest = check[col];
					break;
				}
			}
		}
	}
	free(map);
	return dest;
}
//MAKE SURE THE HUNTER IS NOT IN THE PATH CHOSEN
/*int onWhoseTrail( DracView currentView, LocationID *trail,
    LocationID loc, PlayerID player );*/
int HuntHistory(DracView gameState, LocationID loc) {
	int chosenP, found = FALSE; LocationID ids[TRAIL_SIZE];
	giveMeTheTrail(gameState, 3, ids);
	for ( chosenP = 0; chosenP < NUM_PLAYERS - 1; chosenP++ ) {
		found = onWhoseTrail( gameState, loc, chosenP );
		if ( found == TRUE ) {
			return TRUE;
		}
	}
	return FALSE;
}

int HuntInPath(DracView gameState, LocationID loc) {
	LocationID start, end; int pID;
	for ( pID = 0; pID < NUM_PLAYERS; pID++ ) {
		lastMove(gameState, pID, &start, &end);
		if ( start == loc || end == loc ) return TRUE;
	}
	return FALSE;
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

int shortestPath(DracView gameState, int sea, int dest, int *path) {
	//Intialise Everything needed i.e. Map, the curr Pos, and the trail
	Map map = newMap();
	int src = whereIs(gameState, PLAYER_DRACULA);
	int found = FALSE;
	QHead dracQ = initQ();
	addQ(dracQ, src);
	//The set for vertexes, vlad's trail, hunter trail
	LocationID vex[NUM_MAP_LOCATIONS], trailV[TRAIL_SIZE];
	giveMeTheTrail(gameState, PLAYER_DRACULA, trailV); //Get Dracs trail
	//Check the destination is not in the path
	dest = validateDest(gameState, trailV, dest, sea);
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
			if( HuntInPath(gameState, check[col]) ) continue;
			if ( !onTrail(trailV, check[col]) ) {
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
