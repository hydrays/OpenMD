#ifdef __C
#ifndef __FELECTROSTATICSCREENINGMETHOD
#define __FELECTROSTATICSCREENINGMETHOD

#define UNDAMPED           1
#define DAMPED             2

#endif
#endif /*__C*/

#ifdef  __FORTRAN90

  INTEGER, PARAMETER:: UNDAMPED     = 1
  INTEGER, PARAMETER:: DAMPED       = 2

#endif