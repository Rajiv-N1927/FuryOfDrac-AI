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
		// int trailLengthToGo = 0;
		if ( myPos != CASTLE_DRACULA && curHealth <= HP_THRESHOLD ) {
			printf("EXB\n");
			shortestPath( gameState, TRUE, CASTLE_DRACULA, locToGo);
			bestPos = locToGo[1];
		} else {
			printf("EXH\n");
			int Tlength = shortestPath( gameState, TRUE, BORDEAUX, locToGo);
			for ( int x = 0; x < Tlength; x++ ) {
				printf("\n[Pos]:%d->%s\n", x, idToName(locToGo[x]));
			}
			bestPos = locToGo[1];
		}

	}
	printf("=============CURRENT STATS=============\n");
	printf("\n[Registered Pos]->%s\n", idToName(bestPos));
	printf("[HEALTH]: %d\n\n---------------------------------------\n", curHealth);
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
