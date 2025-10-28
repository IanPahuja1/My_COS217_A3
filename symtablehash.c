/* Hash table implementation*/
#include "symtable.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* One binding in a bucket*/
typedef struct Binding {
    /* Key string*/
    char *pcKey;
    /* Value pointer*/
    void *pvValue;
    /* Next binding in bucket*/
    struct Binding *pNext;
} Binding;

/* Table representation*/
struct SymTable {
    /* Bucket heads array*/
    Binding **ppBuckets;
    /* Bucket count*/
    size_t uBucketCount;
    /* Binding count*/
    size_t uLength;
    /* Index into size table*/
    int iSizeIndex;
};

/* Compute the hash index of pcKey within the range from zero to uBucketCount minus one. 
Return the index*/
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
    const size_t HASH_MULTIPLIER = 65599U;
    size_t u;
    size_t uHash = 0U;
    assert(pcKey != NULL);
    for (u = 0U; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
    return uHash % uBucketCount;
}

/* Find binding for pcKey in pHead, return pointer or NULL*/
static Binding *SymTableHash_find(Binding *pHead, const char *pcKey)
{
    Binding *pCur;
    assert(pcKey != NULL); /* pHead could be NULL*/
    for (pCur = pHead; pCur != NULL; pCur = pCur->pNext)
        if (strcmp(pCur->pcKey, pcKey) == 0) return pCur;
    return NULL;
}

/* Global constant: prime bucket sizes required by the specification for growth. */
static const size_t kPrimeSizes[] = {
    509U, 1021U, 2039U, 4093U, 8191U, 16381U, 32749U, 65521U
};

/* Grow oSymTable to next size if uLength > uBucketCount*/
static void SymTableHash_maybeGrow(SymTable_T oSymTable)
{
    Binding **ppNew;
    size_t uNewCount;
    size_t i;
    if (oSymTable->iSizeIndex >= (int)(sizeof(kPrimeSizes)/sizeof(kPrimeSizes[0])) - 1)
        return;
    if (oSymTable->uLength <= oSymTable->uBucketCount)
        return;

    uNewCount = kPrimeSizes[oSymTable->iSizeIndex + 1];
    ppNew = (Binding **)calloc(uNewCount, sizeof(*ppNew));
    if (ppNew == NULL) return; /* continue without expanding */

    for (i = 0U; i < oSymTable->uBucketCount; i++) {
        Binding *pCur = oSymTable->ppBuckets[i];
        while (pCur != NULL) {
            Binding *pNext = pCur->pNext;
            size_t uHashIdx = SymTable_hash(pCur->pcKey, uNewCount);
            pCur->pNext = ppNew[uHashIdx];
            ppNew[uHashIdx] = pCur;
            pCur = pNext;
        }
    }
    free(oSymTable->ppBuckets);
    oSymTable->ppBuckets = ppNew;
    oSymTable->uBucketCount = uNewCount;
    oSymTable->iSizeIndex++;
}

/* Create table*/
SymTable_T SymTable_new(void)
{
    SymTable_T oSymTable = (SymTable_T)malloc(sizeof(*oSymTable));
    if (oSymTable == NULL) return NULL;
    oSymTable->iSizeIndex = 0;
    oSymTable->uBucketCount = kPrimeSizes[oSymTable->iSizeIndex];
    oSymTable->uLength = 0U;
    oSymTable->ppBuckets = (Binding **)calloc(oSymTable->uBucketCount,
                                              sizeof(*oSymTable->ppBuckets));
    if (oSymTable->ppBuckets == NULL) { free(oSymTable); return NULL; }
    return oSymTable;
}

/* Free table, keys, and nodes*/
void SymTable_free(SymTable_T oSymTable)
{
    size_t i;
    if (oSymTable == NULL) return;
    for (i = 0U; i < oSymTable->uBucketCount; i++) {
        Binding *pCur = oSymTable->ppBuckets[i];
        while (pCur != NULL) {
            Binding *pNext = pCur->pNext;
            free(pCur->pcKey);
            free(pCur);
            pCur = pNext;
        }
    }
    free(oSymTable->ppBuckets);
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
    size_t uHashIdx;
    Binding *pNew;
    char *pcCopy;
    assert(oSymTable != NULL);
    assert(pcKey != NULL); /* pvValue could be NULL*/

    uHashIdx = SymTable_hash(pcKey, oSymTable->uBucketCount);
    if (SymTableHash_find(oSymTable->ppBuckets[uHashIdx], pcKey) != NULL) return 0;

    pcCopy = (char *)malloc(strlen(pcKey) + 1U);
    if (pcCopy == NULL) return 0;
    strcpy(pcCopy, pcKey);

    pNew = (Binding *)malloc(sizeof(*pNew));
    if (pNew == NULL) { free(pcCopy); return 0; }

    pNew->pcKey = pcCopy;
    pNew->pvValue = (void *)pvValue;
    pNew->pNext = oSymTable->ppBuckets[uHashIdx];
    oSymTable->ppBuckets[uHashIdx] = pNew;
    oSymTable->uLength++;

    SymTableHash_maybeGrow(oSymTable);
    return 1;
}

/* Replace value*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    size_t uHashIdx;
    Binding *pBind;
    void *pvOld;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    uHashIdx = SymTable_hash(pcKey, oSymTable->uBucketCount);
    pBind = SymTableHash_find(oSymTable->ppBuckets[uHashIdx], pcKey);
    if (pBind == NULL) return NULL;
    pvOld = pBind->pvValue;
    pBind->pvValue = (void *)pvValue;
    return pvOld;
}

/* Check if contains key*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    size_t uHashIdx;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    uHashIdx = SymTable_hash(pcKey, oSymTable->uBucketCount);
    return SymTableHash_find(oSymTable->ppBuckets[uHashIdx], pcKey) != NULL;
}

/* Get value*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    size_t uHashIdx;
    Binding *pBind;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    uHashIdx = SymTable_hash(pcKey, oSymTable->uBucketCount);
    pBind = SymTableHash_find(oSymTable->ppBuckets[uHashIdx], pcKey);
    return pBind ? pBind->pvValue : NULL;
}

/* Remove binding*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    size_t uHashIdx;
    Binding *pPrev;
    Binding *pCur;
    void *pvVal;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    uHashIdx = SymTable_hash(pcKey, oSymTable->uBucketCount);
    pPrev = NULL;
    pCur = oSymTable->ppBuckets[uHashIdx];
    while (pCur != NULL) {
        if (strcmp(pCur->pcKey, pcKey) == 0) {
            pvVal = pCur->pvValue;
            if (pPrev != NULL) pPrev->pNext = pCur->pNext;
            else oSymTable->ppBuckets[uHashIdx] = pCur->pNext;
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
    size_t i;
    Binding *pCur;
    assert(oSymTable != NULL);
    assert(pfApply != NULL); /* pvExtra could be NULL*/
    for (i = 0U; i < oSymTable->uBucketCount; i++) {
        pCur = oSymTable->ppBuckets[i];
        while (pCur != NULL) {
            pfApply(pCur->pcKey, pCur->pvValue, (void *)pvExtra);
            pCur = pCur->pNext;
        }
    }
}
