/* chess.h */

#ifndef _CHESS_H_
#define _CHESS_H_

#include "dat_struct.h"
#include "figurky.h"
#include "sach_global.h"
#include <ncurses.h>
#include <iostream>

void               initSach                       ( int, char ** );
void               ukazHelp                       ( void );
void               sachNovaHra                    ( void );
bool               umistiFigurkuNaSachovnici      ( const char& , const bool&, const char );
void               cleanPool                      ( void );
void               zobrazSachovnici               ( void );
void               zobrazSachovniciShort          ( void );

void               sachKrytiPolicek               ( void );
void               sachKrytiPolicek               ( const tah_t* );
bool               sachPolicko                    ( const pozice_t& );
bool               sachVolnePolicko               ( const pozice_t& );
bool               sachPolickoSFigurkouProtihrace ( const pozice_t& , bool , bool );
bool               sachVolnePolickoNeboProtihrace ( const pozice_t& , bool , bool );
bool               sachRosadovaVez                ( const pozice_t& , bool );
void               premenPesceNaKonciSachovnice   ( void );
bool               rozpoznejSachMatPat            ( const tah_t& , bool );
bool               mrtvaPozice                    ( void );

tah_t              ctiVstup                       ( char*, int );
bool               porovnejRetezce                ( const char*, const char* );
void               setPrikazovyRadek              ( const char* );
void               setPrikazovyRadek              ( void );

tah_t              jakByTahlPocitac                   ( char*, bool );
void               najdiNejlepsiTah               ( const tah_t*, tah_t&, int&, bool );
int                ohodnotTah                     ( const tah_t& );

// ncurses
WINDOW            *create_newwin                  ( int, int, int, int );
void               destroy_win                    ( WINDOW * );

#endif // _CHESS_H_

