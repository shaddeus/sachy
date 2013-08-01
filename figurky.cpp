/* figurky.cpp */

#include "figurky.h"

/* ---------------------------------------------------------------------------*\
 *
 *   CFigurka
 *
\* ---------------------------------------------------------------------------*/
CFigurka::CFigurka ( void ) {
  CFigurka::sebranaFigurka = NULL;
  CFigurka::kolikratTazeno = 0;
  CFigurka::pReference = 0;
}

CFigurka::CFigurka ( pozice_t pozice, bool barva ) {
  CFigurka::pozice = pozice;
  CFigurka::barva = barva;
  CFigurka::kolikratTazeno = 0;
  CFigurka::sebranaFigurka = NULL;
  CFigurka::pReference = 0;
}

CFigurka::~CFigurka ( void ) {
}

bool CFigurka::mozneTahy ( tah_t * & mozneTahy, bool kryti ) {
  //debugMsg( "virtual CFigurka::mozneTahy()" );
  return false;
}

/** 
 * Rekurzivni bestie na niceni memory leaku.
 * Metoda rekurzivne uvolni nepotrebne figurky, ktere jsou na zasobniku sebranaFigurka
 */
void CFigurka::vyreseniReferenciSebranaFigurka( void ) {
  if ( this->sebranaFigurka != NULL ) {
    if ( this->sebranaFigurka->pReference <= 0 )  {
      // nema ta figurka v sebranych figurkach taky nekoho, koho je potreba jeste deletnout?
      this->sebranaFigurka->vyreseniReferenciSebranaFigurka();
      
      //wprintw(::pravySloupec,"delete: %d [%d][%d]\n", this->sebranaFigurka->getTypFigurky(), this->sebranaFigurka->getPozice().radek, this->sebranaFigurka->getPozice().sloupec);
      //wrefresh(::pravySloupec);
      
      delete this->sebranaFigurka;
    }
    this->sebranaFigurka = NULL;
  }
}

/**
 * Tahne figurkou.
 * @param tah tah odkud kam se ma figurkou tahnout
 * @param testPlatnosti pokud vime ze tah je urcite platny nastavime false, preskoci se test platnosti tahu
 * @param return v pripade uspesneho tahu navraci true, jinak false
 */
bool CFigurka::tahni ( const tah_t & tah, bool testPlatnosti = true ) {
  //debugMsg ( "virtual CFigurka::tahni()" );
  
  if (  testPlatnosti ) {
    // overime zda tah je platny (mozny)
    if ( ! this->overeniPlatnostiTahu ( tah ) )
      return false;
    // zde uz vime, ze tah je skutecne platny
  }

  // s figurkou se bude tahnout, minuly tah nepujde uz vratit. 
  // uvolnime vsechny sebrane jeste neuvolnene figurky
  this->vyreseniReferenciSebranaFigurka();

  // tahneme figurkou
  ::sach[ (int)this->pozice.radek ][ (int)this->pozice.sloupec ].figurka = NULL;
  this->pozice = tah.kam;
  if ( ( this->sebranaFigurka = ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka ) != NULL ){
    //wprintw(::pravySloupec,"\nsebranaFigurka1.2:%d [%d][%d]\n", this->sebranaFigurka->getTypFigurky(), this->sebranaFigurka->getPozice().radek, this->sebranaFigurka->getPozice().sloupec, this->sebranaFigurka->pReference);
    //wrefresh(::pravySloupec);
    //getch();

    this->sebranaFigurka->pReference--;
  }
  
  ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this;
  this->kolikratTazeno++;

  return true;
}

/**
 * Vrati predchozi tah figurkou.
 * @param tah tam odkud kam se tahlo
 * @param return NEVER
 */
