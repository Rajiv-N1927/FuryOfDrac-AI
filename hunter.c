// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "HunterView.h"
#include "Queue.h"

int isUnique(int *arr, int obj);
int shortestPath(HunterView gameState, PlayerID pID, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int ret(int src, int dest, LocationID *pathFound, int *pathToAdd);

void decideHunterMove(HunterView gameState)
{
  LocationID bestPos = GENEVA;
  registerBestPlay(idToAbbrev(bestPos),"I'm on holiday in Geneva");
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
int shortestPath(HunterView gameState, PlayerID pID, int dest, int *path) {
	Map map = newMap();
	int src = whereIs(gameState, PLAYER_DRACULA);
	int *x= 0;
	QHead huntQ = initQ();
	addQ(huntQ, src);
	LocationID vex[NUM_MAP_LOCATIONS], vexDis[NUM_MAP_LOCATIONS];
	LocationID *check = malloc( sizeof(LocationID)*9 );
	//Initialise the vertex arrays
	for( int i = 0; i < NUM_MAP_LOCATIONS; i++ ) {
		vex[i] = -1;
	}
	//The src point to itself
	vex[src] = src;
	int toSearch = -1, col = -1;
  while ( vex[dest] == -1 && QSize(huntQ) != 0 ) {
		toSearch = deQ(huntQ);
    //FIND THE REACHABLE LOCATIONS
    int dist = sizePath(src, dest, vex);
    int railLength = (giveMeTheRound(gameState) + dist + pID)%4
    check = reachableLocations(map, x, toSearch, FALSE, railLength, TRUE, TRUE);
    //Loop through the check array to get locations
    for ( col = 0; col < *x; col++ ) {
			if( isUnique(vex, check[col]) == FALSE ) continue;
			vex[check[col]] = row;
			addQ(huntQ, check[col]);
      if( vex[dest] != -1 ) break;
		}

  } if ( QSize(huntQ) == 0 ) {
		free(check);
		disposeQ(huntQ);
		printf("NO PATH FOUND\n");
		return FALSE;
	}
	else {
		free(check);
		disposeQ(huntQ);
		return ret(src, dest, vex, path);
	}
}

int sizePath( int src, int dest, LocationID *pathFound ) {
	int i, j;
  if ( src == dest ) return 0;
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
	return x;
}
