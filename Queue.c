#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"

// int main( int argc, char* argv[] ) {
//   QHead k = initQ();
//   addQ(k, LONDON);
//   addQ(k, BERLIN);
//   dispQ(k);
//   deQ(k);
//   dispQ(k);
//   deQ(k);
//   dispQ(k);
//   return 0;
// }

QHead initQ() {
  QHead newQ = malloc(sizeof(struct qHead));
  newQ->head = NULL;
  newQ->tail = NULL;
  return newQ;
}

QNode initN(LocationID lID) {
  QNode node = malloc(sizeof(struct qNode));
  node->Loc = lID;
  node->next = NULL;
  return node;
}
void addQ(QHead Q, LocationID lID) {
  QNode toAdd = initN(lID);
  if ( Q ) {
    if ( Q->head == NULL ) {
      Q->head = toAdd;
      Q->tail = toAdd;
    } else {
      Q->tail->next = toAdd;
      Q->tail = toAdd;
    }
  }
}

void dispQ(QHead Q) {
  QNode start;
  for (start = Q->head; start != NULL; start = start->next ) {
    printf("Location: %s\n", idToName(start->Loc));
  }
  printf("Size %d\n", QSize(Q));
}

int QSize(QHead Q) {
  QNode start;
  int i = 0;
  for (start = Q->head; start != NULL; start = start->next, i++ );
  return i;
}

LocationID deQ(QHead Q) {
  LocationID retID = Q->head->Loc;
  QNode old = Q->head;
  Q->head = old->next;
  if (Q->head == NULL)
    Q->tail = NULL;
  free(old);
  return retID;
}
void disposeQ(QHead Q) {
  QNode start = Q->head;
  QNode nextN;
  while ( start != NULL ) {
    nextN = start->next;
    free(start);
    start = nextN;
  }
}