void CFigurka::vrat ( const tah_t & tah ) {
  //debugMsg( "virtual CFigurka::vrat()" );
  
  ::sach[ (int)(this->pozice.radek = tah.odkud.radek) ][ (int)(this->pozice.sloupec = tah.odkud.sloupec) ].figurka = this;
  if ( ( ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this->sebranaFigurka ) != NULL ) {
    this->sebranaFigurka->pReference++;
    this->sebranaFigurka = NULL;
    //wprintw(::pravySloupec,"\nvracena sebranaFigurka1.2:%d [%d][%d]\n", this->sebranaFigurka->getTypFigurky(), this->sebranaFigurka->getPozice().radek, this->sebranaFigurka->getPozice().sloupec, this->sebranaFigurka->pReference);
    //wrefresh(::pravySloupec);
    //getch();
 
  }
    
  /*
  if ( ( ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this->sebranaFigurka ) != NULL ) {
    if ( this->sebranaFigurka->pReference-- <= 1 ) {
      wprintw(::pravySloupec,"\ndelete:%d [%d][%d]\n", this->sebranaFigurka->getTypFigurky(), this->sebranaFigurka->getPozice().radek, this->sebranaFigurka->getPozice().sloupec);
      delete this->sebranaFigurka;
      pocNa50Tahu = 0;
    }
    this->sebranaFigurka = NULL;
  }
  */

  this->kolikratTazeno--;
}

bool CFigurka::getEnPassant ( void ) const {
  //debugMsg( "virtual CFigurka::getEnPassant()" );
  return false;
}

/**
 * Metoda proveri zda je mozne figurkou udelat dany tah i vzhledem k ostatnim figurkam
 * @param tah proverovany tah
 * @param return true pokud tah lze provest, false pokud tah nelze provest
 */
bool CFigurka::overeniPlatnostiTahu ( const tah_t& tah ) {
  //debugMsg( "virtual CFigurka::overeniPlatnostiTahu()" );
  tah_t *mozneTahy;
  if ( ! this->mozneTahy ( mozneTahy, false ) ) {
    //debugMsg( "CFigurka::overeniPlatnostiTahu() s figurou nelze nikam tahnout" );
    return false;
  }
  if ( ! jeTahMeziTahy ( tah, mozneTahy ) ) {
    //debugMsg (  "CFigurka::overeniPlatnostiTahu() pokus o neplatny tah s figurou" );
    delete [] mozneTahy;
    return false;
  }
  delete [] mozneTahy; 
  return true;
}

/**
 * Specifická funkce uzita ve FOR cyklech pri hledani moznych tahu figurek: DAMA, VEZ, STRELEC
 * Zjistuje zda je mozne presunout (resp. vzit cizi) figurku na danem policku sachovnice.
 * Na policko s protihracovou figurou, lze tahnout. Ale na pole s vlastni figurou tahnout nelze.
 * @param poz policko, ktere se ma otestovat
 * @param mozneTahy pole tahu, do ktereho se pripadne policko prida
 * @param pTahy pocitadlo tahu, v pripade ze lze tahnout, inkrementuje se o jednicku
 * @param kryti true aktivuje rezim kryti i vlastnich figurek
 * @param return vraci false jako priznak na predcasne ukonceni FOR cyklu (zbytek tahu je uz zastinenych nejakou figurkou)
 */
bool CFigurka::pujdeTahnoutNaPolicko ( const pozice_t & poz, tah_t * & mozneTahy, int &pTahy, bool kryti ) {
  //debugMsg( "virtual CFigurka::pujdeTahnoutNaPolicko()" );
  
  if ( sachVolnePolicko( poz ) ) {
    if ( kryti || this->kralPoTahuNebudeNapaden( poz )  )
     mozneTahy[pTahy++].kam = poz;
    
    return true;
  }
  else if ( sachPolickoSFigurkouProtihrace ( poz, this->barva, kryti )
    && ( kryti || this->kralPoTahuNebudeNapaden( poz ) ) ) {
    mozneTahy[pTahy++].kam = poz;
    return false;
  }
  else
    return false;
}

/**
 * Vykresli pomoci ncurses zadanou radku figurky
 * @param radek urcuje ktery radek z figurky se ma vykreslit
 */
