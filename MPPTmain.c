//  /*
//    * MPPTmain. c
//    *
//    *  Created on : May 3 , 2012
//    *       Author : Stefan Moring
//    */
// 
//  #include <avr/io.h>
//  #include <util/delay.h>
//  #include <avr/interrupt.h>
// 
//  #include "mpptfunctions.h"
//  #include "can.h"
// 
// 
//  // Define the operating modes used in the switch function
//  #define PO                        1
//  #define PO ADAPT                  2
//  #define INCOND                    3
//  #define INCOND ADAPT              4
//  #define DUTY CONTROL              5
//  #define PO STABLE                 6
//  #define INCOND STABLE             7
// 
// // Variables used by the MPPT algorithms
// unsigned int V = 0 , I = 0 , Vold = 0 , Iold = 0;
// signed int dV = 0 , dI = 0;
// unsigned int P = 0 , Pold = 0;
// signed int step = 0 , stepold = 0;
//
// // Buffer used by the adaptive algorithms
// int adapt_buffer [ 2 ] = { 0 , 0 };
//
// // Buffers used by the stable algorithms
// unsigned int Pbuf [ 4 ] , Vbuf [ 4 ] , Ibuf [ 4 ] ;
// signed int dVbuf [ 4 ] , dIbuf [ 4 ] ;
//
// // Variables used in the main code
// char OPERATING_MODE = 0;
// int count = 0;
// int STEPSIZE = 1;
//
// int main ( void ) {
//
//      cli () ;
//      OPERATING_MODE = INCOND ;
//      init_adc () ;
//      init_timer () ;
//      init_pwm () ;
//      sei () ;
//
//       while (1) ;
//
//      // Should never happen
//       return 0;
// }
//
//
// // Interrupt routine for timer0
// ISR ( SIG_OUTPUT_COMPARE0A )
// {
//      // Divide interrupt frequency , count == 0 for 100 Hz, 1 for 50 Hz, 2 for 25 Hz etc .
//       if ( count == 2)
//      {
//		count = 0;
//		switch ( OPERATING_MODE )
//					{
//             case PO :
//							 // Changing the step and pwm duty cycle for sure
//                 Pold = P ;						// Saving the previous power
//                 stepold = step ;			// Saving the previous step
//                 read_V(&V ) ;
//                 read_I(&I ) ;
//                 P = I  * V ;					// Calc power
//                 step = po (P , Pold , stepold ) ;		// Setting step to go forwards or backwards
//                 set_duty ( step * STEPSIZE ) ;	// Function to increase the duty cycle
//                 break ;
//
//			case PO_STABLE :
//					Pold = P ;
//					stepold = step ;
//					// Measure V and I four times
//					for ( int i = 0; i < 4; i++)
//					{
//							 read_V(&V ) ;
//							 read_I(&I ) ;
//							 Pbuf [ i ] = I* V ;
//					}
//					// Calculate average P
//					P = Pbuf [ 3 ] + Pbuf [ 2 ] + Pbuf [ 1 ] + Pbuf [ 0 ] ;
//					P = P / 4;
//					step = po (P , Pold , stepold ) ;
//					set_duty ( step   *STEPSIZE ) ;
//					break ;
//
//			case INCOND :
//					read_dV(&V , &dV ) ;
//					read_dI(&I , &dI ) ;
//					step = incond (V , dV , I , dI ) ;		// Set step direction
//					set_duty ( step   *STEPSIZE ) ;
//					break ;
//
//			case INCOND_STABLE :
//					// Measure V,dV, I and dI four times
//					for ( int i = 0; i < 4; i++)
//					{
//							 read_dV(&Vbuf [ i ] , &dVbuf [ i ] ) ;
//							 read_dI(&Ibuf [ i ] , &dIbuf [ i ] ) ;
//					}
//
//					// Calculate Averages
//					V = Vbuf [ 3 ] + Vbuf [ 2 ] + Vbuf [ 1 ] + Vbuf [ 0 ] ;
//					dV = dVbuf [ 3 ] + dVbuf [ 2 ] + dVbuf [ 1 ] + dVbuf [ 0 ] ;
//					I = Ibuf [ 3 ] + Ibuf [ 2 ] + Ibuf [ 1 ] + Ibuf [ 0 ] ;
//					dI = dIbuf [ 3 ] + dIbuf [ 2 ] + dIbuf [ 1 ] + dIbuf [ 0 ] ;
//					V = V / 4;
//					dV = dV / 4;
//					I = I / 4;
//					dI = dI / 4;
//					step = incond (V , dV , I , dI ) ;
//					set_duty ( step   *STEPSIZE ) ;
//					break ;
//					
//			case PO_ADAPT :
//					adapt_buffer [ 0 ] = adapt_buffer [ 1 ] ;
//					stepold = step ;
//					Pold = P ;
//					Vold = V ;
//					read_V(&V ) ;
//					read_I(&I ) ;
//					P = I  * V ;
//					step = po (P , Pold , stepold ) ;
//					// if signs are equal , accumulate , if not control signal equals the result of incond ( . . )
//	
//					 if ((( adapt_buffer [ 0 ] > 0) && ( step > 0) ) || ((-
//												adapt_buffer [ 0 ] < 0) && ( step < 0) ) )
//									adapt_buffer [ 1 ] = adapt_buffer [ 0 ] + step ;
//					 else
//									adapt_buffer [ 1 ] = step ;
//	
//					 set_duty ( STEPSIZE    *  adapt_buffer [ 0 ] ) ;
//					 break ;
//
//			 case INCOND_ADAPT :
//
//					 adapt_buffer [ 0 ] = adapt_buffer [ 1 ] ;
//
//					 read_dV(&V , &dV ) ;
//					 read_dI(&I , &dI ) ;
//					 step = incond (V , dV , I , dI ) ;
//					 // i f signs are equal , accumulate ,          i f not control -
//												signal equals the result of incond ( . . )
//					 i f ((( adapt_buffer [ 0 ] > 0) && ( step > 0) ) | ((-
//												adapt_buffer [ 0 ] < 0) & ( step < 0) ) )
//									adapt_buffer [ 1 ] = adapt_buffer [ 0 ] + step ;
//					 e l s e
//									adapt_buffer [ 1 ] = step ;
//
//					 set_duty ( STEPSIZE    *  adapt_buffer [ 0 ] ) ;
//					 break ;
//
//			 case DUTY_CONTROL :
//
//					 read_V(&V ) ;
//					 double d = ( double ) V / 1023;
//					 d = d    * 199;
//					 OCR1A = ( int ) d ;
//					 break ;
//
//			 default :
//					 // i f operating mode not set , set to P&O ( Should never happen )
//					 OPERATING_MODE = PO ;
//         }
//  }
//  else
//         count++;
//}
