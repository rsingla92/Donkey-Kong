/* Note that if you are using this file, you should make
   sure your switches and LED addresses match those 
   when you ran QSYS to generate the processor */

#define switches (volatile char *) 0x0002000 

#define leds (char *) 0x0002010



void main()

{
 
   while (1)

       *leds = *switches;

}
