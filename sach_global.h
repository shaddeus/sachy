/* sach_global.h */

#ifndef SACH_GLOBAL
#define SACH_GLOBAL

#include <vector>
#include "dat_struct.h"
#include "figurky.h"

// Jedno policko sachovnice
struct polickoSach {
  CFigurka * figurka;
  //CFigurka * dosah [ MAX_POCET_FIGUREK_NA_1_POLICKO ];
  std::vector<CFigurka*> dosah;  
};

// Globalni promenna sach jako cela herni sachovnice.
extern polickoSach sach[8][8];

// Globalni promenne, ukazatele na oba dva krale obou hracu. Slouzi pro rychle nalezeni krale na sachovnici
extern CFigurka* bilyKral;
extern CFigurka* cernyKral;

// pocitadlo na 50 tahu, kdy se zadny kamen nevezme a zaroven se ani netahne pesakem
extern int pocNa50Tahu;

// verbose mod
extern int verbose;

#endif

