#include "SerialPort.h"

#include <iostream>

SerialPort::SerialPort()
{
   _portHandle = NULL;
   _Os.hEvent = 0;
   _inQueue = 1024;
   _outQueue = 1024;
}

SerialPort::~SerialPort()
{
   this->closePort();
}

bool SerialPort::openPort(int portnumber)
{
   char filename[20];
   wsprintf( filename , "\\\\.\\COM%d", portnumber );

   memset(&_Os,0,sizeof(_Os));

   _portHandle = CreateFile(filename,
      GENERIC_READ | GENERIC_WRITE,
      0,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED,
      NULL);

   if(_portHandle == INVALID_HANDLE_VALUE)
   {
      std::cerr << "An error occured when opening COM" << portnumber << "(" << GetLastError() << ")" << std::endl;
      return false;
   }

   if(!SetupComm(_portHandle,_inQueue,_outQueue))
   {
      std::cerr << "An error occured on setup COM" << portnumber << "(" << GetLastError() << ")" << std::endl;
      return false;
   }

   DWORD err = GetLastError();
   if(err)
      std::cout << "Error on port initial configuration: " << err << std::endl;

   return true;
}

void SerialPort::closePort()
{
   CloseHandle(_portHandle);
}

bool SerialPort::setPort(DWORD baudrate, BYTE bytesize)
{
   if (!GetCommState(_portHandle,&_dcb)) 
   {
      std::cerr << "Unable to get COM port state (" << GetLastError() << ")" << std::endl;
      return false;
   }
   
   _dcb.BaudRate = baudrate;
   _dcb.ByteSize = bytesize;
   _dcb.Parity = NOPARITY;
   _dcb.StopBits = ONESTOPBIT;
   _dcb.fOutxCtsFlow = 1;
   _dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;

   if (!SetCommState(_portHandle,&_dcb))
   {
      std::cerr << "Unable to set COM port state (" << GetLastError() << ")" << std::endl;
      return false;
   }

   COMMTIMEOUTS timeouts={0};
   timeouts.ReadIntervalTimeout=500;
   timeouts.ReadTotalTimeoutConstant=100;
   timeouts.ReadTotalTimeoutMultiplier=20;

   if(!SetCommTimeouts(_portHandle, &timeouts))
   {
      std::cerr << "Unable to set COM port timeouts (" << GetLastError() << ")" << std::endl;
      return false;
   }

   std::cout << "Port configuration is OK" << std::endl;
   return true;
}

int SerialPort::readOnPort(unsigned char* buffer, unsigned int buffersize)
{
   BOOL readResult = FALSE;
   DWORD bytesread = 0;
   DWORD commEvent = 0;
   DWORD err = GetLastError();

   if(!SetCommMask(_portHandle, EV_RXCHAR | EV_ERR))
   {
      std::cerr << "Unable to setup COM events ! (" << GetLastError() << ")" << std::endl;
      return FALSE;
   }

   if(!WaitCommEvent(_portHandle, &commEvent, NULL))
   {
      std::cerr << "Listening for read event failed (" << GetLastError() << ")" << std::endl;
      return FALSE;
   }
   
   if (commEvent & EV_RXCHAR)
   {
      readResult = ReadFile(_portHandle, buffer, buffersize, &bytesread, &_Os);
      err = GetLastError();
      if(err == ERROR_IO_PENDING)
      {
         if(GetOverlappedResult( _portHandle, &_Os, &bytesread, TRUE ))
         {
            std::cout << "Read " << bytesread << " bytes (" << GetLastError() << ")" << std::endl;
            return bytesread;
         }
         std::cerr << "Unable to complete read on the COM port ! (" << GetLastError() << ")" << std::endl;
      }

      std::cerr << "A reading error occured: " << GetLastError() << std::endl;
   }
   else if(commEvent & EV_ERR)
   {
      std::cerr << "A communication error occured ! (" << GetLastError() << ")" << std::endl;
   }

   return FALSE;
}

int SerialPort::writeOnPort(LPCVOID data, DWORD datasize)
{
   BOOL writeResult = FALSE;
   DWORD byteswritten = 0;
   DWORD err = GetLastError();
   COMSTAT comstat;
   memset(&comstat,0,sizeof(comstat));

   writeResult = WriteFile(_portHandle, data, datasize+1, &byteswritten, &_Os);
   err = GetLastError();
   if(writeResult == FALSE && (err == ERROR_NO_MORE_FILES || err == ERROR_IO_PENDING))
   {
      if(err == ERROR_IO_PENDING)
      {
         std::cout << "Waiting for write completion..." << std::endl;
         if(GetOverlappedResult(_portHandle,&_Os,&byteswritten,TRUE))
         {
            ClearCommError( _portHandle, &err, &comstat );
            std::cout << "Wrote " << byteswritten << " bytes (" << GetLastError() << ")" << std::endl;
            return byteswritten;
         }
         else
         {
            std::cerr << "An error occured when writing data ! (" << GetLastError() << ")" << std::endl;
         }
      }

      ClearCommError( _portHandle, &err, &comstat );
   }
   else if(!writeResult)
   {
      std::cerr << "An error occured when writing on the COM port (" << err << ")" << std::endl;
   }

   return FALSE;
}