/* SymTable ADT interface*/
#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED
#include <stddef.h>

/* Opaque handle to a symbol table object. */
typedef struct SymTable *SymTable_T;

/* Create an empty table and return it, or return NULL on allocation failure. */
SymTable_T SymTable_new(void);

/* Free all memory owned by oSymTable, doesn't free any value pointers. */
void SymTable_free(SymTable_T oSymTable);

/* Return the number of bindings currently stored in oSymTable. */
size_t SymTable_getLength(SymTable_T oSymTable);

/* Insert key pcKey with value pvValue into oSymTable if absent.
   Return 1 if inserted, 0 if pcKey already present or on allocation failure*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* Replace the value for pcKey in oSymTable with pvValue.
   Return the old value pointer, or NULL if pcKey not present*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

/* Return 1 if oSymTable contains pcKey, else return 0*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* Return the value pointer for pcKey in oSymTable, or NULL if absent*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* Remove pcKey from oSymTable and return its value pointer, or NULL if absent*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* Apply pfApply to each binding in oSymTable as pfApply(pcKey, pvValue, pvExtra).
   Return nothing*/
void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra);
#endif