void CFigurka::vykresli ( const int radek ) const {
  const char grFigurky[6][3][8] = 
  { 
    {
    "   _   ",
    "  (#)  ",
    "  /#\\  "
    },
    {
    "  /\\\\  ",
    "  \\#/  ",
    " ($$$) "
    },
    {
    " [_|_] ",
    "  [#]  ",
    " ($$$) "
    },
    {
    " /_^)\\ ",
    "  /#\\\\ ",
    " ($$$) "
    },
    {
    "  \\*/  ",
    "  (#)  ",
    " ($$$) "
    },
    {
    "  \\+/  ",
    "  {#}  ",
    " ($$$) "
    }
 };
  
  int f = (int)this->getTypFigurky() - 1;
  
  for( int i = 0; i < 7; i++ ) {
    if ( grFigurky[ f ][ radek ][ i ] == ' ' && ((this->pozice.radek + this->pozice.sloupec) % 2) == 0 )
      addch(ACS_BULLET);
    else if ( ( grFigurky[ f ][ radek ][ i ] == ' ' && ((this->pozice.radek + this->pozice.sloupec) % 2) != 0 )
           || ( grFigurky[ f ][ radek ][ i ] == '#' &&  ! (this->getBarva()) ) )
      addch(' ');
    else if ( ( grFigurky[ f ][ radek ][ i ] == '#' &&  this->getBarva() )
           || ( grFigurky[ f ][ radek ][ i ] == '$' &&  this->getBarva() ) )
      addch('#');
    else if ( grFigurky[ f ][ radek ][ i ] == '$' &&  ! (this->getBarva()) )
      addch('_');
    else
      addch( grFigurky[ f ][ radek ][i] );
  }
}

/**
 * Funkce proveri zda hrac ma svoji figurku krale v sachu od protihrace (true), ci nikoli (false)
 */
bool CFigurka::dostavamSach ( void ) const {
  //debugMsg("CFigurka::dostavamSach");
  pozice_t p;
  if ( this->getBarva() )
    p = ::bilyKral->getPozice();
  else
    p = ::cernyKral->getPozice();

  std::vector<CFigurka*>::iterator ii;
  std::vector<CFigurka*> * d = & ::sach[ p.radek ][ p.sloupec ].dosah;
  for ( ii = d->begin(); ii != d->end(); ii++ ) {
    if ( *ii == NULL )
      break;
    else if ( (*ii)->getBarva() != this->getBarva() )
      return true;
  }
  return false;
}

/**
 * Funkce zkusi s figurkou tahnout, overi se zda kral neni napaden nejakou protihracovou figurou a tah vrati zpet
 * @param return vraci true pokud kral po tahu nebude napaden. pokud na paden bude, false
 */
bool CFigurka::kralPoTahuNebudeNapaden ( const pozice_t& pozKam ) {
  //debugMsg("CFigurka::kralPoTahuNebudeNapaden");
  tah_t tah = tah_t( this->getPozice(), pozKam);
  this->tahni( tah, false ); // bez testu platnosti, jinak by se to zacyklilo
  sachKrytiPolicek();
  bool r = !( this->dostavamSach() );
  this->vrat( tah );
  return r;
}

/**
 * Vypise zakladni udaje o figurce
 */
std::ostream& operator<< (std::ostream& o, const CFigurka& p) {
  o << "figurka:" << (int)p.getTypFigurky() << "  barva:" << p.getBarva() << "  pozice:" << p.getPozice();
  o << "   kolikratTazeno:" << p.kolikratTazeno << std::flush;
  return o;
}


/* ---------------------------------------------------------------------------*\
 *
 *   CPesak : CFigurka
 *
\* ---------------------------------------------------------------------------*/
CPesak::CPesak ( pozice_t pozice, bool barva ) : CFigurka ( pozice, barva ) {
  typFigurky = (char)PESAK;
  kolikratTazeno = 0;
  enPassant = false;
  sebranaFigurkaPozice = 0;
}

CPesak::~CPesak ( void ) {
}

