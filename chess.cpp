/* chess.cpp */

#include <vector>
#include <algorithm>
#include "chess.h"

/**
 * Pripravi sachovnici na prve pouziti a celkove program
 */
void initSach ( int argc, char * argv[] ) {
  // verbose mod
  verbose = 0;
  if ( argc > 1 )
    for( int i = 1; i < argc; i++ )
      if ( porovnejRetezce(argv[i], "-v") ) {
        verbose = 1;
        std::cout << argv[i] << " verbose rezim 1" << std::endl;
        break;
      }

  // rozbehne ncurses
  initscr();                    /* Start curses mode     */
  //raw();                        /* Line buffering disabled  */
  //keypad(stdscr, TRUE);         /* We get F1, F2 etc..    */
  noecho();                     /* Don't echo() while we do getch */
 
  int startx, starty, width, height;

  height = LINES;
  width = COLS - 76;
  starty = 0;  
  startx = 75;  
  ::pravySloupec = create_newwin(height, width, starty, startx);

  // pro jistotu vynulujeme adresy v sachovnici
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      ::sach[ radek ][ sloupec ].figurka = NULL;
      ::sach[ radek ][ sloupec ].dosah.reserve( MAX_POCET_FIGUREK_NA_1_POLICKO );
    }
  }
}

/**
 * Funkce zobrazi uvodni napovedu, povidani
 */
void ukazHelp ( void ) {
  clear();
  attron(A_BOLD);
  if ( verbose == 0 ) 
    printw("\n              ***  Vitejte v programu Sachy  ***\n\n");
  else
    printw("\n     ***  Vitejte v programu Sachy [verbose mod]  ***\n\n");
  if ( COLS < (90) || LINES < 37)
  printw("  Zvetste si prosim terminalove okno minimalne na 90 sloupcu\n  a 37 radek. Nyni mate pouze %d sloupcu a %d radek.\n\n", COLS, LINES);
  attroff(A_BOLD);
  printw("  Budete hrat bilymi (dolnimi) figurkami. Sachove tahy se\n  zadavaji tak, ze napisete pozici odkud a kam tahnete\n  bez jakykoliv mezer - napr. e2e4\n");
  printw("  Rosadu provedete tahem krale na policko veze - napr e1a1\n");
  printw("  Pro preruseni stavajici hry a zahajeni nove sachove partie,\n  musite napsat prikaz ");
  attron(A_BOLD);  printw("/novaHra\n"); attroff(A_BOLD);
  printw("  Pro ukonceni programu, napisete ");
  attron(A_BOLD);  printw("/konec\n");  attroff(A_BOLD);
  if ( verbose == 0 ) {
    printw("  Pokud program spustite s parametrem ");
    attron(A_BOLD);  printw("-v");  attroff(A_BOLD);
    printw(", budou se vam\n  zobrazovat dostupne tahy a jejich ohodnoceni pocitacem.\n");
  }
  printw("  Nyni pokracujte stiskem libovolne klavesy...\n\n");
  printw("  -- Pavel Tvrdik\n");
  getch();
  clear();
}

/**
 * Dealokuje z sachovnice vsechny dynamicky vytvorene figurky, nastavi adresy na NULL
 */
void cleanPoolSach ( void ) {
  // projdeme pocitane reference na nekym sebrane figurky
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      if ( ::sach[ radek ][ sloupec ].figurka != NULL ) {
        ::sach[ radek ][ sloupec ].figurka->vyreseniReferenciSebranaFigurka();
      }
    }
  }

  // dealokujeme figurky z sachovnice
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      if ( ::sach[ radek ][ sloupec ].figurka != NULL ) {
        delete ::sach[ radek ][ sloupec ].figurka;
        ::sach[ radek ][ sloupec ].figurka = NULL;
      }
      ::sach[ radek ][ sloupec ].dosah.clear();
    }
  }
}

/**
 * Umisti na sachovnici figuru
 * @param typFigurky PESAK, STRELEC, VEZ, KUN, ...
 * @param barva BILA/true, CERNA/false
 * @param poz sachova pozice "e2", "d8", ...
 * @param return true v pripade uspechu, jinak false
 */
