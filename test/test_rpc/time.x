/* time.x: Remote time printing protocol */
program TIMEPROG {
   version PRINTIMEVERS {
     string PRINTIME(string) = 1;
   } = 1;
} = 0x20000001; 
