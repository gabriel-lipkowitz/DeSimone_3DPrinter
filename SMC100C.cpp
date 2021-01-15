
#include "SMC100C.h"
#include "serialib.h"
#include <stdio.h>

/**************************************************************************************************************************************
Module: 
    SMC100C

Revision: 
    0.0.2

Description: 
    Interfaces with an SMC100CC motion controller to control 

Notes:
    Very raw, has not been tested properly yet

Reference Materials:
    SMC100CC manual:
    https://www.newport.com/mam/celum/celum_assets/resources/SMC100CC___SMC100PP_-_User_s_Manual.pdf?2
    SeriaLib:
    https://lucidar.me/en/serialib/cross-plateform-rs232-serial-library/

History:
 When           Who     What/Why
 -------------- ---     --------
 1/13/21       TimS   Added Command, Controller, Error libraries
 1/14/21       TimS   Added initial Serial Interfacing and Set/Send Command Functions
 1/15/21       TimS   Changed serial communications library to SeriaLib
                      Can now effectively communicate with serial port

***************************************************************************************************************************************/


//Many of these functions will likely not be needed and can be removed at a later date after testing
//Command Libarary (Based on SMC100C User Manual p. 22-70)
    //CommandType describes the command and pairs it with the appropriate ASCII command
    //Many of these may not be needed if we use the GUI for config
    //CommandParameterType: (May need to revisit)
        //None =  No input parameter is needed
        //Int = Used for setting a state e.g 0 = Off, 1 = On
        //Float =  Usually pertains to a real value e.g Setting Velocity
    //CommandGetSetType:
        //None = Only returns value
        //GetSet = Can be used to get(return) or set a value (May want to change some of these to GetAlways)
        //GetAlways = ALways gets(returns) the value

const SMC100C::CommandStruct SMC100C::CommandLibrary[] =
{
    {CommandType::None,"  ",CommandParameterType::None,CommandGetSetType::None},
	{CommandType::Acceleration,"AC",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::BacklashComp,"BA",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::HysterisisComp,"BH",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::DriverVoltage,"DV",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::KdLowPassFilterCutOff,"FD",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::FollowingErrorLim,"FE",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::FrictionComp,"FF",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::HomeSearchType,"HT",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::StageIdentifier,"ID",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::LeaveJoggingState,"JD",CommandParameterType::None,CommandGetSetType::None},
    {CommandType::KeypadEnable,"JM",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::JerkTime,"JM",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::DerivativeGain,"KD",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::IntegralGain,"KI",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::ProportionalGain,"KP",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::VelocityFeedForward,"KV",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::Enable,"MM",CommandParameterType::Int,CommandGetSetType::None},
    {CommandType::HomeSearchVelocity,"OH",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::HomeSearch,"OR",CommandParameterType::None,CommandGetSetType::None},
    {CommandType::HomeSearchTimeout,"OT",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::MoveAbs,"PA",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::MoveRel,"PR",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::MoveEstimate,"PT",CommandParameterType::Float,CommandGetSetType::GetAlways},
    {CommandType::Configure,"PW",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::Analog,"RA",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::TTLInputVal,"RB",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::Reset,"RS",CommandParameterType::None,CommandGetSetType::None},
    {CommandType::RS485Adress,"SA",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::TTLOutputVal,"SB",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::ControlLoopState,"SC",CommandParameterType::Int,CommandGetSetType::GetSet},
    {CommandType::NegativeSoftwareLim,"SL",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::PositiveSoftwareLim,"SR",CommandParameterType::Float,CommandGetSetType::GetSet},
	{CommandType::StopMotion,"ST",CommandParameterType::None,CommandGetSetType::None},
    {CommandType::EncoderIncrementVal,"SU",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::CommandErrorString,"TB",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::LastCommandErr,"TE",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::PositionAsSet,"TH",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::PositionReal,"TP",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::ErrorStatus,"TS",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::Velocity,"VA",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::BaseVelocity,"Vb",CommandParameterType::Float,CommandGetSetType::GetSet},
    {CommandType::ControllerRevisionInfo,"VE",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::AllConfigParam,"ZT",CommandParameterType::None,CommandGetSetType::GetAlways},
    {CommandType::ESPStageConfig,"ZX",CommandParameterType::None,CommandGetSetType::GetSet},
};

//Current Controller States (Based on SMC100C User Manual p.65)
//Used to interpret output from ErrorStatus command
const SMC100C::StatusCharSet SMC100C::StatusLibrary[] = 
{
    //Not referenced from Reset
	{"0A",StatusType::NoReference},
    //Not referenced from Homing
	{"0B",StatusType::NoReference},
    //Not refernced from Config
	{"0C",StatusType::NoReference},
    //Not referenced from Disable
	{"0D",StatusType::NoReference},
    //Not referenced from Moving
	{"0E",StatusType::NoReference},
    //Not referenced from Ready
	{"0F",StatusType::NoReference},
    //Not referenced ESP stage error
	{"10",StatusType::NoReference},
    //Not referenced from Jogging 
	{"11",StatusType::NoReference},
    //Configuration
	{"14",StatusType::Config},
    //Homing Commanded from RS-232-C
	{"1E",StatusType::Homing},
    //Homing commanded by SMC-RC
	{"1F",StatusType::Homing},
    //Moving
	{"28",StatusType::Moving},
    //Ready from Homing
	{"32",StatusType::Ready},
    //Ready from Moving
	{"33",StatusType::Ready},
    //Ready from Disable
	{"34",StatusType::Ready},
    //Ready from Jogging
	{"35",StatusType::Ready},
    //Disable from Ready
	{"3C",StatusType::Disabled},
    //Disable from Moving
	{"3D",StatusType::Disabled},
    //Disable from Jogging
	{"3E",StatusType::Disabled},
    //Jogging from Ready
	{"46",StatusType::Jogging},
    //Jogging from Disable
	{"47",StatusType::Jogging},
};



