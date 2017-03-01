#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>



using namespace std;

int byteToHv(double byte, int channel)
{
  // check to see if byte is a natural number between 1 and 255
  if(byte != floor(byte) || byte < 1 || byte > 255)
    {
      cout << "Error, value must be an integer between 1 and 255." << endl;
      return 1;
    }
  
  //create array of coefficients that fit the equation coeff[n][0]*byte+coeff[n][1]
  double coeff[4][2] = {
    {-0.0152,70.809},
    {-0.0151,69.262},
    {-0.0151,70.367},
    {-0.0152,68.750},
  };


  if(channel < 5 && channel > 0)
    {
      double volt;
      volt = coeff[channel-1][0]*byte + coeff[channel-1][1];
      cout << "The voltage attained by setting the byte to " << byte << " is " << volt << " volts +/- 0.0058%" << endl;
    }
  else
    {
      cout << "Please input a channel value between 1 and 4." << endl;
    }
  return 0;
  
}
  
