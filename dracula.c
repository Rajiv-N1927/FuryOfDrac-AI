// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "Map.h"
#include "DracView.h"
#include "Queue.h"

#define MAX_NO_CONN 9
//FOR THE SHORTEST PATH
int isUnique(int *arr, int obj);
int shortestPath(DracView gameState, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int ret(int src, int dest, LocationID *pathFound, int *pathToAdd);
//FOR THE MOVEMENT DECISION
int checkPosInNTrail(LocationID loc);
//int locPos(int *arr, LocationID obj);
//int closestPosToTrail(LocationID loc);
//CHANGE THIS
void decideDraculaMove(DracView gameState)
{
	LocationID bestPos = -1;
	PlayerID me = PLAYER_DRACULA;
	LocationID myPos = whereIs(gameState, me);
	int curHealth = howHealthyIs(gameState, me);
	if( curHealth >= 28 && checkPosInNTrail(myPos) ) {
		swtich(myPos) {
			case UNKNOWN_LOCATION:  bestPos = MARSEILLES; //0
				break;
			case MARSEILLES: 				bestPos = MEDITERRANEAN_SEA; //1 |
				break; 																								// |
			case MEDITERRANEAN_SEA: bestPos = ALICANTE; //2					 |
				break;																								// |
			case ALICANTE: 					bestPos = SARAGOSSA; //3         |
				break;																								// |
			case SARAGOSSA: 				bestPos = BORDEAUX; //4          |
				break;																								// |
			case BORDEAUX: 					bestPos = BAY_OF_BISCAY; //5     |
				break; 																							  // |
			case BAY_OF_BISCAY: 		bestPos = MEDITERRANEAN_SEA; //6 |
		}
	} else {
		//GOTTA GO BACK TO THE CASTLE BOIS
		LocationID locToGo[NUM_MAP_LOCATIONS];
		int trailLengthToGo = 0;
		if ( myPos != CASTLE_DRACULA && curHealth <= 28 ) {
			trailLengthToGo = shortestPath(gameState, CASTLE_DRACULA, locToGo);
			bestPos = locToGo[1];
		} else {
			trailLengthToGo = shortestPath(gameState, MEDITERRANEAN_SEA, locToGo);
			bestPos = locToGo[1];
		}
	}
	//Register the best play
	registerBestPlay(idToAbbrev(bestPos),"Mwuhahahaha");
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
												 ALICANTE, SARAGOSSA, BORDEAUX, BAY_OF_BISCAY};
	for (check = 0; check < 7; check++) {
		if ( nTrail[check] == loc ) return TRUE;
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


//INCLUDES BOTH ROAD AND SEA CONNS
int shortestPath(DracView gameState, int dest, int *path) {
	Map map = newMap();
	int src = whereIs(gameState, PLAYER_DRACULA);
	QHead dracQ = initQ();
	addQ(dracQ, src);
	LocationID vex[NUM_MAP_LOCATIONS];
	//Initialise the vertex arrays
	for( int i = 0; i < NUM_MAP_LOCATIONS; i++ ) {
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
    check = reachableLocations(map, x, toSearch, TRUE, 0, TRUE, TRUE);
    //Loop through the check array to get locations
    for ( col = 0; col < *x; col++ ) {
			//Need to work on this
			if( isUnique(vex, check[col]) == FALSE ) continue;
			vex[check[col]] = toSearch;
			addQ(dracQ, check[col]);
      if( vex[dest] != -1 ) break;
		}

  } if ( QSize(dracQ) == 0 ) {
		free(map);
		disposeQ(dracQ);
		printf("NO PATH FOUND\n");
		return FALSE;
	}
	else {
		free(map);
		disposeQ(dracQ);
		return ret(src, dest, vex, path);
	}
}

int sizePath( int src, int dest, LocationID *pathFound ) {
	int i, j;
	for( i = pathFound[dest], j = 0; i != src; i = pathFound[i], j++ );
	return j;
}

int ret(int src, int dest, LocationID *pathFound, int *pathToAdd) {
	int x = sizePath(src, dest, pathFound), i, j;
	pathToAdd[0] = src;
	pathToAdd[x+1] = dest;
	for( i = pathFound[dest], j = x; i != src; i = pathFound[i], j-- ) {
		pathToAdd[j] = i;
	}
	return x+2;
}
