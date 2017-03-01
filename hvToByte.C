#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

void hvToByte(double volt, int channel)
{


  
  //create array of coefficients that fit the equation coeff[n][0]*byte+coeff[n][1]
  double coeff[4][2] = {
    {-0.0152,70.809},
    {-0.0151,69.262},
    {-0.0151,70.367},
    {-0.0152,68.750},
  };


  if(channel < 5 && channel > 0)
    {
      double byte;
      double realv;
      byte = (volt - coeff[channel-1][1])/(coeff[0][0]);
      realv = coeff[channel-1][0]*round(byte) + coeff[channel-1][1];
      if(byte > 255 || byte < 0)
	{
	  cout << "Error, voltage out of bounds for selected channel" << endl;
	}
      else{
	cout << "The byte for " << volt << " volts is " <<round(byte) << " which yields " << realv << " volts." << endl;}
    }
  else
    {
      cout << "Please input a channel between 1 and 4." << endl;
    }
}
