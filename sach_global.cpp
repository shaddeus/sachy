/* sach_global.cpp */

#include "sach_global.h"

// Globalni(!) promenna sach jako cela herni sachovnice.
polickoSach sach[8][8];

// Globalni promenne, ukazatele na oba dva krale obou hracu. Slouzi pro rychle nalezeni krale na sachovnici
CFigurka* bilyKral;
CFigurka* cernyKral;

// pocitadlo na 50 tahu, kdy se zadny kamen nevezme a zaroven se ani netahne pesakem
int pocNa50Tahu;

// nastavuje verbos mod
int verbose;

