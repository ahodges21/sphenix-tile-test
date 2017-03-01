#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>



using namespace std;

void bytetohv(double byte, int channel)
{
  // check to see if byte is a natural number between 1 and 255
  if(byte != floor(byte) || byte < 1 || byte > 255)
    {
      cout << "Error, value must be an integer between 1 and 255." << endl;
      exit(EXIT_FAILURE);
    }
  
  //create array of coefficients that fit the equation coeff[n][0]*byte+coeff[n][1]
  double coeff[4][2] = {
    {-0.0152,70.809},
    {-0.0151,69.262},
    {-0.0151,70.367},
    {-0.0152,68.750},
  };

  double volt;
  
  if(channel == 1)
    {

      volt = coeff[0][0]*byte + coeff[0][1];
      cout << "The voltage attained by setting the byte to " << byte << " is " << volt << " volts +/- 0.0058%" << endl;
    }
  else if(channel == 2)
    {
      volt = coeff[1][0]*byte + coeff[1][1];
      cout << "The voltage attained by setting the byte to " << byte << " is " << volt << " volts +/- 0.012%%" << endl;
    }
  else if(channel == 3)
    {
      volt = coeff[2][0]*byte + coeff[2][1];
      cout << "The voltage attained by setting the byte to " << byte << " is " << volt << " volts +/- 0.0023%" << endl;
    }
  else if(channel == 4)
    {
      volt = coeff[3][0]*byte + coeff[3][1];
      cout << "The voltage attained by setting the byte to " << byte << " " << "is " << volt <<" " <<  "volts +/- 0.001%" << endl;
    }
  else
    {
      cout << "Please input a channel value between 1 and 4." << endl;
    }

  
}
  
