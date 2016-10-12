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

int Godalming(LocationID loc);
int Seward(LocationID loc);
int VanHelsing(LocationID loc);
int MinHark(LocationID loc);



void decideHunterMove(HunterView gameState)
{
  LocationID bestPos = LONDON;
  PlayerID me = whoAmI(gameState);
  LocationID whereAmI = whereIs(gameState, me);

  // IMPORTANT: need to also account for when they die and are transported to the hospital.
  // in that case...if we're going to use these temporary hard loops. We just need to add
  // a default for moving back to any position we choose in the loop (using shortestpath)

  switch( me ) {
    case PLAYER_LORD_GODALMING:
      bestPos = Godalming(whereAmI);
    break;
    case PLAYER_DR_SEWARD:
      bestPos = Seward(whereAmI);
    break;
    case PLAYER_VAN_HELSING:
      bestPos = VanHelsing(whereAmI);
    break;
    case PLAYER_MINA_HARKER:
      bestPos = MinHark(whereAmI);
    break;
  }  
  registerBestPlay(idToAbbrev(bestPos),"fight");
}
int Godalming(LocationID loc) {
  LocationID bestPos = LISBON;
  switch( loc ) {
    case SANTANDER :
      bestPos = LISBON; 
      break;

    case LISBON :
      bestPos = CADIZ; 
      break;

    case CADIZ :
      bestPos = GRANADA; 
      break;

    case GRANADA :
      bestPos = ALICANTE; 
      break;

    case ALICANTE :
      bestPos = SARAGOSSA; 
      break;

    case SARAGOSSA :
      bestPos = SANTANDER; 
      break;

    case UNKNOWN_LOATION : 
      bestPos = CADIZ; 
      break;

    default : 
      bestPos = // shortestPath back to alicante; 
      break;

  }
  return bestPos;
}

int Seward(LocationID loc) {
  LocationID bestPos = ENGLISH_CHANNEL;
  switch( loc ) {
    case LE_HAVRE :
      bestPos = ENGLISH_CHANNEL; 
      break;

    case ENGLISH_CHANNEL :
      bestPos = LONDON; 
      break;

    case LONDON :
      bestPos = SWANSEA; 
      break;

    case SWANSEA :
      bestPos = LIVERPOOL; 
      break;

    case LIVERPOOL :
      bestPos = MANCHESTER; 
      break;

    case MANCHESTER :
      bestPos = EDINBURGH; 
      break;

    case EDINBURGH:
      bestPos = NORTH_SEA; 
      break;

    case NORTH_SEA :
      bestPos = AMSTERDAM; 
      break;

    case AMSTERDAM :
      bestPos = BRUSSELS; 
      break;

    case BRUSSELS :
      bestPos = LE_HAVRE; 
      break;

    case UNKNOWN_LOATION : 
      bestPos = LONDON; 
      break;

    default : 
      bestPos = // shortestPath back to london; 
      break;
  }
  return bestPos;
}

int VanHelsing(LocationID loc) {
  LocationID bestPos = PARIS;
  switch( loc ) {

    case HAMBURG :
      bestPos = COLOGNE; 
      break;

    case COLOGNE :
      bestPos = BRUSSELS; 
      break;

    case BRUSSELS :
      bestPos = PARIS; 
      break;

    case PARIS :
      bestPos = STRASBOURG; 
      break;

    case STRASBOURG :
      bestPos = GENEVA; 
      break;

    case GENEVA :
      bestPos = MARSEILLES; 
      break;

    case MARSEILLES :
      bestPos = MILAN; 
      break;

    case MILAN :
      bestPos = MUNICH; 
      break;

    case MUNICH :
      bestPos = NUREMBURG; 
      break;

    case NUREMBURG :
      bestPos = LEIPZIG; 
      break;

    case LEIPZIG :
      bestPos = HAMBURG; 
      break;

    case UNKNOWN_LOATION : 
      bestPos = PARIS; 
      break;

    default : 
      bestPos = // shortestPath back to PARIS 
      break;

  }
  return bestPos;
}

int MinHark(LocationID loc) {
  LocationID bestPos = BUDAPEST;
  switch( loc ) {
    case KLAUSENBURG :
      bestPos = BUDAPEST; 
      break;

    case BUDAPEST :
      bestPos = ZAGREB; 
      break;

    case ZAGREB :
      bestPos = SARAJEVO; 
      break;

    case SARAJEVO :
      bestPos = VALONA; 
      break;

    case VALONA :
      bestPos = SALONICA; 
      break;

    case SALONICA :
      bestPos = SOFIA; 
      break;

    case SOFIA :
      bestPos = BUCHAREST; 
      break;

    case BUCHAREST :
      bestPos = CONSTANTA; 
      break;

    case CONSTANTA :
      bestPos = GALATZ; 
      break;

    case GALATZ :
      bestPos = KLAUSENBURG; 
      break;

    case UNKNOWN_LOATION : 
      bestPos = SOFIA; 
      break;

    default : 
      bestPos = // shortestPath back to VALONA 
      break;
  }
  return bestPos;
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
    int railLength = (giveMeTheRound(gameState) + dist + 0)%4;
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
