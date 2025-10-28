/* Linked list implementation*/
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* One binding in the list*/
typedef struct Binding {
    /* Key string*/
    char *pcKey;
    /* Value pointer*/
    void *pvValue;
    /* Next binding*/
    struct Binding *pNext;
} Binding;

/* Table representation*/
struct SymTable {
    /* Head of list*/
    Binding *pHead;
    /* Binding count*/
    size_t uLength;
};

/* Find binding for pcKey starting at pHead; return pointer or NULL*/
static Binding *SymTableList_find(Binding *pHead, const char *pcKey)
{
    Binding *pCur;
    assert(pcKey != NULL);
    for (pCur = pHead; pCur != NULL; pCur = pCur->pNext)
        if (strcmp(pCur->pcKey, pcKey) == 0) return pCur;
    /* Null if not found*/
    return NULL;
}

/* Create table*/
SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable = (SymTable_T)malloc(sizeof(*oSymTable));
    if (oSymTable == NULL) return NULL;
    oSymTable->pHead = NULL;
    oSymTable->uLength = 0U;
    return oSymTable;
}

/* Free table and keys*/
void SymTable_free(SymTable_T oSymTable)
{
    Binding *pCur;
    Binding *pNext;
    if (oSymTable == NULL) return;
    pCur = oSymTable->pHead;
    while (pCur != NULL) {
        pNext = pCur->pNext;
        free(pCur->pcKey);
        free(pCur);
        pCur = pNext;
    }
    free(oSymTable);
}

/* Get count*/
size_t SymTable_getLength(SymTable_T oSymTable)
{
    assert(oSymTable != NULL);
    return oSymTable->uLength;
}

/* Insert binding*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    Binding *pNew;
    char *pcCopy;
    assert(oSymTable != NULL);
    assert(pcKey != NULL); /* pvValue could be NULL*/

    if (SymTableList_find(oSymTable->pHead, pcKey) != NULL) return 0;

    pcCopy = (char *)malloc(strlen(pcKey) + 1U);
    if (pcCopy == NULL) return 0;
    strcpy(pcCopy, pcKey);

    pNew = (Binding *)malloc(sizeof(*pNew));
    if (pNew == NULL) { free(pcCopy); return 0; }

    pNew->pcKey = pcCopy;
    pNew->pvValue = (void *)pvValue;
    pNew->pNext = oSymTable->pHead;
    oSymTable->pHead = pNew;
    oSymTable->uLength++;
    return 1;
}

/* Replace value*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    Binding *pBind;
    void *pvOld;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    pBind = SymTableList_find(oSymTable->pHead, pcKey);
    if (pBind == NULL) return NULL;
    pvOld = pBind->pvValue;
    pBind->pvValue = (void *)pvValue;
    return pvOld;
}

/* Checks if contains key*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    return SymTableList_find(oSymTable->pHead, pcKey) != NULL;
}

/* Get value*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    Binding *pBind;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    pBind = SymTableList_find(oSymTable->pHead, pcKey);
    return pBind ? pBind->pvValue : NULL;
}

/* Remove binding*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    Binding *pPrev;
    Binding *pCur;
    void *pvVal;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    pPrev = NULL;
    pCur = oSymTable->pHead;
    while (pCur != NULL) {
        if (strcmp(pCur->pcKey, pcKey) == 0) {
            pvVal = pCur->pvValue;
            if (pPrev != NULL) pPrev->pNext = pCur->pNext;
            else oSymTable->pHead = pCur->pNext;
            free(pCur->pcKey);
            free(pCur);
            oSymTable->uLength--;
            return pvVal;
        }
        pPrev = pCur;
        pCur = pCur->pNext;
    }
    return NULL;
}

/* Map over bindings*/
void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra)
{
    Binding *pCur;
    assert(oSymTable != NULL);
    assert(pfApply != NULL); /* pvExtra could be NULL*/
    pCur = oSymTable->pHead;
    while (pCur != NULL) {
        pfApply(pCur->pcKey, pCur->pvValue, (void *)pvExtra);
        pCur = pCur->pNext;
    }
}
