#include <iostream>
#include <NIDAQmx.h>

int main(int argc,char** argv)
{
	std::cout << "Hello !" << std::endl;

   int16 iStatus = 0;
   int16 iRetVal = 0;
   int16 iDevice = 1;
   int16 iChan = 1;
   int16 iGain = 1;
   float64 dVoltage = 0.0;
   int16 iIgnoreWarning = 0;

   iStatus = AI_VRead(iDevice, iChan, iGain, &dVoltage);
   iRetVal = DAQmxErrorHandler(iStatus, "AI_VRead", iIgnoreWarning);

   if (iStatus == 0) {
      std::cout << "The voltage at AI channel " << iChan << " is " << dVoltage << " volts.\n";
   }

	return 0;
}