/**
 * Najde vsechny tahy, ktere muze dana figurka prave provest
 * @param mozneTahy vytvori a zapise seznam moznych tahu + naraznik tah z policka A8 na A8
 * @param kryti true aktivuje rezim kryti i vlastnich figurek
 * @param return v pripade, ze nenajde ani jeden tah, vrati false
 */
bool CPesak::mozneTahy ( tah_t * & mozneTahy, bool kryti ) {
  // vnitrni smer, bily pesak X cerny pesak
  int smer = CPesak::barva ? -1 : 1;
 
  // tah z A8 na A8 slouzi jakoby naraznik pri prochazenim pole mozneTahy, proto musime vzdy
  // alokovat jeden tah navic nez by bylo v nejkrajnejsim pripade potreba
  mozneTahy = new tah_t [ 4 + 1 ];

  int pTahy = 0; // pocet moznych tahu
 
  // tah pesaka dopredu o jedno policko
  if ( !kryti && sachVolnePolicko( CPesak::pozice + pozice_t( smer, 0 ) )
    && ( this->kralPoTahuNebudeNapaden( CPesak::pozice + pozice_t( smer, 0 ) ) ) ) {
    mozneTahy[pTahy++].kam = CPesak::pozice + pozice_t( smer, 0 );

    // tah pesaka dopredu o dve policka
    if ( ( CPesak::kolikratTazeno == 0 ) && sachVolnePolicko( CPesak::pozice + pozice_t( 2*smer, 0 ) )
      && ( kryti || this->kralPoTahuNebudeNapaden( CPesak::pozice + pozice_t( 2*smer, 0 ) ) ) )
      mozneTahy[pTahy++].kam = CPesak::pozice + pozice_t( 2*smer, 0 );
  }
  
  // brani doleva dopredu
  if ( ( sachPolickoSFigurkouProtihrace( CPesak::pozice + pozice_t( smer, -1 ), CPesak::barva, kryti )
      || ( kryti && sachPolicko (CPesak::pozice + pozice_t( smer, -1 ) ) )
    // brani protihracoveho pesce nalevo 'en passant'
      || (sachPolickoSFigurkouProtihrace( CPesak::pozice + pozice_t( 0, -1 ), CPesak::barva, false ) 
         && ( ::sach[ (int) pozice.radek ][ (int) pozice.sloupec - 1 ].figurka->getTypFigurky() == (char)PESAK )
         &&   ::sach[ (int) pozice.radek ][ (int) pozice.sloupec - 1 ].figurka->getEnPassant()  ) ) 
    && ( kryti || this->kralPoTahuNebudeNapaden( CPesak::pozice + pozice_t( smer, -1 ) ) ) ) 
    mozneTahy[pTahy++].kam = CPesak::pozice + pozice_t( smer, -1 );

  // brani doprava dopredu
  if ( ( sachPolickoSFigurkouProtihrace( CPesak::pozice + pozice_t( smer, 1 ), CPesak::barva, kryti ) 
      || ( kryti && sachPolicko (CPesak::pozice + pozice_t( smer, 1 ) ) )
    // brani protihracoveho pesce napravo 'en passant'
      || (sachPolickoSFigurkouProtihrace( CPesak::pozice + pozice_t( 0, 1 ), CPesak::barva, false ) 
         && ( ::sach[ (int) pozice.radek ][ (int) pozice.sloupec + 1 ].figurka->getTypFigurky() == (char)PESAK )
         &&   ::sach[ (int) pozice.radek ][ (int) pozice.sloupec + 1 ].figurka->getEnPassant()  ) )
    && ( kryti || this->kralPoTahuNebudeNapaden( CPesak::pozice + pozice_t( smer, 1 ) ) ) ) 
    mozneTahy[pTahy++].kam = CPesak::pozice + pozice_t( smer, 1 );

  // pokud nelze figurkou nikam tahnout, deletneme pole a vratime false
  if ( pTahy == 0 ) {
     delete [] mozneTahy;
     return false;
  }

  // doplneni tabulky mozneTahy o vychozi pozici tahu odkud se tahne
  while ( pTahy )
    mozneTahy[ --pTahy ].odkud = CPesak::pozice;

  return true;
}

