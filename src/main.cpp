#include <iostream>
#include <time.h>

#include <Windows.h>
#include <NIDAQmx.h>

#define SAMPLE_SIZE 255

int main(int argc,char** argv)
{
	std::cout << "Hello !" << std::endl;

   // Task parameters
   int32       error = 0;
   TaskHandle  taskHandle = 0;
   char        errBuff[2048]={'\0'};

   // Channel parameters
   char        chan[] = "Dev1/ao0";
   float64     min = 0.0;
   float64     max = 5.0;

   // Timing parameters
   uInt64      samplesPerChan = SAMPLE_SIZE;

   // Data write parameters
   float64     data[SAMPLE_SIZE];
   int32       pointsWritten;
   float64     timeout = 10.0;
   float64     frequency = 5;
   float64     amplitude = 5;

   // Creating a task for the USB module
	error = DAQmxCreateTask("MyTask",&taskHandle);
   if( !DAQmxFailed(error) )
   {
      std::cout << "Frequency ?\n";
      std::cin >> frequency;
      std::cout << "timeout ?\n";
      std::cin >> timeout;
      std::cout << "Set: " << min << " " << max << " " << frequency << "\n";

      // Setting output voltage signal
      error = DAQmxCreateAOVoltageChan(taskHandle,chan,"",min,max,DAQmx_Val_Volts,NULL);
   }
   else
      std::cout << "Error CreateTask" << std::endl;

   // Do the task
   if( !DAQmxFailed(error) )
      error = DAQmxStartTask(taskHandle);
   else
      std::cout << "Error CreateVoltage" << std::endl;

   double j=0;
   for(int i=0;i<samplesPerChan;i++,j+=0.001) {
      data[i] = abs(sin(j))*amplitude;
   }

   if( !DAQmxFailed(error) ) {
      time_t t = time(NULL);
      while(time(NULL) < t+timeout) {
         error = DAQmxWriteAnalogF64(taskHandle,samplesPerChan,0,timeout,DAQmx_Val_GroupByChannel,data,&pointsWritten,NULL);
      }
   }
   else
      std::cout << "Error StartTask" << std::endl;

   if( DAQmxFailed(error) )
      DAQmxGetExtendedErrorInfo(errBuff,2048);

   if( taskHandle!=0 )
   {
      DAQmxStopTask(taskHandle);
      DAQmxClearTask(taskHandle);
   }

   if( DAQmxFailed(error) )
   {
	   std::cout << "DAQmxBase Error " << error << ": " << errBuff << "\n";
   }

   system("pause");
   return 0;
}
