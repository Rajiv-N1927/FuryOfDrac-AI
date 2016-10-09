// dracula.c
// Implementation of your "Fury of Dracula" Dracula AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "Map.h"
#include "DracView.h"
#include "Queue.h"

#define MAX_NO_CONN 9
int isUnique(int *arr, int obj);
int shortestPath(DracView gameState, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int ret(int src, int dest, LocationID *pathFound, int *pathToAdd);

void decideDraculaMove(DracView gameState)
{
	LocationID bestPos = CASTLE_DRACULA;
	registerBestPlay(idToAbbrev(bestPos),"Mwuhahahaha");
}

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
	int *x= 0;
	QHead dracQ = initQ();
	addQ(dracQ, src);
	LocationID vex[NUM_MAP_LOCATIONS], vexDis[NUM_MAP_LOCATIONS];
	LocationID *check = malloc( sizeof(LocationID)*9 );
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
    check = reachableLocations(map, x, toSearch, TRUE, 0, TRUE, TRUE);
    //Loop through the check array to get locations
    for ( col = 0; col < *x; col++ ) {
			if( isUnique(vex, check[col]) == FALSE ) continue;
			vex[check[col]] = row;
			addQ(dracQ, check[col]);
      if( vex[dest] != -1 ) break;
		}

  } if ( QSize(dracQ) == 0 ) {
		free(check);
		disposeQ(dracQ);
		printf("NO PATH FOUND\n");
		return FALSE;
	}
	else {
		free(check);
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