/**
 * Tahne figurkou.
 * @param tah tah odkud kam se ma figurkou tahnout
 * @param testPlatnosti pokud vime ze tah je urcite platny nastavime false, preskoci se test platnosti tahu
 * @param return v pripade uspesneho tahu navraci true, jinak false
 */
bool CPesak::tahni ( const tah_t& tah, bool testPlatnosti = true ) {
  //debugMsg( "CPesak::tahni()" );
  
  pocNa50Tahu = 0;

  if ( testPlatnosti ) {
    // overime zda tah je platny (mozny)
    if ( ! CPesak::overeniPlatnostiTahu ( tah ) )
      return false;
    // zde uz vime, ze tah je skutecne platny
  }
  
  // tahnuti figurkou
  ::sach[ (int)CPesak::pozice.radek ][ (int)CPesak::pozice.sloupec ].figurka = NULL;
  CPesak::pozice = tah.kam;
  CPesak::enPassant = false;
  CPesak::kolikratTazeno++;
  
  // s figurkou se bude tahnout, minuly tah nepujde uz vratit. 
  // uvolnime vsechny sebrane jeste neuvolnene figurky
  this->vyreseniReferenciSebranaFigurka();
 
  // pesak muze brat protihracovi figury pouze tahem do strany 
  if ( tah.odkud.sloupec == tah.kam.sloupec ) {
    // prvy skok pres dve policka
    if ( ( tah.odkud.radek == (char)1 && tah.kam.radek == (char)3 )
      || ( tah.odkud.radek == (char)6 && tah.kam.radek == (char)4 ) ) {
      CPesak::enPassant = true;
    }
  }

  // figurou se tahne do strany -> bere se protihracova figura;
  // pokud se tahne do strany, ale na volne pole sachovnice -> brani mimochodem / en passant
  else if ( ( CPesak::sebranaFigurka = ::sach[ (int)CPesak::pozice.radek ][ (int)CPesak::pozice.sloupec ].figurka ) == NULL ) {
    // brani en passant
    CPesak::sebranaFigurka = ::sach[ (int)(CPesak::sebranaFigurkaPozice.radek = tah.odkud.radek) ][ (int)(CPesak::sebranaFigurkaPozice.sloupec = tah.kam.sloupec) ].figurka;
    ::sach[ (int)CPesak::sebranaFigurkaPozice.radek ][ (int)CPesak::sebranaFigurkaPozice.sloupec ].figurka = NULL;    
    this->sebranaFigurka->pReference--;
  }
  // standardni brani figury
  else {
    CPesak::sebranaFigurkaPozice = tah.kam;
    this->sebranaFigurka->pReference--;
  }
  ::sach[ (int)CPesak::pozice.radek ][ (int)CPesak::pozice.sloupec ].figurka = this;
  return true;
} 

/**
 * Vrati predchozi tah figurkou.
 * @param tah tam odkud kam se tahlo
 * @param return NEVER
 */
void CPesak::vrat ( const tah_t& tah ) {
  //debugMsg( "CPesak::vrat()" );
  
  ::sach[ (int)(CPesak::pozice.radek = tah.odkud.radek) ][ (int)(CPesak::pozice.sloupec = tah.odkud.sloupec) ].figurka = this;
  ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = NULL;
  
  if ( ( tah.odkud.radek == (char)6 && CPesak::barva == (int)BILA )
    || ( tah.odkud.radek == (char)1 && CPesak::barva == (int)CERNA ) )
    CPesak::kolikratTazeno--;

  // brala se protihracova figura?
  if ( CPesak::sebranaFigurka != NULL ) {
    ::sach[ (int)CPesak::sebranaFigurkaPozice.radek ][ (int)CPesak::sebranaFigurkaPozice.sloupec ].figurka = CPesak::sebranaFigurka;
    this->sebranaFigurka->pReference++;
    this->sebranaFigurka = NULL;
    if ( ! ( tah.kam == CPesak::sebranaFigurkaPozice ) )
      CPesak::enPassant = false;
  }
}


