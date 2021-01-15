#include "SMC100C.h"
#include "serialib.h"
#include <iostream>

//Mainly being used to test functionality at this time



int main()
{
    serialib serial;

    /*Test function  
    int test = serial.openDevice("COM3",115200);
    char testchar[5];
    sprintf(testchar,"%d",test);
    printf(testchar);
    serial.writeString("Hello World"); */
    //serial.closeDevice();

    //Working Demo,
    SMC100C SMC100Ctest;
    SMC100Ctest.Home();
    
    return 0;
}