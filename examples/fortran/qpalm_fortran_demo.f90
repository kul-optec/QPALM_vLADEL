! THIS VERSION: 25/04/2022 AT 13:45 GMT
! Nick Gould (nick.gould@stfc.ac.uk)

PROGRAM QPALM_fortran_example

   USE QPALM_fiface

   !  set problem dimensions

   INTEGER ( KIND = integer_kind ), PARAMETER :: n = 2
   INTEGER ( KIND = integer_kind ), PARAMETER :: m = 3
   INTEGER ( KIND = integer_kind ), PARAMETER :: hne = 2
   INTEGER ( KIND = integer_kind ), PARAMETER :: ane = 4

   !  set problem and solution arrays

   INTEGER ( KIND = integer_kind ), DIMENSION( hne ) :: hrow
   INTEGER ( KIND = integer_kind ), DIMENSION( n + 1 ) :: hptr
   REAL ( KIND = real_kind ), DIMENSION( hne ) :: hval
   REAL ( KIND = real_kind ), DIMENSION( n ) :: g
   REAL ( KIND = real_kind ) :: f
   INTEGER ( KIND = integer_kind ), DIMENSION( ane ) :: arow
   INTEGER ( KIND = integer_kind ), DIMENSION( n + 1 ) :: aptr
   REAL ( KIND = real_kind ), DIMENSION( ane ) :: aval
   REAL ( KIND = real_kind ), DIMENSION( m ) :: cl, cu
   REAL ( KIND = real_kind ), DIMENSION( n ) :: x
   REAL ( KIND = real_kind ), DIMENSION( m ) :: y
   TYPE ( QPALM_settings ) :: settings
   TYPE ( QPALM_info ) :: info

   !  set problem data (NB 1-based integer index arrays)

   hptr = (/ 1, 2, 3 /) ; hrow = (/ 1, 2 /)
   hval = (/ 1.0_real_kind, 1.5_real_kind /)
   g = (/ 8.4018771715_real_kind, 3.9438292682_real_kind /) ; f = 0.0_real_kind
   aptr = (/ 1, 3, 5 /) ; arow = (/ 1, 3, 2, 3 /)
   aval = (/ 1.0_real_kind, 1.0_real_kind, 1.0_real_kind, 1.0_real_kind /)
   cl = (/ - 2.0_real_kind, - 2.0_real_kind, - 2.0_real_kind /)
   cu = (/ 2.0_real_kind, 2.0_real_kind, 2.0_real_kind /)

   !  assign non-default settings prior to solution

   settings%verbose = 0

   !  solve the problem

   CALL qpalm_fortran( n, m, hne, hptr, hrow, hval, g, f,                       &
      ane, aptr, arow, aval, cl, cu, settings, x, y, info )

   !  report the solution

   IF ( info%status_val == 1 ) THEN
      WRITE( 6, "( ' problem solved in ', I0, ' iterations, objective value =',  &
      &  ES18.10 )" ) info%iter, info%objective
      WRITE( 6, "( ' X =', ( 5ES13.5 ) )" ) x
   ELSE
      WRITE( 6, "( ' error return with status = ', I0 )" ) info%status_val
   END IF

END PROGRAM QPALM_fortran_example
