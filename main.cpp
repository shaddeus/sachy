/* -- main.cpp --------------------------------------------------------------*\
**  
**   Autor: Pavel Tvrdik <pawel.tvrdik@gmail.com> <tvrdipa1@fel.cvut.cz>
**     Rok: 2009
** Predmet: Programovani v jazyce C/C++ (X36PJC)
**
**   Uloha: Hra sachy
**  Zadani: Realizujte hru sachy podle obvyklych pravidel. Program bude umet
**          zobrazovat hraci plochu s figurkami, bude reagovat na pozadavky
**          o tahy (kontroluje spravnost tahu) a bude obsahovat umelou
**          inteligenci, ktera umozni hru cloveka proti pocitaci. Pro zobrazeni
**          staci textovy vystup, zajemci mohou pouzit ncurses, rozhrani
**          Windows konzole nebo graficky vystup.
**
** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**
**   Autor: Pavel Tvrdik <pawel.tvrdik@gmail.com> <tvrdipa1@fel.cvut.cz>
**     Rok: 2010 - 2011
** Predmet: Technologie programovani v jazyce C/C++ (X36TJC)
**
**  Zadani: Pouzijte polymorfismus, STL a sablony.
**          ~ 2 000 radek.
**
\* ---------------------------------------------------------------------------*/

#include "dat_struct.h"
#include "figurky.h"
#include "sach_global.h"
#include "chess.h"

#include <iostream>
#include <ncurses.h>
#include <string.h>

int main( int argc, char * argv[] )
{
  initSach( argc, argv );
  ukazHelp();
  sachNovaHra();
  
  char vstup[100] = "";
  char upozorneni[100] = "";
  tah_t t;
  while( true ) {
    zobrazSachovnici();
    setPrikazovyRadek( upozorneni ); // upozorneni pod prikazovym radkem

    t = jakByTahlPocitac ( vstup, BILA ); // retezec vstup se nepouzije
    if ( rozpoznejSachMatPat ( t , BILA ) || mrtvaPozice() ) {
      strcpy(upozorneni,"");
      sachNovaHra();
      continue;
    }
 
    t = ctiVstup( vstup , 20);
    
    // byl zadan prikaz
    if ( t.prazdnyTah() ) {
      if ( porovnejRetezce( vstup ,"/konec" ) ) {
        break;
      }
      if ( porovnejRetezce( vstup ,"/novaHra" ) ) {
        sachNovaHra();
        wmove(::pravySloupec,0,0);
        continue;
      }
      if ( porovnejRetezce( vstup ,"/debug" ) ) {
        wprintw(::pravySloupec,"\n");
        wrefresh(::pravySloupec);
        getch();
        continue;
      }
    } 
    // byl zadan nejaky tah
    else {
      if ( ::sach[ (int)t.odkud.radek ][ (int)t.odkud.sloupec ].figurka != NULL
           && ::sach[ (int)t.odkud.radek ][ (int)t.odkud.sloupec ].figurka->getBarva() 
           && ::sach[ (int)t.odkud.radek ][ (int)t.odkud.sloupec ].figurka->tahni( t, true) ) {
        premenPesceNaKonciSachovnice();
        zobrazSachovnici();
        
        setPrikazovyRadek( "Pocitac premysli o svem tahu..." );
        t = jakByTahlPocitac( upozorneni, CERNA );
        if ( rozpoznejSachMatPat ( t, CERNA ) || mrtvaPozice() ) {
          strcpy(upozorneni,"");
          sachNovaHra();
          continue;
        }
        setPrikazovyRadek( upozorneni );
        if ( ! ::sach[ (int)t.odkud.radek ][ (int)t.odkud.sloupec ].figurka->tahni( t, true) )
          errorMsg("Oops. pocitac vygeneroval jako nejlepsi tah, nejaky neplatny tah."); 
        premenPesceNaKonciSachovnice();

        // getch();
      }
      else {
        strcpy( upozorneni, "Neplatny tah.");
      }
    } 
  }
  
  cleanPool();
  return 0;
}
