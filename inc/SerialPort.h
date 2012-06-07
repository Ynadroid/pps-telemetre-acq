#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <Windows.h>

class SerialPort
{
public:
   SerialPort();
   ~SerialPort();

   bool openPort(int portnumber = 4);
   void closePort();

   bool setPort(DWORD baudrate, BYTE bytesize);

   int readOnPort(unsigned char* buffer, unsigned int buffersize);
   int writeOnPort(LPCVOID data, DWORD datasize);

private:
   HANDLE _portHandle;
   DWORD _inQueue;
   DWORD _outQueue;
   OVERLAPPED _Os;
   DCB _dcb;
};

#endif