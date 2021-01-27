
/************************************************************************************************************
Module: 
    SMC100CRun.cpp

Revision: 
    0.0.1

Description: 
    Tests functionality of SMC100C.cpp
    To run a test function defines the corresponding variable in Module Defines

Notes:
    A dump of different test functions

Reference Materials:
    SMC100CC manual:
    https://www.newport.com/mam/celum/celum_assets/resources/SMC100CC___SMC100PP_-_User_s_Manual.pdf?2
    SeriaLib:
    https://lucidar.me/en/serialib/cross-plateform-rs232-serial-library/

History:
 When           Who     What/Why
 -------------- ---     --------
 1/17/21       TimS   Added SteppedMove and documentation

************************************************************************************************************/
/*-------------------------------------------- Include Files ----------------------------------------------*/
#include "SMC100C.h"
#include "serialib.h"
#include <iostream>
#include <windows.h>
/*-------------------------------------------- Module Defines ----------------------------------------------*/
//Define a keyword here to run a function
//#define HelloWorld
//#define HomeTest
//#define SteppedMove
/*--------------------------------------------- Module Code ------------------------------------------------*/
int main()
{
    serialib serial;
/*---------------------- "HelloWorld"  ---------------------*/
    #ifdef HelloWorld
    int test = serial.openDevice("COM3",115200);
    char testchar[5];
    sprintf(testchar,"%d",test);
    printf(testchar);
    serial.writeString("Hello World");
    serial.closeDevice();
    #endif
/*----------------------- "HomeTest"  ----------------------*/
    #ifdef HomeTest
    SMC100C SMC100CHome;
    SMC100CHome.Home();
    #endif
/*--------------------- "SteppedMove"  --------------------*/
    #ifdef SteppedMove
    SMC100C SMC100CSteppedMove;
    //Set velocity to 1 unit/s
    SMC100CSteppedMove.SetVelocity(1);
    int i = 0;
    for (i = 1; i <= 10; i++)
    {
        //Move 1 unit
        SMC100CSteppedMove.RelativeMove(1);
        //Wait 2 seconds(2000ms), 1 second to move and 1 second wait
        Sleep(2000);
    };
    #endif
    
    return 0;
}