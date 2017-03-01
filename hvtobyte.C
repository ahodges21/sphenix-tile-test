#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>


using namespace std;

void hvtobyte(double volt, int channel)
{


  
  //create array of coefficients that fit the equation coeff[n][0]*byte+coeff[n][1]
  double coeff[4][2] = {
    {-0.0152,70.809},
    {-0.0151,69.262},
    {-0.0151,70.367},
    {-0.0152,68.750},
  };



   double byte;
   double realv;
   if(channel == 1)
     {
       byte = (volt - coeff[0][1])/(coeff[0][0]);
       realv = coeff[0][0]*round(byte) + coeff[0][1];
       if(byte > 255 || byte < 0)
	 {
	   cout << "Error, voltage out of bounds for selected channel" << endl;
	 }
       else{
	 cout << "The byte for " << volt << " volts is " <<round(byte) << " which yields " << realv << " volts." << endl;}
     }
   else if(channel == 2)
     {
       byte = (volt - coeff[1][1])/(coeff[1][0]);
       realv = coeff[0][1]*round(byte) + coeff[0][0];
       if(byte > 255 || byte < 0)
	 {
	   cout << "Error, voltage out of bounds for selected channel" << endl;
	 }
       cout << "The byte for " << volt << " volts is " <<round(byte) << " which yields " << realv << " volts." << endl;
     }
   else if(channel == 3)
     {
       byte = (volt - coeff[2][1])/(coeff[2][0]);
       realv = coeff[0][1]*round(byte) + coeff[0][0];
       if(byte > 255 || byte < 0)
	 {
	   cout << "Error, voltage out of bounds for selected channel" << endl;
	 }
       cout << "The byte for " << volt << " volts is " <<round(byte) << " which yields " << realv << " volts." << endl;
     }
   else if(channel == 4)
     {
       byte = (volt - coeff[3][1])/(coeff[3][0]);
       realv = coeff[0][0]*round(byte) + coeff[0][1];
       if(byte > 255 || byte < 0)
	 {
	   cout << "Error, voltage out of bounds for selected channel" << endl;
	 }
       cout << "The byte for " << volt << " volts is " <<round(byte) << " which yields " << realv << " volts." << endl;
     }
   else
     {
       cout << "Please input a channel between 1 and 4." << endl;
     }

}
