//---------------------------------------------------------------
// Ein C-Modul mit Funktionen um Taster zu entprellen 
// Funktionsweise: 
//   Reagiere auf die erste Flankenänderung
//   Dann reagiere für die Prellzeit aber auf keine weiteren
//   Änderungen des Tasters
// Als ein Satz wiederverndbarer C-Funktionen realsiert.
// Es können eine zuvor mit bd_init() festgelegte Anzahl an 
// Taster "gleichzeitig" abgefragt werden
// GPIO und Prellzeit der Taster werden über die Funktion
// bd_setButton() gesetzen
// bd_getButton() oder bd_getChange() wird der Taster abgefragt.
//---------------------------------------------------------------
// v 1.1b
//---------------------------------------------------------------
#include <Arduino.h>
#include "ButtonDebouncedCalloc.h"

// Tasterinformationen
struct _bd_struct {
    int           gpio; 
    int           stateLast;
    int           stateLastChange[2]; //separat für chabge to LOW und HIGH;
    unsigned long bounceTime;   
    unsigned long timeToggeled;
};
// Zeiger auf den Speicher, der die
// Liste der Tasterinformationen enthält
struct _bd_struct *_bdList;
int    bd_MAX;   // Anzahl Taster für die Speicher allokiert wurde

//---------------------------------------------------------------
// Allokiert speicher für n Taster
// return true, falls erfolgreich, ansonsten false
//---------------------------------------------------------------
boolean bd_init(int n)
{
  if ( n<=0 )
    n=1;
  _bdList = (struct _bd_struct *) malloc(n*sizeof(struct _bd_struct));
  if ( _bdList )
  {
    bd_MAX = n;
    return true;
  }
  bd_MAX = 0;
  return false;
}

//---------------------------------------------------------------
// setze GPIO und bounceTime für einen Taster x im Array
//---------------------------------------------------------------
int bd_setButton(int x, int gpio, int bounceTime )
{
  if ( x<0 || x>=bd_MAX || gpio==0 )
    return -1;
  _bdList[x].gpio = gpio;
  _bdList[x].bounceTime = bounceTime;
  _bdList[x].stateLast = 
  _bdList[x].stateLastChange[0] =
  _bdList[x].stateLastChange[1] = digitalRead(gpio);
  _bdList[x].timeToggeled = 0;
  return 0;
}

//---------------------------------------------------------------
// liefert den Status des Buttons zurück
// LOW oder HIGH
// ein Wechsel zwischen LOW bzw. HIGH wird nur vollzogen, 
// wenn eine Flanke außerhalb der bounceTime auftritt 
//---------------------------------------------------------------
int bd_getButton(int x)
{
  if ( x<0 || x>=bd_MAX )
    return HIGH;
  if ( _bdList[x].gpio==0 )
    return HIGH;

  int state = digitalRead(_bdList[x].gpio);
  if ( state != _bdList[x].stateLast )   // es hat sich was geändert
  {
    if ( millis() - _bdList[x].timeToggeled < _bdList[x].bounceTime )
      return _bdList[x].stateLast;       // wir sind in der Prellzeit, 
                                         // liefere den letzten Status zurück
    _bdList[x].timeToggeled = millis();
    _bdList[x].stateLast = state;
    //Serial.print(gpio);
    //Serial.println(state);
  }
  return state;
}

//---------------------------------------------------------------
// Liefert true zurück, wenn nach der letzten Abfrage ein
// entprellter Wechsel auf changeTo (LOW oder HIGH) erfolgte
// ansonsten false.
//---------------------------------------------------------------
boolean bd_getChange(int x, int changeTo)
{
  if ( x<0 || x>=bd_MAX )
    return false;
  if ( _bdList[x].gpio==0 )
    return false;

  int state = bd_getButton(x);
  int changeFrom = (changeTo==LOW)?HIGH:LOW;
  boolean r = ( state==changeTo && _bdList[x].stateLastChange[(changeTo==LOW)?0:1]==changeFrom );
  _bdList[x].stateLastChange[(changeTo==LOW)?0:1] = state;
  return r;
}
//---------------------------------------------------------------
