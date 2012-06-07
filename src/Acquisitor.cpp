#include "Acquisitor.h"
#include <iostream>

DWORD WINAPI Thread1(LPVOID lpParameter)
{
   std::cout << "Thread 1 begin" << std::endl;
   ThreadParameter* p = (ThreadParameter*)lpParameter;
   AcquisitionCallback* cb = p->cb;
   SerialPort* port = p->port;

   if(!port->openPort(1))
   {
      std::cerr << "T1: Unable to open COM port !" << std::endl;
   }

   if(!port->setPort(CBR_19200,8))
   {
      std::cerr << "Port configuration failed !" << std::endl;
   }

   // --------------- TEST ---------------
   char data[9] = "BEGINACQ";
   int byteswritten = port->writeOnPort(data,9);
   if(byteswritten < 9)
   {
      std::cerr << "Error while writing data ! (" << byteswritten << ")" << std::endl;
   }

   unsigned char buf[ACQUISITION_SAMPLE_SIZE];
   int bytesrecv = port->readOnPort(buf, sizeof(buf));
   if(!bytesrecv)
   {
      std::cerr << "Error on read !" << std::endl;
   }
   else
   {
      buf[bytesrecv]='\0';
      std::cout << "Received: " << buf << std::endl;
   }

   for(int i=0; i<bytesrecv; i++)
   {
      cb->setRawData(i,bytesrecv,buf[i]);
   }

   cb->result(1.5);
   // --------------- END TEST ---------------

   port->closePort();

   std::cout << "Thread 1 end" << std::endl;
   return 0;
}

DWORD WINAPI Thread2(LPVOID lpParameter)
{
   std::cout << "Thread 2 begin" << std::endl;
   ThreadParameter* p = (ThreadParameter*)lpParameter;
   AcquisitionCallback* cb = p->cb;
   SerialPort* port = p->port;

   if(!port->openPort()) {
      std::cerr << "T2: Unable to open COM port !" << std::endl;
   }

   Sleep(2000);

   port->closePort();

   cb->result(2.5);

   std::cout << "Thread 2 end" << std::endl;

   return 0;
}

DWORD WINAPI CalibrationThread(LPVOID lpParameter)
{
   std::cout << "CalibrationThread begin" << std::endl;

   Sleep(1000);

   std::cout << "CalibrationThread end" << std::endl;

   return 0;
}

Acquisitor::Acquisitor() : _port()
{}

Acquisitor::~Acquisitor()
{
   WaitForSingleObject(_acqThread,5000);
}

void Acquisitor::launchAcquisition(int type, AcquisitionCallback& cb)
{
   static ThreadParameter p;
   if(_acqThread)
   {
      WaitForSingleObject(_acqThread,5000);
   }

   if(type == ACQ_TYPE_ONE)
   {
      // Make the acquisition one time and return the calculated value
      p.cb = &cb;
      p.port = &_port;
      DWORD threadID;
      _acqThread = CreateThread(NULL, 0, Thread1, &p, 0, &threadID);
   }
   else if(type == ACQ_TYPE_MEAN)
   {
      // Make the acquisition five times and return the average calculated value
      p.cb = &cb;
      p.port = &_port;
      DWORD threadID;
      _acqThread = CreateThread(NULL, 0, Thread2, &p, 0, &threadID);
   }
   else
   {
      cb.error("Unknown acquisition type !");
   }
}

bool Acquisitor::calibrate()
{
   // Make calibration procedure
   DWORD threadID;
   _acqThread = CreateThread(NULL, 0, CalibrationThread, NULL, 0, &threadID);
   return false;
}
