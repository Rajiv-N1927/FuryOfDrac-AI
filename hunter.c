// hunter.c
// Implementation of your "Fury of Dracula" hunter AI

#include <stdlib.h>
#include <stdio.h>
#include "Game.h"
#include "Map.h"
#include "HunterView.h"
#include "Queue.h"

int isUnique(int *arr, int obj);
int shortestPath(HunterView gameState, PlayerID pID, int dest, int *path);
int sizePath( int src, int dest, LocationID *pathFound );
int ret(int src, int dest, LocationID *pathFound, int *pathToAdd);
//Where to move
int Godalming(LocationID loc);
int Seward(LocationID loc);
int VanHelsing(LocationID loc);
int MinHark(LocationID loc);

void decideHunterMove(HunterView gameState)
{
  LocationID bestPos = GENEVA;
  PlayerID me = whoAmI(gameState);
  LocationID whereAmI = whereIs(gameState, me);
  switch( me ) {
    case PLAYER_LORD_GODALMING:
      Godalming(whereAmI);
    break;
    case PLAYER_DR_SEWARD:
      Seward(whereAmI);
    break;
    case PLAYER_VAN_HELSING:
      VanHelsing(whereAmI);
    break;
    case PLAYER_MINA_HARKER:
      MinHark(whereAmI);
    break;
  }
  registerBestPlay(idToAbbrev(bestPos),"I'm on holiday in Geneva");
}
int Godalming(LocationID loc) {
  LocationID bestPos = -1;
  switch( loc ) {
    case UNKNOWN_LOCATION:
    break;
  }
  return bestPos;
}

int Seward(LocationID loc) {
  LocationID bestPos = -1;
  switch( loc ) {
    case UNKNOWN_LOCATION:
    break;
  }
  return bestPos;
}

int VanHelsing(LocationID loc) {
  LocationID bestPos = -1;
  switch( loc ) {
    case UNKNOWN_LOCATION:
    break;
  }
  return bestPos;
}

int MinHark(LocationID loc) {
  LocationID bestPos = -1;
  switch( loc ) {
    case UNKNOWN_LOCATION:
    break;
  }
  return bestPos;
}

//FOR THE SHORTESTPATH
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
	int src = whereIs(gameState, pID);
	QHead huntQ = initQ();
	addQ(huntQ, src);
	LocationID vex[NUM_MAP_LOCATIONS];
	//Initialise the vertex arrays
  int i;
	for( i = 0; i < NUM_MAP_LOCATIONS; i++ ) {
		vex[i] = -1;
	}
	//The src point to itself
	vex[src] = src;
	int toSearch = -1, col = -1;
  while ( vex[dest] == -1 && QSize(huntQ) != 0 ) {
    toSearch = deQ(huntQ);
    //FIND THE REACHABLE LOCATIONS
    int dist = sizePath(src, toSearch, vex); int test = 0;
    int railLength = (giveMeTheRound(gameState) + dist + pID)%4;
    //Check the location area;
    LocationID *check; int *x = &test;
    check = reachableLocations(map, x, toSearch, FALSE, railLength, TRUE, TRUE);
    //Loop through the check array to get locations
    for ( col = 0; col < *x; col++ ) {
      if( isUnique(vex, check[col]) == FALSE ) continue;
      vex[check[col]] = toSearch;
      addQ(huntQ, check[col]);
      if( vex[dest] != -1 ) break;
    }

  } if ( QSize(huntQ) == 0 ) {
    disposeQ(huntQ);
    printf("NO PATH FOUND\n");
    return FALSE;
  }
  else {
    disposeQ(huntQ);
    int x = ret(src, dest, vex, path);
    return x;
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
  return x+2;
}
