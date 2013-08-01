/*  figurky.h */

#ifndef FIGURKY_H
#define FIGURKY_H

#include "dat_struct.h"
#include <iostream>
#include <ncurses.h>
#include <typeinfo>
#include <exception>
#include <string.h>

// Nastaveni vnitrni reprezentace oznaceni figurek. Implicitne se budou z integeru konvertovat na char
enum FIGURKY {
  PESAK = 1,
  STRELEC = 2,
  VEZ = 3,
  KUN = 4,
  DAMA = 5,
  KRAL = 6
};

enum BARVA {
  BILA = true,
  CERNA = false
};

// Kontejnerova trida pro jednotlive typy figurek - pesak, jezdec, vez, ...
class CFigurka {
  public:
                   CFigurka                        ( void );
                   CFigurka                        ( pozice_t, bool );
    virtual       ~CFigurka                        ( void );
    virtual bool   tahni                           ( const tah_t & , bool );
    virtual void   vrat                            ( const tah_t & );
    virtual bool   mozneTahy                       ( tah_t * &, bool ); // bohuzel se tam pouzivaji tahni a vrat a ty nejsou const
    int            getKolikratTazeno               ( void ) const { return ( kolikratTazeno ); }
    char           getTypFigurky                   ( void ) const { return ( typFigurky ); }
    bool           getBarva                        ( void ) const { return ( barva ); }
    pozice_t       getPozice                       ( void ) const { return ( pozice ); }
    void           setPozice                       ( const pozice_t& p ) { pozice = p; }
    virtual bool   getEnPassant                    ( void ) const;
    friend         std::ostream & operator<<       ( std::ostream&, const CFigurka& );
    void           vykresli                        ( const int ) const; 
    CFigurka      *sebranaFigurka;
    int            pReference;                     // pocitana reference na objekt 
    void           vyreseniReferenciSebranaFigurka ( void );
    bool           dostavamSach                    ( void ) const;
  protected:
    char           typFigurky;                     // viz. enum FIGURKY
    bool           barva;                          // false -> cerna   ;   true -> bila
    pozice_t       pozice;
    int            kolikratTazeno; 
    bool           kralPoTahuNebudeNapaden         ( const pozice_t& );
    bool           overeniPlatnostiTahu            ( const tah_t& );
    bool           pujdeTahnoutNaPolicko           ( const pozice_t &, tah_t * &, int &, bool );
};

// Sachova figurka pesak
class CPesak : public CFigurka {
  public:
                   CPesak                          ( pozice_t , bool );
                  ~CPesak                          ( void );
    bool           tahni                           ( const tah_t & , bool);
    void           vrat                            ( const tah_t & );
    bool           mozneTahy                       ( tah_t * &, bool );
    bool           getEnPassant                    ( void ) const { return ( enPassant ); }
    friend         std::ostream & operator<<       ( std::ostream&, const CPesak& );
  protected:
  //void           zmenFigurku                     ( char novyTypFigurky );
    bool           enPassant;                      // pokud true pesce je mozne brat mimochodem - en passant 
    pozice_t       sebranaFigurkaPozice;           // kvuli vraceni tahu brani mimochodem - e.p.
};

// Sachova figurka KRAL
class CKral : public CFigurka {
  public:
                   CKral               ( pozice_t , bool );
                  ~CKral               ( void );
    bool           mozneTahy           ( tah_t * &, bool );
    bool           tahni               ( const tah_t & , bool );
    void           vrat                ( const tah_t & );
};


// Tridy pro sablonu
class CDama {
};

class CStrelec {
};

class CVez {
};

class CKun {
};

/* ---------------------------------------------------------------------------*\
 *
 *   TFigurka : CFigurka
 *   Sablona pro figurky DAMA, STRELEC, VEZ, KUN
 *
\* ---------------------------------------------------------------------------*/
template <class T>
class TFigurka : public CFigurka {
  public:
                   TFigurka                        ( pozice_t , bool );
                  ~TFigurka                        ( void );
    bool           mozneTahy                       ( tah_t * &, bool );
};

