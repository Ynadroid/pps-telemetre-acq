#ifndef __ACQUISITOR_H__
#define __ACQUISITOR_H__

#include <string>
#include "SerialPort.h"

#define ACQUISITION_SAMPLE_SIZE 1024

#define ACQ_TYPE_ONE 0
#define ACQ_TYPE_MEAN 1

class AcquisitionCallback
{
public:
   AcquisitionCallback()
   { acqsize = 0; }

   virtual void result(double distance) = 0;
   virtual void error(std::string reason) = 0;

   double* getRawData(unsigned int& size) { size = acqsize; return results; }
   void setRawData(int i, unsigned int maxsize, double value)
   {
      if(i >= 0 && i < ACQUISITION_SAMPLE_SIZE)
      {
         results[i] = value;
      }

      if(maxsize <= ACQUISITION_SAMPLE_SIZE && acqsize < maxsize)
      {
         acqsize = maxsize;
      }
   }

private:
   double results[ACQUISITION_SAMPLE_SIZE];
   unsigned int acqsize;
};

class Acquisitor
{
public:
   Acquisitor();
   ~Acquisitor();

   void launchAcquisition(int type, AcquisitionCallback& cb);
   bool calibrate();

private:
   SerialPort _port;
   HANDLE _acqThread;
};

struct ThreadParameter
{
   AcquisitionCallback* cb;
   SerialPort* port;
};

#endif