bool umistiFigurkuNaSachovnici ( const char& typFigurky, const bool& barva, const char poz[3] ) {
  pozice_t pozice = sach2pozice ( poz );

  if ( ! sachPolicko ( pozice ) ) {
    std::cout << "umistiFigurkuNaSachovnici(): neplatna pozice figury" << std::endl;
    return false;
  }
  
  if ( ! sachVolnePolicko ( pozice ) ){
    std::cout << "umistiFigurkuNaSachovnici(): sachove policko " << pozice << " je obsazene figurou" << std::endl;
    return false;
  }
  
  if ( typFigurky == PESAK ){
    CPesak* ptr = new CPesak ( pozice , barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else if ( typFigurky == KRAL ){
    CKral* ptr = new CKral ( pozice , barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else if ( typFigurky == DAMA ){
    TFigurka<CDama> *ptr = new TFigurka<CDama>( pozice, barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else if ( typFigurky == STRELEC ){
    TFigurka<CStrelec> *ptr = new TFigurka<CStrelec>( pozice, barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else if ( typFigurky == VEZ ){
    TFigurka<CVez> *ptr = new TFigurka<CVez>( pozice, barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else if ( typFigurky == KUN ){
    TFigurka<CKun> *ptr = new TFigurka<CKun>( pozice, barva );
    ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka = ptr;
  }
  else {
    std::cout << "umistiFigurkuNaSachovnici(): neplatny typ figury" << std::endl;
    return false;
  }

  // inkrementace pocitane reference
  ::sach [ (int)pozice.radek ][ (int)pozice.sloupec ].figurka->pReference++;
  return true;
}

/**
 * Usporne zobrazeni (vypsani) sachovnice
 * Pouzivano pri vyvoji
 */
void zobrazSachovniciShort ( void ) {
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      if ( ::sach[ radek ][ sloupec ].figurka != NULL ){
        printw("typ:%d  barva:%d  pozice:", ::sach[ radek ][ sloupec ].figurka->getTypFigurky(), ::sach[ radek ][ sloupec ].figurka->getBarva() );
        ::sach[ radek ][ sloupec ].figurka->getPozice().vypis();
        printw("\n");
      }
    }
  }
}

/**
 * Vykresli pres ncurses celou sachovnici s rozmistenyma figurkama
 */
void zobrazSachovnici ( void ) {
  clear();
    
  // horni okraj sachovnice
  printw( "         A       B       C       D       E       F       G       H\n" );
  printw( "     " );
  addch(ACS_ULCORNER); 
  for (int sl = 0; sl < 8; sl ++) {
    for (int i = 0; i < 7; i++ ) {
      addch(ACS_HLINE);
    }
    if ( sl == 7 ) 
      addch(ACS_URCORNER);
    else
      addch(ACS_TTEE);
  }
  printw("\n");
 
  // obsah sachovnice
  for (int radek = 0; radek < 8; radek++) {
    for (int p = 0; p <= 2; p++) {
      
      // leve odsazeni a cislovani radek po stranach sachovnice
      if(p == 1)
        printw( "   %d ",  4 - (radek - 4) );
      else 
       printw( "     " );

      addch(ACS_VLINE); // vertical line
      
      // vykresleni figurek
      for (int sloupec = 0; sloupec < 8; sloupec++) {
        if ( ::sach[ radek ][ sloupec ].figurka == NULL){
          if ( (( radek + sloupec) % 2) == 0 ) 
            for ( int i = 0; i < 7; i++)
              addch(ACS_BULLET);
          else
            printw( "       " );
        }
        else
          ::sach[ radek ][ sloupec ].figurka->vykresli(p);
        
        addch(ACS_VLINE); // vertical line
      }

      // napravo cislovani radek
      if(p == 1)
        printw( " %d", 4 - (radek - 4));
 
     printw("\n");
    }

    // radek mezi jednotlivymi 8 radky sachovnice
    printw( "     " );
    if ( radek != 7 ) { 
      addch(ACS_LTEE);
      for (int sl = 0; sl < 8; sl ++) {
        for (int i = 0; i < 7; i++ ) {
          addch(ACS_HLINE);
        }
        if ( sl == 7 ) 
          addch(ACS_RTEE);
        else
          addch(ACS_PLUS);
      }
      printw("\n");
    }
    // spodni okraj sachovnice
    else {
      addch(ACS_LLCORNER);
      for (int sl = 0; sl < 8; sl ++) {
        for (int i = 0; i < 7; i++ ) {
          addch(ACS_HLINE);
        }
        if ( sl == 7 ) 
          addch(ACS_LRCORNER);
        else
          addch(ACS_BTEE);
      }
      printw( "\n         A       B       C       D       E       F       G       H\n" );
    }
  }
  printw("> ");
  refresh();
  // napise male info nahoru do praveho sloupce
  wmove ( ::pravySloupec, 0, 0 );
  wprintw ( ::pravySloupec, "Prikazy:\n /novaHra\n /konec\n");
  if ( verbose >= 1)
    wprintw ( ::pravySloupec, "Dostupne tahy:\n");
  wrefresh ( ::pravySloupec );
}

/**
 * Uklidi a rozmisti figurky po sachovnici na novou hru
 */
void sachNovaHra( void ) {
  cleanPoolSach();
 
  if ( ! ( umistiFigurkuNaSachovnici( PESAK   , BILA  , "a2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "b2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "c2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "d2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "e2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "f2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "g2" ) 
        && umistiFigurkuNaSachovnici( PESAK   , BILA  , "h2" ) 
        && umistiFigurkuNaSachovnici( VEZ     , BILA  , "a1" ) 
        && umistiFigurkuNaSachovnici( KUN     , BILA  , "b1" ) 
        && umistiFigurkuNaSachovnici( STRELEC , BILA  , "c1" ) 
        && umistiFigurkuNaSachovnici( DAMA    , BILA  , "d1" ) 
        && umistiFigurkuNaSachovnici( KRAL    , BILA  , "e1" ) 
        && umistiFigurkuNaSachovnici( STRELEC , BILA  , "f1" ) 
        && umistiFigurkuNaSachovnici( KUN     , BILA  , "g1" ) 
        && umistiFigurkuNaSachovnici( VEZ     , BILA  , "h1" )
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "a7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "b7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "c7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "d7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "e7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "f7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "g7" ) 
        && umistiFigurkuNaSachovnici( PESAK   , CERNA , "h7" ) 
        && umistiFigurkuNaSachovnici( VEZ     , CERNA , "a8" ) 
        && umistiFigurkuNaSachovnici( KUN     , CERNA , "b8" ) 
        && umistiFigurkuNaSachovnici( STRELEC , CERNA , "c8" ) 
        && umistiFigurkuNaSachovnici( DAMA    , CERNA , "d8" ) 
        && umistiFigurkuNaSachovnici( KRAL    , CERNA , "e8" ) 
        && umistiFigurkuNaSachovnici( STRELEC , CERNA , "f8" ) 
        && umistiFigurkuNaSachovnici( KUN     , CERNA , "g8" ) 
        && umistiFigurkuNaSachovnici( VEZ     , CERNA , "h8" ) ) )
    errorMsg( "Chyba pri rozmistovani figur." );

  /* 
  if ( ! ( umistiFigurkuNaSachovnici( DAMA   , BILA  , "c5" ) 
        && umistiFigurkuNaSachovnici( KRAL    , BILA  , "e1" ) 
        && umistiFigurkuNaSachovnici( DAMA    , BILA , "a7" ) 
        && umistiFigurkuNaSachovnici( KRAL    , CERNA , "e8" ) 
        //&& umistiFigurkuNaSachovnici( KUN , CERNA , "b8" ) 
        && umistiFigurkuNaSachovnici( STRELEC   , CERNA , "c3" ) ) )
    errorMsg( "Chyba pri rozmistovani figur." );
  */

  // nastaveni globalnich promennych kralu obou hracu
  ::bilyKral = ::sach[ 7 ][ 4 ].figurka;
  ::cernyKral = ::sach[ 0 ][ 4 ].figurka;

  pocNa50Tahu = 0;
}

/**
 * Uklidi po celem programu, vola se pred ukoncenim aplikace
 */
void cleanPool ( void ) {
  cleanPoolSach();  
  endwin(); // ukonci ncurses
  std::cout << "Bye, bye ..." << std::endl;
}

/**
 * Prepocita ::sach[][].dosah[] .. kazde policko sachocnice ma pole ukazatelu na vsechny figurky, ktere na toto
 * policko mohou rovnou utocne nebo v pripade potreby normalne tahnout
 */
void sachKrytiPolicek ( void ) {
  // vynuluje adresy dosahu/kryti figur
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      ::sach[ radek ][ sloupec ].dosah.clear();
    }
  }

  tah_t *mozneTahy;

  // projde celou sachovnici a zaznamena dosahy/kryti policka sachovnice figurkama
  for (int radek = 0; radek < 8; radek++) {
    for (int sloupec = 0; sloupec < 8; sloupec++) {
      if ( ::sach[ radek ][ sloupec ].figurka != NULL) {
        if ( ::sach[ radek ][ sloupec ].figurka->mozneTahy( mozneTahy, true ) ) {
          sachKrytiPolicek ( mozneTahy ); 
          delete [] mozneTahy;
        }
      }
    }
  }
}

/**
 * Funkce zaznamena do sachovnice vsechny mozne utocne i kryci (kryti vlastnich figurek) tahy z pole tahy
 */
void sachKrytiPolicek ( const tah_t * tahy ) {
  while ( ! tahy->prazdnyTah() ) {
    ::sach[ (int)tahy->kam.radek ][ (int)tahy->kam.sloupec ].dosah.push_back( ::sach[ (int)tahy->odkud.radek ][ (int)tahy->odkud.sloupec ].figurka );
    tahy++;
  }
}


/**
 * Otestuje zda dana pozice existuje na sachovnici
 */
bool sachPolicko ( const pozice_t& pozice ) {
  return ( pozice.radek >= 0 && pozice.radek <= 7 && pozice.sloupec >= 0 && pozice.sloupec <= 7 );
}

/**
 * Otestuje zda policko na dane pozici na sachovnici je prazdne (nestoji na nem zadna figurka)
 */
bool sachVolnePolicko( const pozice_t& pozice ) {
  return ( sachPolicko ( pozice )  &&  ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka == NULL );
}

/**
 * Otestuje zda na dane pozici stoji figurka protihrace.
 * @param pozice pozice, ktera se ma otestovat
 * @param barvaHrace barva figurky, ktera testovani vyvolala.. (hleda se opacna barva)
 * @param kryti true aktivuje rezim kryti i vlastnich figurek
 * @param return pokud na dane pozici figurka protihrace stoji vrati true, jinak false
 */
bool sachPolickoSFigurkouProtihrace ( const pozice_t& pozice, bool barvaHrace, bool kryti ) {
  return ( sachPolicko ( pozice )  &&  ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka != NULL
  &&  ( kryti || ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka->getBarva() != barvaHrace ) );
} 

/**
 * Testuje zda je dana pozice volna nebo na ni stoji figurka protihrace. Nesmi tam byt figurka hrace
 * @param pozice pozice, ktera se testuje
 * @param barvaHrace barva hrace, ktery vyvolal tuto funkci
 * @param kryti true aktivuje rezim kryti i vlastnich figurek
 * @param return vraci true pokud je pozice volna nebo na ni stoji figurka protihrace. Jinak false
 */
bool sachVolnePolickoNeboProtihrace ( const pozice_t& pozice, bool barvaHrace, bool kryti ) {
  return ( sachPolicko ( pozice ) 
        && ( ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka == NULL
          || ( ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka != NULL
             &&  ( kryti || ::sach[ (int) pozice.radek ][ (int) pozice.sloupec ].figurka->getBarva() != barvaHrace )
             )
           )
         );
}

/**
 * Zjistuje zda je na dane pozici umistena vez, se kterou lze provest rosadu.
 * @param p pozice, na ktere se hleda vez
 * @param barvaHrace barva hrace (barva krale)
 * @param return vraci true pokud se tam nachazi vez se kterou by rosadu mozna slo provest, jinak false
 */
bool sachRosadovaVez ( const pozice_t& p, bool barvaHrace ) {
  return ( ::sach[ (int)p.radek ][ (int)p.sloupec ].figurka != NULL
        && ::sach[ (int)p.radek ][ (int)p.sloupec ].figurka->getBarva() == barvaHrace
        && ::sach[ (int)p.radek ][ (int)p.sloupec ].figurka->getTypFigurky() == VEZ
        && ::sach[ (int)p.radek ][ (int)p.sloupec ].figurka->getKolikratTazeno() == 0 );
}

void premenPesceNaKonciSachovnice ( void ) {
  // hledam bily pesaky
  for ( int s = 0; s < 8; s++ )
    if ( ::sach[ 0 ][ s ].figurka != NULL && ::sach[ 0 ][ s ].figurka->getBarva() && ::sach[ 0 ][ s ].figurka->getTypFigurky() == PESAK ) {
      // pesaka uvolnime
      ::sach[ 0 ][ s ].figurka->vyreseniReferenciSebranaFigurka();
      delete ::sach[ 0 ][ s ].figurka;
      
      // vytvorime damu a vlozime ji do sachovnice na misto pesaka
      TFigurka<CDama> *ptr = new TFigurka<CDama>( pozice_t( (char)0, (char)s ), BILA );
      ::sach [ 0 ][ s ].figurka = ptr;      

      // inkrementace pocitane reference
      ::sach [ 0 ][ s ].figurka->pReference++;
      return;
    }

  // hledam cerne pesaky
  for ( int s = 0; s < 8; s++ )
    if ( ::sach[ 7 ][ s ].figurka != NULL && !::sach[ 7 ][ s ].figurka->getBarva() && ::sach[ 7 ][ s ].figurka->getTypFigurky() == PESAK ) {
      // pesaka uvolnime
      ::sach[ 7 ][ s ].figurka->vyreseniReferenciSebranaFigurka();
      delete ::sach[ 7 ][ s ].figurka;
      
      // vytvorime damu a vlozime ji do sachovnice na misto pesaka
      TFigurka<CDama> *ptr = new TFigurka<CDama>( pozice_t( (char)7, (char)s ), CERNA );
      ::sach [ 7 ][ s ].figurka = ptr;      

      // inkrementace pocitane reference
      ::sach [ 7 ][ s ].figurka->pReference++;
      return;
    }
} 

/**
 * Funkce rozlisi jestli se jedna o sach mat ci sach pat ci nikoliv
 * @param t tah, ktery vygeneroval pocitac
 * @param barva barvaHrace pro ktereho je tah vygenerovan
 * @param return pokud je sach mat ci pat, vrati true, jinak false
 */
bool rozpoznejSachMatPat ( const tah_t& t, bool barva ) {
  if ( t.prazdnyTah() ) {
    std::vector<CFigurka*>::iterator ii;
    // bily nemuze nijak tahnout -> cerny vyhral nebo remiza
    if ( barva ) {
      bool pat = true;
      /*      
      for ( int i = 0; i < MAX_POCET_FIGUREK_NA_1_POLICKO; i++ ) {
        if ( ::sach[ (int)::bilyKral->getPozice().radek ][ (int)::bilyKral->getPozice().sloupec ].dosah[ i ] != NULL ) {
          if ( ! ::sach[ (int)::bilyKral->getPozice().radek ][ (int)::bilyKral->getPozice().sloupec ].dosah[ i ]->getBarva() ) {
            pat = false;
            break;
          }
        }
        else
          break;
      }
      */
      std::vector<CFigurka*> * d = & ::sach[ (int)::bilyKral->getPozice().radek ][ (int)::bilyKral->getPozice().sloupec ].dosah;
      //ii = ::sach[ (int)::bilyKral->getPozice().radek ][ (int)::bilyKral->getPozice().sloupec ].dosah::iterator;
      for ( ii = d->begin(); ii != d->end(); ii++ ) {
        if ( *ii == NULL )
          break;
        if ( ! (*ii)->getBarva() ) {
          pat = false;
          break;
        }
      }


      if ( pat ) 
        setPrikazovyRadek( "Remiza. Od pocitace jste dostal sach pat." );
      else
        setPrikazovyRadek( "Pocitac vam dal sach mat. Prohral jste." );
    }
    // vyhral clvovek
    else {
      bool pat = true;
      std::vector<CFigurka*> * d = & ::sach[ (int)::cernyKral->getPozice().radek ][ (int)::cernyKral->getPozice().sloupec ].dosah;
      for ( ii = d->begin(); ii != d->end(); ii++ ) {
        if ( *ii == NULL )
          break;
        if ( (*ii)->getBarva() ) {
          pat = false;
          break;
        }
      }

      if ( pat )
        setPrikazovyRadek( "Remiza. Dal jste pocitaci sach pat." );
      else
        setPrikazovyRadek( "VITEZITE! Pocitac dostal sach mat." );
    }
    refresh();
    getch();
    return true;
  }
  return false;
}

/**
 * Rozpozna zda nastala tzv. mrtva pozice. To znamena, ze na obou stranach muze byt jen
 *  1) kral (+ strelec)
 *  2) kral (+ kun (+ kun))
 *  3) kral (+ strelec + kun)
 *  @param return true pokud mrtva pozice nastala
 */
bool mrtvaPozice ( void ) {
  int kB = 0, kC = 0, sB = 0, sC = 0;
  for ( int r = 0; r < 8; r++) {
    for (int s = 0; s < 8; s++ ) {
      if ( ::sach[r][s].figurka != NULL ) {
        switch ( ::sach[r][s].figurka->getTypFigurky() ) {
          case KRAL :
            break;
          case STRELEC :
            if ( ::sach[r][s].figurka->getBarva() )
              sB++;
            else
              sC++;
            break;
          case KUN :
            if ( ::sach[r][s].figurka->getBarva() )
              kB++;
            else
              kC++;
            break;
          default :
            return false;
        }
      } 
    }  
  }
  
  // mrtva pozice
  if ( ( ( sB <= 1 && kB <= 1 ) || ( sB == 0 && kB == 2 ) ) && ( ( sC <= 1 && kC <= 1) || ( sC == 0 && kC == 2 ) ) ) {
    setPrikazovyRadek( "Remiza. Nastala mrtva pozice." );
    refresh();
    getch();
    return true;
  }
  else
    return false;
}


/**
 * Precte z klavesnice prikaz ukonceny enterem
 * @param vstup sem zapise vstup z klavesnice
 * @param len maximalni delka vstupu vcetne ukoncovaciho nuloneho bajtu
 * @param return pokud je vstup tah, vraci tah; pokud je vstup prikaz '/neco' vrati prazdnyTah()
 */
tah_t ctiVstup ( char * vstup, int len = 10 ) {
  char ch;
  int p = 0;
  setPrikazovyRadek();
  printw("> ");
  while((ch = getch()) != 10) { // dokud se nezmackne enter
    switch( ch ) {
      case 27: // esc
        vstup [ p = 0 ] = '\0';
      case 127: // backspace
        if ( p > 0) {
          vstup[--p] = '\0';
        }
      default:
        if ( p < (len-1) && ( ( ch>='0' && ch<='9' ) || ( ch>='a' && ch<='z')  || ( ch>='A' && ch<='Z' ) || ch == '/') ) {
          vstup[p++] = ch;
          vstup[p] = '\0';
          attron(A_BOLD);
          printw("%c", ch);
          attroff(A_BOLD);
        }

        printw ("\r");
        for (int i = 0; i <= len + 20; i++ )
          printw(" ");
        printw ("\r> ");
        attron(A_BOLD);
        printw("%s", vstup);
        attroff(A_BOLD);
        break;  
    }
  }
  vstup[ p ] = '\0';
  if ( p == 4 ) {
    pozice_t p1 = pozice_t ( vstup[1], vstup[0] );
    pozice_t p2 = pozice_t ( vstup[3], vstup[2] );
    if ( sach2pozice ( &p1 ) && sach2pozice( &p2 ) && !(p1 == p2) ) 
      return ( tah_t ( p1 , p2) ); 
  }
  if ( vstup[0] == '/')
    return ( tah_t() ) ;
  
  setPrikazovyRadek("Neplatny vstup.");
  return ( ctiVstup( vstup, len ) );
}

/**
 * Porovna dva retezce
 * @param return pokud se retezce shoduji, vrati true, jinak false
 */
bool porovnejRetezce ( const char * r1, const char * r2 ) {
  int i = 0;
  while ( r1[ i ] != '\0' && r2[ i ] != '\0' ) {
    if ( r1[ i ] != r2[ i ] )
      return false;
    i++;
  }
  return true;
}


/**
 * Zobrazi informativni upozorneni v retezci pod prikazovou radku
 */
void setPrikazovyRadek ( const char * s ) {
  setPrikazovyRadek();  
  printw ("> \n                                               \r  ");
  attron(A_BOLD);
  printw("%s", s);
  attroff(A_BOLD);
}

/**
 * Nastavi ncurses kurzor na spodni prikazovy radek
 */
void setPrikazovyRadek ( void ) {
  move (35, 0); 
}

/** 
 * @param res retezec do ktereho zapise celou vetou, ze pozitac tahl na pozici ....
 * @param barva parametr urcuje, jakou barvou figurek ma ted zahrat
 * @param return vrati nejlepsi tah. Pokud nemuze nikam tahnout, vrati prazdny tah
 */
tah_t jakByTahlPocitac ( char* res, bool barva) {
  int bestScore = barva ? -2000000000 : 2000000000;
  int score = bestScore;
  tah_t bestTah = tah_t(),
        tah = tah_t();
  
  wmove( ::pravySloupec, 4,0);
  
  for (int r = 0; r < 8; r++ ) {
    for (int s = 0; s < 8; s++) {
      tah_t *mozneTahy;
      if ( ::sach[r][s].figurka != NULL && ::sach[r][s].figurka->getBarva() == barva && ::sach[r][s].figurka->mozneTahy( mozneTahy, false ) ) {
        //vypisTahy ( mozneTahy );
        //wprintw(::pravySloupec, "| ");
        najdiNejlepsiTah( mozneTahy, tah, score, barva );
        if ( ( score > bestScore && barva ) || ( score < bestScore && !barva ) ){
          bestTah = tah;
          bestScore = score;
        }
        delete [] mozneTahy;
      }
    }
  }
  
  if ( verbose != 0 ) { 
    wmove(::pravySloupec,LINES-2,0 );
    wprintw(::pravySloupec, "\nbest:");
    bestTah.vypis();
    wprintw(::pravySloupec, " %d\n", bestScore);
    wrefresh( ::pravySloupec );
  }

  // Vytvori se veta, jak by pocitac zahral tah
  tah = bestTah;
  char strTah[5] = "";
  if ( ! ( pozice2sach( &tah.odkud ) && pozice2sach( &tah.kam ) ) )
    wprintw(::pravySloupec, "\n\nSPATNA POZICE!!\n");
  else {
    strTah[0] = tah.odkud.sloupec;
    strTah[1] = tah.odkud.radek;
    strTah[2] = tah.kam.sloupec;
    strTah[3] = tah.kam.radek;
    strTah[4] = '\0';
  }

  char sach[20] = "";
  if ( ! bestTah.prazdnyTah() ) {
    // zkusime si udelat tah, abychom zjistili jestli bude sach nebo nebude sach a pripadne ho nahlasime
    ::sach[ (int)bestTah.odkud.radek ][ (int)bestTah.odkud.sloupec ].figurka->tahni( bestTah , false );
    // propocitani aktualnich dosahu figurek
    sachKrytiPolicek();  
    if ( barva )
      sprintf(sach, "%s", ( ::cernyKral->dostavamSach() ) ? "Cerny dostal sach!" : "" );
    else
      sprintf(sach, "%s", ( ::bilyKral->dostavamSach() ) ? "Bily dostal sach!" : "" );
    sprintf(res, "%s tahl na pozici %s. %s", (barva ? "Bily" : "Cerny"), strTah, sach);
    ::sach[ (int)bestTah.kam.radek ][ (int)bestTah.kam.sloupec ].figurka->vrat( bestTah );
  }

  sprintf(res, "%s tahl na pozici %s. %s", (barva ? "Bily" : "Cerny"), strTah, sach);

  return bestTah;
}

/**
 * Z pole tahy vybere nejlepsi a zapiseho ho do bestTah
 * @param tahy vstupni tahy, ze kterych se ma vybrat nejlepsi
 * @param bestTah sem se zapise vybrany nejlepsi tah
 * @param bestScore pomyslne score nejlepsiho tahu
 * @param barva barva hrace, pro ktereho hledame nejlepsi tah
 * @param return never
 */
void najdiNejlepsiTah( const tah_t* tahy, tah_t& bestTah, int& bestScore, bool barva ) {
  int score;
  while ( ! tahy->prazdnyTah() ) {
    if ( ( ( score = ohodnotTah( *tahy ) ) > bestScore && barva ) || ( score < bestScore && !barva ) ) {
      bestTah = *tahy;
      bestScore = score;  
    }
    tahy++;
  }
}

/**
 * Ohodnoti tah, respektive pouze pozici po tahu. cim vyssi kladny score, tim lepe pro bileho,
 * cim nizsi zaporne score, tim lepe pro cerneho hrace.
 * Jedna se o nejspodnejsi cast, o jadro 'umele inteligence' pocitace :)
 * @param tah tah ktery se ma ohodnotit
 * @param return vrati pomyslne score tahu
 */
int ohodnotTah ( const tah_t& tah ) {

  if ( ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka == NULL ){
    errorMsg("ohodnotTah::neni tah zadna figurka!");
    return 0;
  }

  // to abychom vedeli kdo tim tahem tahl a kdo bude dal na rade
  bool barva = ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka->getBarva();

  ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka->tahni( tah, false );
  int score = 0;
  int hodnFigurky[7] = 
  { 0,      // nic
    100000, // pesak
    300000, // strelec
    500000, // vez
    300000, // kun
    900000, // dama
  10000000  // kral - pomerne nizka hodnota, aby se to nesnazilo za zbytecne vysokou cenu na to zautocit
  };

  // propocitani aktualnich dosahu figurek
  sachKrytiPolicek();  
 
  std::vector<CFigurka*>::iterator ii;

  for (int radek = 0; radek < 8; radek++ ) {
    for (int sloupec = 0; sloupec < 8; sloupec++ ) {

      // projedeme vecto dosah
      // absolutni hodnoty, kladne
      int pCernychKryti = 0, pBilychKryti = 0; 
      int minHodnCernych = 2000000000, minHodnBilych = minHodnCernych;

      std::vector<CFigurka*> * d = & ::sach[ radek ][ sloupec ].dosah;
      for ( ii = d->begin(); ii != d->end(); ii++ ) {
        if ( *ii == NULL )
          break;

        if ( (*ii)->getBarva() ) {
          // pod dosahem bilym
          score += 5;
          pBilychKryti++;
          if ( minHodnBilych > hodnFigurky[ (int) (*ii)->getTypFigurky() ] ) 
            minHodnBilych = hodnFigurky[ (int) (*ii)->getTypFigurky() ];
        }
        else {
          // pod dosahem cernym
          score -= 5;
          pCernychKryti++;
          if ( minHodnCernych > hodnFigurky[ (int) (*ii)->getTypFigurky() ] ) 
            minHodnCernych = hodnFigurky[ (int) (*ii)->getTypFigurky() ];
        }
      }
      
      // hodnPolicka je +- podle barvy
      int hodnPolicka = 0;
      // ohodnotime figurku na policku
      if ( ::sach[ radek ][ sloupec ].figurka != NULL ) {
        
        // stojici bila figurka
        if ( ::sach[ radek ][ sloupec ].figurka->getBarva() ) {
          hodnPolicka += hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ];
          hodnPolicka -= (hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] / 1000)*pCernychKryti;
        }
        // stojici cerna figurka
        else {
          hodnPolicka -= hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ];
          hodnPolicka += (hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] / 1000)*pBilychKryti;
        }

        // ohodnotime pozici pesce -> radi bychom ho promenili v damu na konci sachovnice
        if ( ::sach[ radek ][ sloupec ].figurka->getTypFigurky() == PESAK ) {
          if ( ::sach[ radek ][ sloupec ].figurka->getBarva() )
            hodnPolicka += hodnFigurky[DAMA] / ((1 + radek)*(1 + radek)*(1 + radek)*(1 + radek));
          else
            hodnPolicka -= hodnFigurky[DAMA] / ((1 + (4 - (radek - 3)))*(1 + (4 - (radek - 3))*(1 + (4 - (radek - 3)))*(1 + (4 - (radek - 3)))));
        }
        
        // jemne linearni popostrceni figurek na druhou stranu sachovnice
        if ( ::sach[ radek ][ sloupec ].figurka->getBarva() )
          hodnPolicka += (4 - (radek - 3))*1;
        else
          hodnPolicka -= radek*1;

        score += hodnPolicka;
        
        // cerny napada nechranenou figurku bileho
        if ( pCernychKryti > 0 && pBilychKryti == 0 && ::sach[ radek ][ sloupec ].figurka->getBarva() && barva) {
          score -= ( hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] * 9 ) / 10;
        }        
        // bili napada nechranenou figurku cerneho
        if ( pCernychKryti == 0 && pBilychKryti > 0 && !::sach[ radek ][ sloupec ].figurka->getBarva() && !barva) {
          score += ( hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] * 9 ) / 10;
        }
        /*
        // cerny napada slabsi figurkou silnejsi chranenou figurku bileho
        if ( pCernychKryti > 0 && pBilychKryti > 0 && ::sach[ radek ][ sloupec ].figurka->getBarva() && barva )
          score -= ( hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] - minHodnCernych );
        
        // bily napada slabsi figurkou silnejsi chranenou figurku cerneho
        if ( pCernychKryti > 0 && pBilychKryti > 0 && !::sach[ radek ][ sloupec ].figurka->getBarva() && !barva )
          score += ( hodnFigurky[ (int) ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] - minHodnBilych );
        */
        /*
        // bila figurka stojici v cernem kryti a na rade je cerny hrac
        if ( ::sach[ radek ][ sloupec ].figurka->getBarva() && pCernychKryti > 0 && barva )
          score -= (hodnFigurky[ ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ]*5)/6 ;
         
        // bila figurka bez obrany stojici v cernem kryti a na rade je cerny hrac
        if ( ::sach[ radek ][ sloupec ].figurka->getBarva() && pCernychKryti > 0 && pBilychKryti == 0 && barva )
          score -= ( hodnFigurky[ ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] );
         

        // cerna figurka stojici v bilem kryti a na rade je bily hrac
        if( ! ::sach[ radek ][ sloupec ].figurka->getBarva() && pBilychKryti > 0 && !barva)
          score += (hodnFigurky[ ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ]*5)/6 ;

        // cerna figurka bez obrany stojici v bilem kryti a na rade je bily hrac
        if ( ::sach[ radek ][ sloupec ].figurka->getBarva() && pBilychKryti > 0 && pCernychKryti == 0 && !barva )
          score += ( hodnFigurky[ ::sach[ radek ][ sloupec ].figurka->getTypFigurky() ] );
         */
      }
    }
  }

  ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka->vrat( tah );
  
  if ( verbose != 0) { 
    wprintw(::pravySloupec, " ");
    tah.vypis();
    wprintw(::pravySloupec, " %d\n", score);
    wrefresh(::pravySloupec);
  }

  return score;
}

/**
 * Ncurses, vytvori okno
 */
WINDOW *create_newwin(int height, int width, int starty, int startx)
{  WINDOW *local_win;

  local_win = newwin(height, width, starty, startx);
  box(local_win, 0 , 0);    /* 0, 0 gives default characters 
           * for the vertical and horizontal
           * lines      */
  wrefresh(local_win);    /* Show that box     */
  wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  return local_win;
}

/**
 * Ncurses, zrusi okno
 */
void destroy_win(WINDOW *local_win)
{  
  /* box(local_win, ' ', ' '); : This won't produce the desired
   * result of erasing the window. It will leave it's four corners 
   * and so an ugly remnant of window. 
   */
  wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
  /* The parameters taken are 
   * 1. win: the window on which to operate
   * 2. ls: character to be used for the left side of the window 
   * 3. rs: character to be used for the right side of the window 
   * 4. ts: character to be used for the top side of the window 
   * 5. bs: character to be used for the bottom side of the window 
   * 6. tl: character to be used for the top left corner of the window 
   * 7. tr: character to be used for the top right corner of the window 
   * 8. bl: character to be used for the bottom left corner of the window 
   * 9. br: character to be used for the bottom right corner of the window
   */
  wrefresh(local_win);
  delwin(local_win);
}
