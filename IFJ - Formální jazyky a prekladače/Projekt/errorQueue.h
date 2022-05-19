/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Juraj Sloboda
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file parser.c
 * @author Juraj Sloboda
 * @brief Interface for the syn analyzer for IFJ19
 */

struct errQueueElem{
    int errCode;
    int row;
    char *msg;
    struct errQueueElem *next;
};
typedef struct errQueueElem* tErrQueueElem;

struct errQueue{
    tErrQueueElem head;
    tErrQueueElem tail;
};
typedef struct errQueue* tErrQueue;

tErrQueueElem ErrqueueInit();
tErrQueueElem newErrQueueElem(int errCode, int row, const char *msg);
bool ErrqueueEmpty(tErrQueue q);
void enqueErr(tErrQueue q, int errCode, int row, const char *msg);
tErrQueueElem dequeErr(tErrQueue q);