//Converts error message received to a plaintext string (Based on SMC100CC User Manual)
const char* SMC100C::ConvertToErrorString(char ErrorChar)
{
	switch (ErrorChar)
	{
		case 'A':
			return "Unknown message";
		case 'B':
			return "Incorrect address";
		case 'C':
			return "Parameter missing";
		case 'D':
			return "Command not allowed";
		case 'E':
			return "Already homing";
		case 'F':
			return "ESP stage unknown";
		case 'G':
			return "Displacement out of limits";
		case 'H':
			return "Not allowed in NOT REFERENCED";
		case 'I':
			return "Not allowed in CONFIGURATION";
		case 'J':
			return "Not allowed in DISABLED";
		case 'K':
			return "Not allowed in READY";
		case 'L':
			return "Not allowed in HOMING";
		case 'M':
			return "Not allowed in MOVING";
		case 'N':
			return "Out of soft limits";
		case 'S':
			return "Communication time out";
		case 'U':
			return "EEPROM error";
		case 'V':
			return "Error during command execution";
		case 'W':
			return "Command not allowed for PP";
		case 'X':
			return "Command not allowed for CC";
		default:
			return "Unknown error character";
	};
};

//Serial Port initialization (May be removed)
void SMC100C::SMC100CInit()
{
    serialib serial;
    serial.openDevice("COM3",57600);
    printf("Serial Port initiated");
};

//Home, sends Home request to controller
void SMC100C::Home()
{
    printf("Request For Home \r\n");
    SetCommand(CommandType::HomeSearch, 0.0, CommandGetSetType::None);
};

//Setting Command

void SMC100C::SetCommand(CommandType Type, float Parameter, CommandGetSetType GetOrSet)
{
    printf("Setting Command \r\n");
    CommandToPrint.Command = &CommandLibrary[static_cast<int>(Type)];
    CommandToPrint.Parameter = Parameter;
    CommandToPrint.GetOrSet = GetOrSet;
    SendCurrentCommand();
};

//Send Command to SMC
//Commented out parts rely on serial port interfacing information that isn't done yet
bool SMC100C::SendCurrentCommand()
{
    printf("Sending Command \r\n");
    serialib serial;
    //Will move GetCharacter, CarriageReturnChar and NewLineChar out of this function eventually
    static const char* GetCharacter = "?";
    static const char* CarriageReturnChar = "\r";
    static const char* NewLineChar = "\n";
    //Establishing Status Variable
    bool Status = true;
    //Reading the parameter and GetOrSet type from CommandToPrint (This step may not be needed)
    CurrentCommandParameter = CommandToPrint.Parameter;
    CurrentCommandGetOrSet = CommandToPrint.GetOrSet;
    //Open Serial Port
    serial.openDevice("COM3",115200);
    //Write ASCII characters to Serial (Also printed to output)
    printf(&CommandToPrint.Command->CommandChar[0]);
    serial.writeString(&CommandToPrint.Command->CommandChar[0]);
    //If the GetOrSet command type is Get, print the Get character ("?") 
    if (CurrentCommandGetOrSet == CommandGetSetType::Get)
	{
        //Write Get Character to Serial
        serial.writeString(GetCharacter);
	}
    //If the GetOrSet command type is Set 
    else if (CurrentCommandGetOrSet == CommandGetSetType::Set)
    {
        //If the CommandParameterType is Int
        if (CommandToPrint.Command->SendType == CommandParameterType::Int)
        {
            printf("Setting Int");
            //Create target char array for int
            char IntParam[10];
            //Populate target char array with converted int
            sprintf(IntParam, "%d" , CurrentCommandParameter);
            //Write IntParam to Serial
            serial.writeString(IntParam);
        }
        //If the CommandParameterType is Float
        else if (CommandToPrint.Command->SendType == CommandParameterType::Float)
        {
            //Create target char array for float
            char FloatParam[10];
            //Populate target char array with converted float
            sprintf(FloatParam, "%f", CurrentCommandParameter);
            //Write FloatParam to Serial
            serial.writeString(FloatParam);
        }
        else
        {
            //Error check
            Status = false;
        };
    }
    //Else if the GetSetType is None or GetAlways do nothing, else error
    else if ( (CommandToPrint.Command->GetSetType == CommandGetSetType::None) || (CommandToPrint.Command->GetSetType == CommandGetSetType::GetAlways) )
    {

    }
    else
    {
        //Error Check
        Status = false;
    }
    return Status;
};