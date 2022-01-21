//---------------------------------------------------------------
// Header-Datei
// Deklaration der Funktion in ButtonDebouncedC.c
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
// v 1.1
//----------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
boolean bd_init(int n);
int     bd_setButton(int x, int gpio, int bounceTime );
int     bd_getButton(int x);
boolean bd_getChange(int x, int changeTo);
#ifdef __cplusplus
}
#endif
