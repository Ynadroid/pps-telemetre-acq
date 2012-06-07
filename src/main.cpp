#include <iostream>
#include <ctime>

#include "Acquisitor.h"

static bool canAcq = false;

class TestCallback : public AcquisitionCallback
{
   virtual void result(double distance)
   {
      std::cout << "Result is " << distance << std::endl;
      canAcq = true;
   }

   virtual void error(std::string reason)
   {
      std::cerr << "Error: " << reason << std::endl;
   }
};

int main(int argc,char** argv)
{
	std::cout << "Hello !" << std::endl;

   Acquisitor acq;
   TestCallback cb;

   //acq.calibrate();
   acq.launchAcquisition(ACQ_TYPE_ONE,cb);
   int timer = 15;
   time_t time = std::time(NULL);
   while(!canAcq /*&& std::time(NULL) < time+timer*/) { Sleep(2000); }

   unsigned int ressize = 0;
   double* d = cb.getRawData(ressize);

   for(unsigned int i=0; i<ressize; i++)
   {
      std::cout << d[i] << " ";
   }
   std::cout << std::endl;

   std::cout << "END" << std::endl;
   return 0;
}