/* ---------------------------------------------------------------------------*\
 *
 *   CKral : CFigurka
 *
\* ---------------------------------------------------------------------------*/
CKral::CKral ( pozice_t pozice, bool barva ) : CFigurka ( pozice, barva ) {
  typFigurky = (char)KRAL;
}

CKral::~CKral ( void ) {
}

/**
 * Najde vsechny tahy, ktere muze dana figurka prave provest
 * @param mozneTahy vytvori a zapise seznam moznych tahu + naraznik tah z policka A8 na A8
 * @param kryti true aktivuje rezim kryti i vlastnich figurek
 * @param return v pripade, ze nenajde ani jeden tah, vrati false
 */
bool CKral::mozneTahy ( tah_t * & mozneTahy, bool kryti ) {
  // tah z A8 na A8 slouzi jakoby naraznik pri prochazenim pole mozneTahy, proto musime vzdy
  // alokovat jeden tah navic nez by bylo v nejkrajnejsim pripade potreba
  mozneTahy = new tah_t [ 8 + 1 ];

  int pTahy = 0; // pocet moznych tahu
  
  // tahy o jedno policko
  for (char s = -1; s <= 1; s++ ) {
    for (char r = -1; r <= 1; r++ ) {
      if ( !( s == 0 && r == 0 ) && sachVolnePolickoNeboProtihrace( this->pozice + pozice_t( s, r ), this->barva, kryti )
        && ( kryti || this->kralPoTahuNebudeNapaden( this->pozice + pozice_t( s, r ) ) ) )
        mozneTahy[pTahy++].kam = this->pozice + pozice_t( s, r );
    }
  }

  // rosady
  if ( this->kolikratTazeno == 0 && this->pozice.sloupec == 4 && this->pozice.radek == (int)(this->barva)*7 ) {
    // bili kral
    if ( this->barva ) {       
      // mala rosada, bili kral
      if ( sachRosadovaVez ( pozice_t ( (char)7, (char)7 ), this->barva ) 
        && sachVolnePolicko( pozice_t ( (char)7, (char)6 ) )
        && sachVolnePolicko( pozice_t ( (char)7, (char)5 ) ) 
        && ( kryti || this->kralPoTahuNebudeNapaden( pozice_t( (char)7, (char)7 ) ) ) ) {
        mozneTahy[pTahy++].kam = pozice_t( (char)7, (char)7 );
      }
      // velka rosada, bili kral
      if ( sachRosadovaVez ( pozice_t ( (char)7, (char)0 ), this->barva ) 
        && sachVolnePolicko( pozice_t ( (char)7, (char)1 ) )
        && sachVolnePolicko( pozice_t ( (char)7, (char)2 ) )
        && sachVolnePolicko( pozice_t ( (char)7, (char)3 ) )
        && ( kryti || this->kralPoTahuNebudeNapaden( pozice_t( (char)7, (char)0 ) ) ) ) {
        mozneTahy[pTahy++].kam = pozice_t( (char)7, (char)0 );
      }
    }
    // cerny kral
    else {
      // mala rosada, cerny kral
      if ( sachRosadovaVez ( pozice_t ( (char)0, (char)7 ), this->barva ) 
        && sachVolnePolicko( pozice_t ( (char)0, (char)6 ) )
        && sachVolnePolicko( pozice_t ( (char)0, (char)5 ) )
        && ( kryti || this->kralPoTahuNebudeNapaden( pozice_t( (char)0, (char)5 ) ) )) {
        mozneTahy[pTahy++].kam = pozice_t( (char)0, (char)7 );
      }
      // velka rosada, cerny kral
      if ( sachRosadovaVez ( pozice_t ( (char)0, (char)0 ), this->barva ) 
        && sachVolnePolicko( pozice_t ( (char)0, (char)1 ) )
        && sachVolnePolicko( pozice_t ( (char)0, (char)2 ) )
        && sachVolnePolicko( pozice_t ( (char)0, (char)3 ) )
        && ( kryti || this->kralPoTahuNebudeNapaden( pozice_t( (char)0, (char)0 ) ) ) ) {
        mozneTahy[pTahy++].kam = pozice_t( (char)0, (char)0 );
      }
    }  
  }

  // pokud nelze figurkou nikam tahnout, deletneme pole a vratime false
  if ( pTahy == 0 ) {
     delete [] mozneTahy;
     return false;
  }

  // doplneni tabulky mozneTahy o vychozi pozici tahu odkud se tahne
  while ( pTahy )
    mozneTahy[ --pTahy ].odkud = this->pozice;

  return true;
}

