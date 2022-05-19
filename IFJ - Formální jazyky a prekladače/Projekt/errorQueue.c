/*
 * Created on Fri Oct 18 2019
 *
 * Copyright (c) 2019 Jakub Kocalka
 * This work is licensed under: GNU General Public License, version 3
 * @license https://www.gnu.org/licenses/gpl-3.0.en.html
 * @file parser.c
 * @author Jakub Kocalka
 * @brief Interface for the syn analyzer for IFJ19
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "errorQueue.h"


tErrQueueElem ErrqueueInit(){
    tErrQueue q = malloc(sizeof(struct errQueue));
    q->head = NULL;
    q->tail = NULL;
    return q;
}

tErrQueueElem newErrQueueElem(int errCode, int row, const char *msg){
    tErrQueueElem temp = malloc(sizeof(struct errQueueElem));
    temp->errCode = errCode;
    temp->row = row;
    temp->msg = malloc(strlen(msg)+1);
    strcpy(temp->msg, msg);
    temp->next = NULL;
    return temp;
}

bool ErrqueueEmpty(tErrQueue q){
    //return (q->count == 0)?true:false;
    return (q->tail == NULL)?true:false;
}

void enqueErr(tErrQueue q, int errCode, int row, const char *msg){
    tErrQueueElem tmpElem = newQueueElem(errCode, row, msg);
    
    if(queueEmpty(q)){
        q->head = tmpElem;
        q->tail = tmpElem;
    }else{
        q->tail->next = tmpElem;
        q->tail = tmpElem;
    }
}

tErrQueueElem dequeErr(tErrQueue q){
    if(queueEmpty(q)){
        return NULL;
    }else{
        tErrQueueElem temp = q->head;
        q->head = q->head->next;

        if(q->head == NULL){
            q->tail = NULL;
        }
        tErrQueueElem tmpElem = temp;
        free(temp);
        return tmpElem;
    }
}