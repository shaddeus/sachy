/* dat_struct.h */

#ifndef DAT_STRUCT_H
#define DAT_STRUCT_H

// napocital jsem, ze na jedno policko sachovnice nebude mit soucasne vliv/dosah 
// vic jak 13 figurek (beru-li vpotaz zakryti tahu jinou figurkou)
#define MAX_POCET_FIGUREK_NA_1_POLICKO 16
// 16 jsem nastavil s rezervou pro pripad, ze jsem se prepocital

#include <ncurses.h>
#include <iostream>

// ncurses okno praveho sloupce od sachovnice
extern WINDOW* pravySloupec;

// Datovy typ sachova pozice
struct pozice_t {
  unsigned char          radek;
  unsigned char          sloupec;

                         pozice_t     ( const char r = 0, const char s = 0 ) { radek = r; sloupec = s; }
  pozice_t             & operator=    ( const pozice_t& ); 
  friend bool            operator==   ( const pozice_t&, const pozice_t& );
  friend pozice_t        operator+    ( const pozice_t&, const pozice_t& );
  friend pozice_t        operator-    ( const pozice_t&, const pozice_t& );
  friend std::ostream  & operator<<   ( std::ostream&, const pozice_t& );
  void                   vypis        ( void ) const;
};

// Datovy typ sachovy tah
struct tah_t {
  pozice_t               odkud;
  pozice_t               kam;

                         tah_t        ( const pozice_t&, const pozice_t& );
                         tah_t        ( const tah_t& );
                         tah_t        ( void );
  bool                   prazdnyTah   ( void ) const;
  tah_t                & operator=    ( const tah_t& );
  friend bool            operator==   ( const tah_t& , const tah_t& );
  friend std::ostream  & operator<<   ( std::ostream&, const tah_t& );
  friend std::ostream  & operator<<   ( std::ostream&, const tah_t* & );
  void                   vypis        ( void ) const;
};

void                     errorMsg     ( const char* );
void                     debugMsg     ( const char* );
bool                     pozice2sach  ( pozice_t* );
bool                     sach2pozice  ( pozice_t* );
pozice_t                 sach2pozice  ( const char* );
bool                     jeTahMeziTahy( const tah_t& , const tah_t* );
void                     vypisTahy    ( const tah_t* );

#endif // DAT_STRUCT_H