/**
 * Tahne figurkou.
 * @param tah tah odkud kam se ma figurkou tahnout
 * @param testPlatnosti pokud vime ze tah je urcite platny nastavime false, preskoci se test platnosti tahu
 * @param return v pripade uspesneho tahu navraci true, jinak false
 */
bool CKral::tahni ( const tah_t & tah, bool testPlatnosti = true ) {
  //debugMsg( "CKral::tahni()" );
  
  if (  testPlatnosti ) {
    // overime zda tah je platny (mozny)
    if ( ! this->overeniPlatnostiTahu ( tah ) )
      return false;
    // zde uz vime, ze tah je skutecne platny
  }

  // s figurkou se bude tahnout, minuly tah nepujde uz vratit. 
  // uvolnime vsechny sebrane jeste neuvolnene figurky
  this->vyreseniReferenciSebranaFigurka();

  // rosady
  if ( ( tah.odkud.sloupec - tah.kam.sloupec ) >= 2 || ( tah.odkud.sloupec - tah.kam.sloupec ) <= -2 ) {
    if ( ( ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka = ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka ) != NULL ) {
      // vez je jakoby nase sebrana figurka
      this->sebranaFigurka = ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka;
      // nastavime vezi novou polohu
      ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka->setPozice ( tah.odkud );
      // umistime krale na misto misto veze
      ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this;
    }
    else {
      endwin();
      std::cout << "CKral::tahni" << tah << std::endl;
      for(;;);
    }
  }
  // normalni tahnuti figurkou
  else {
    // vynulujeme na sachovnici policko po krali
    ::sach[ (int)tah.odkud.radek ][ (int)tah.odkud.sloupec ].figurka = NULL;
    
    if ( ( this->sebranaFigurka = ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka ) != NULL ) {
      this->sebranaFigurka->pReference--;
    }
    ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this;
  }

  this->pozice = tah.kam;
  this->kolikratTazeno++;

  return true;
}

/**
 * Vrati predchozi tah figurkou.
 * @param tah tam odkud kam se tahlo
 * @param return NEVER
 */
void CKral::vrat ( const tah_t & tah ) {
  //debugMsg( "CKral::vrat()" );
  
  ::sach[ (int)(this->pozice.radek = tah.odkud.radek) ][ (int)(this->pozice.sloupec = tah.odkud.sloupec) ].figurka = this;
  
  // vraci se zpatky rosada -> vezi je potreba nastavit spravnou pozici
  if ( ( tah.odkud.sloupec - tah.kam.sloupec ) >= 2 ||  ( tah.odkud.sloupec - tah.kam.sloupec ) <= -2 ) {
    // nastaveni sprave pozice veze jako pred rosadou
    if ( ( ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this->sebranaFigurka ) != NULL  ) {
      this->sebranaFigurka->setPozice( tah.kam ); 
      // pocitane reference se neodecitaly, protoze i kdyby se tah nevratil, vez musi zustat stale na sachovnici
      this->sebranaFigurka = NULL;
    }
    else {
      endwin();
      std::cout << "CKral::vrat " << tah << std::endl;
      for(;;);
    }
  }
  // vraci se norm tah
  else {
    if ( ( ::sach[ (int)tah.kam.radek ][ (int)tah.kam.sloupec ].figurka = this->sebranaFigurka ) != NULL ) {
      this->sebranaFigurka->pReference++;
      this->sebranaFigurka = NULL;
    }
  }
  
  //this->sebranaFigurka = NULL;
  this->kolikratTazeno--;
}