template <class T>
TFigurka<T>::TFigurka( pozice_t pozice, bool barva ) : CFigurka ( pozice, barva ) {
  T *p = NULL;
	try
	{
		const char * str = typeid(*p).name();
    if ( strcmp( str, "5CDama" ) == 0 ) {
      this->typFigurky = DAMA; 
      //std::cout << "DAMA!  " << std::endl;
    }
    else if ( strcmp( str, "8CStrelec" ) == 0 ) {
      this->typFigurky = STRELEC; 
      //std::cout << "Strelec!  " << std::endl;
    }
    else if ( strcmp( str, "4CVez" ) == 0 ) {
      this->typFigurky = VEZ; 
      //std::cout << "Vez!  " << std::endl;
    }
    else if ( strcmp( str, "4CKun" ) == 0 ) {
      this->typFigurky = KUN;
      //std::cout << "Kun!  " << std::endl;
    }
    else {
      std::cout << "TFigurka<T>::TFigurka() with T = " << str << std::endl;
      getch();
    }
	}
	catch (std::bad_typeid &e)
	{
		/* Nějaké ošetření výjimky. Identifikace typu se nepovedla. */
		std::cout << "TFigurka<T>::TFigurka() Odchycena vyjimka bad_typeid" << std::cout;
    getch();
	}
}
                  
template <class T>
TFigurka<T>::~TFigurka( void ) {
}

template <class T>
bool TFigurka<T>::mozneTahy ( tah_t * & mozneTahy, bool kryti ) {
  int pTahy = 0; // pocet moznych tahu
  
  // tah z A8 na A8 slouzi jakoby naraznik pri prochazenim pole mozneTahy, proto musime vzdy
  // alokovat jeden tah navic nez by bylo v nejkrajnejsim pripade potreba
  mozneTahy = NULL;  

  switch ( typFigurky ) {
  case DAMA:
    mozneTahy = new tah_t [ 27 + 1 ];
  case VEZ:
    if ( ! mozneTahy )
      mozneTahy = new tah_t [ 14 + 1 ];

    // Pohyb jako vez
    // Otestuje policka napravo od figurky na radku sachovnice, kde figurka stoji
    for (char i = (this->pozice.sloupec + 1); i < 8; i++)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t( this->pozice.radek , i ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka nalevo od figurky na radku sachovnice, kde figurka stoji
    for (char i = (this->pozice.sloupec - 1); i >= 0; i--)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t( this->pozice.radek , i ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka dolu od figurky po sloupci sachovnice, kde figurka stoji
    for (char i = (this->pozice.radek + 1); i < 8; i++)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t( i, this->pozice.sloupec ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka nahoru od figurky po sloupci sachovnice, kde figurka stoji
    for (char i = (this->pozice.radek -1); i >= 0; i--)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t ( i, this->pozice.sloupec ), mozneTahy, pTahy, kryti ) )
        break;

    if ( typFigurky != DAMA )
      break;

  case STRELEC:
    if ( ! mozneTahy )
      mozneTahy = new tah_t [ 13 + 1 ];
  
    // pohyb jako strelec
    // Otestuje policka napravo nahoru od figurky
    for ( char r = (this->pozice.radek - 1), s = (this->pozice.sloupec + 1); s < 8; r--, s++)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t ( r, s ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka napravo dolu od figurky
    for ( char r = (this->pozice.radek + 1), s = (this->pozice.sloupec + 1); s < 8; r++, s++)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t ( r, s ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka nalevo nahoru od figurky
    for ( char r = (this->pozice.radek - 1), s = (this->pozice.sloupec - 1); s >= 0; r--, s--)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t ( r, s ), mozneTahy, pTahy, kryti ) )
        break;

    // Otestuje policka nalevo dolu od figurky
    for ( char r = (this->pozice.radek + 1), s = (this->pozice.sloupec - 1); s >= 0; r++, s--)
      if ( ! this->pujdeTahnoutNaPolicko ( pozice_t ( r, s ), mozneTahy, pTahy, kryti ) )
        break;

    break;

  case KUN:
    mozneTahy = new tah_t [ 8 + 1 ];

    for (char s = -1; s <= 1; s+=2 ) {
      for (char r = -2; r <= 2; r+=4 ) {
        if ( sachVolnePolickoNeboProtihrace( this->pozice + pozice_t( s, r ), this->barva, kryti ) 
          && ( kryti || this->kralPoTahuNebudeNapaden( this->pozice + pozice_t( s, r ) ) ) )
          mozneTahy[pTahy++].kam = this->pozice + pozice_t( s, r );
        if ( sachVolnePolickoNeboProtihrace( this->pozice + pozice_t( r, s ), this->barva, kryti ) 
          && ( kryti || this->kralPoTahuNebudeNapaden( this->pozice + pozice_t( r, s ) ) ) )
          mozneTahy[pTahy++].kam = this->pozice + pozice_t( r, s );
      }
    }

    break;
  default:
    debugMsg( "TFigurka<T>:mozneTahy() { switch->default: ... }" );    
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

// jo, tohle asi neni moc pekne
#include "sach_global.h"
#include "chess.h"

#endif // FIGURKY_H

