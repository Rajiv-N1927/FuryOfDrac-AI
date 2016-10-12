//Linked list representation of a queue
typedef struct qNode *QNode;
typedef struct qHead *QHead;

#include "Places.h"

struct qNode {
  LocationID Loc;
  QNode next;
};

struct qHead {
  QNode head;
  QNode tail;
};

QHead initQ();
QNode initN(LocationID lID);
void dispQ(QHead Q);
void addQ(QHead Q, LocationID lID);
LocationID deQ(QHead Q);
void disposeQ(QHead Q);
int QSize(QHead Q);
