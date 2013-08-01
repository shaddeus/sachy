/* dat_struct.cpp */

#include "dat_struct.h"

// ncurses okno praveho sloupce od sachovnice
WINDOW *pravySloupec;

/* ---------------------------------------------------------------------------*\
 *   pozice_t
\* ---------------------------------------------------------------------------*/
pozice_t & pozice_t::operator=( const pozice_t& p ) {
  if ( this != &p ) {
    radek = p.radek;
    sloupec = p.sloupec;
  }
  return *this;
}

bool operator==( const pozice_t& x, const pozice_t& y) {
  if ( &x == &y )
    return true;
  return ( x.radek == y.radek && x.sloupec == y.sloupec );
}

pozice_t operator+( const pozice_t & x, const pozice_t & y ) {
    return pozice_t( x.radek + y.radek, x.sloupec + y.sloupec );
}

pozice_t operator-( const pozice_t & x, const pozice_t & y ) {
    return pozice_t( x.radek - y.radek, x.sloupec - y.sloupec );
}

std::ostream& operator<< (std::ostream& o, const pozice_t& p) {
  if ( p.sloupec <= 7 && p.radek <= 7 ) {
    pozice_t p2 = p;
    if ( ! pozice2sach( &p2 ) )
      std::cout << "Spatna pozice." << std::endl;
    o << p2.sloupec << p2.radek << "_" << std::flush;
  }
  else
    o << p.sloupec << p.radek << std::flush;
  return o;
}

void pozice_t::vypis ( void ) const {
  if ( this->sloupec <= 7 && this->radek <= 7 ) {
    pozice_t p2 = pozice_t ( this->radek, this->sloupec );
    if ( ! pozice2sach( &p2 ) )
      errorMsg ( "Spatna pozice." );
    wprintw(::pravySloupec, "%c%c", p2.sloupec, p2.radek );
    //printw("%c%c", p2.sloupec, p2.radek );
  }
  else
    wprintw(::pravySloupec, "%c%c_", this->sloupec, this->radek );
    //printw("%c%c_", this->sloupec, this->radek );
}


/* ---------------------------------------------------------------------------*\
 *   tah_t
\* ---------------------------------------------------------------------------*/
tah_t::tah_t ( const pozice_t& from, const pozice_t& where) {
  odkud = from;
  kam = where;
}

tah_t::tah_t ( const tah_t& tah ) {
  odkud = tah.odkud;
  kam = tah.kam;
}

tah_t::tah_t ( void ) {
  odkud = 0;
  kam = 0;
}

/**
 * Otestuje zda tah je 'prazdny'; Tah z A8 na A8.
 * @param tah testovany tah
 * @param return vraci true pokud se jedna o prazdny tah - tah z policka A8 na A8
 */
bool tah_t::prazdnyTah ( void ) const {
  return ( ( (int) odkud.sloupec == 0 && (int) odkud.radek == 0 ) 
          && ( (int) kam.sloupec == 0 && (int) kam.radek == 0 ) );
}

tah_t & tah_t::operator=( const tah_t& p ) {
  if ( this != &p ) {
    odkud = p.odkud;
    kam = p.kam;
  }
  return *this;
}

bool operator==( const tah_t& x, const tah_t& y ) {
  if ( &x == &y )
    return true;
  return ( x.odkud == y.odkud && x.kam == y.kam );
}

std::ostream& operator<< (std::ostream& o, const tah_t& p) {
  o << p.odkud << " -> " << p.kam;
  return o;
}

void tah_t::vypis ( void ) const {
  this->odkud.vypis();
  this->kam.vypis();
}

std::ostream& operator<< (std::ostream& o, const tah_t * & p) {
  o << " !!! " << std::endl;
  return o;
}

void debugMsg ( const char* zprava ) {
  if ( true ) {
    int x, y, row, col;
    getyx(stdscr, y, x);
    getmaxyx(stdscr,row,col);
    mvprintw(row - 2, 0, "Debug: %s                                       ", zprava);
    move(y , x);	
    getch();
  }
}

void errorMsg ( const char* zprava ) {
  if ( true ) {
    int x, y, row, col;
    getyx(stdscr, y, x);
    getmaxyx(stdscr,row,col);
    mvprintw(row - 1, 0, "ERROR: %s                            ", zprava);
    move(y , x);	
    getch();
  }
}

/**
 * Funkce na prevadeni sachoveho zapisu pozice (napr. H8/h8) na vnitrni zapis pozice
 * (jednorozmerne pole, napr. 7) cislovano z leveho horniho rohu po radkach.
 * @param pozice tato promenna bude prevedena
 * @param return v pripade spatneho rozsahu / neexistujici pozice, vrati false.
 */
bool sach2pozice ( pozice_t * pozice ) {

  if ( pozice->sloupec >= 'a' && pozice->sloupec <= 'h' )
    pozice->sloupec -= 'a';
  else if ( pozice->sloupec >= 'A' && pozice->sloupec <= 'H' )
    pozice->sloupec -= 'A';
  else 
    return false; 

  if ( pozice->radek >= '1' && pozice->radek <= '8' )
    pozice->radek -= '1';
  else 
    return false;

  // sachovnice zacina cislovat od dolniho leveho rohu, proto musime prehodit radky;
  pozice->radek = 4 - (pozice->radek - 3);

  return true;
}


/**
 * Funkce na prevadeni sachoveho zapisu pozice na vnitrni zapis pozice
 */
pozice_t sach2pozice ( const char sach[3] ) {
  pozice_t pozice;
  pozice.radek = sach[1];
  pozice.sloupec = sach[0];
  
  if ( ! ( sach2pozice ( & pozice ) ) )
    errorMsg( "Chybna pozice!!" );
  return pozice;
}

/**
 * Funkce na prevadeni pozice vnitrniho zapisu na sachovy zápis (napr. e4)
 * @param pozice prevadena pozice
 * @param return v pripade spatneho rozsahu / neexistujici pozice, vrati false.
 */
bool pozice2sach ( pozice_t * pozice ) {
  if ( pozice->sloupec >= 0 && pozice->sloupec <= 7 && pozice->radek >= 0 && pozice->radek <= 7) {
    pozice->radek = ( 4 - ( pozice->radek - 3 )) + '1';
    pozice->sloupec += 'a';
    return true;
  }
  else
    return false;
}

/**
 * Zjisti zda tah je mezi *tahy
 * @param tah hledany tah
 * @param tahy prohledavany seznam tahu
 * @param return v pripade nalezeni tahu v poli tahy vraci true, jinak false 
 */
bool jeTahMeziTahy ( const tah_t& tah, const tah_t * tahy ) {
  bool povoleni = false;
  while ( ! tahy->prazdnyTah() ) {
    if ( *tahy == tah ) {
      povoleni = true;
      break;
    }
    tahy++;
  }
  return ( povoleni );
}

/**
 * Zobrazi na terminal vsechny tahy v poli
 */
void vypisTahy ( const tah_t * tahy ) {
  // tah z A8 na A8 slouzi jakoby naraznik pri prochazeni pole, proto je vzdy
  // alokovan jeden tah navic nez by bylo v nejkrajnejsim pripade potreba
  /*
  while ( ! tahy->prazdnyTah() )
    std::cout << *(tahy++) << " ";
  std::cout << std::endl;
  */
  while ( ! tahy->prazdnyTah() ) {
    (tahy++)->vypis();
    wprintw(::pravySloupec, " ");
    //printw(" ");
  }
}
