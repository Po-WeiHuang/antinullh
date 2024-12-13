#include <iostream>
#include "TMath.h"
void test(double baseline, double nuEnergy, double fDmSqr21, double fSSqrTheta12,double fDmSqr01, double fSSqrTheta01){
    // baseline in km // nuEnergy in MeV
    Double_t nuE_parent = nuEnergy;

    Double_t fDmSqr32 = 2.453e-3;
    Double_t fSSqrTheta13 = 0.0218; Double_t fCSqrTheta13 = 1 - fSSqrTheta13; 
    Double_t fSSqrTheta23 = 0.545 ; Double_t fCSqrTheta23 = 1 - fSSqrTheta23;
    Double_t fCSqrTheta01 = 1 - fSSqrTheta01; 
    Double_t fCSqrTheta12 = 1 - fSSqrTheta12; 

    // Declare quantities to use in loops
    Double_t scale = scale = 1.267e3 * baseline / nuE_parent;
    Double_t fOscProb = 0.0;

    Double_t fDmSqr31 = fDmSqr32 + fDmSqr21;
    Double_t fDmSqr02 = fDmSqr01 + fDmSqr21; // Normal Ordering

    Double_t Sqrs_01 = pow(sin(scale*fDmSqr01),2); Double_t Sqrs_02 = pow(sin(scale*fDmSqr02),2);
    Double_t Sqrs_12 = pow(sin(scale*fDmSqr21),2); Double_t Sqrs_13 = pow(sin(scale*fDmSqr31),2);
    Double_t Sqrs_23 = pow(sin(scale*fDmSqr32),2);  

    fOscProb = 1 - 4*( fSSqrTheta01*fCSqrTheta12*fCSqrTheta13*fCSqrTheta13*fCSqrTheta01*fCSqrTheta12*Sqrs_01 
                    +  fSSqrTheta01*fCSqrTheta12*fSSqrTheta12*fCSqrTheta13*fCSqrTheta13*Sqrs_02
                    +  fCSqrTheta01*fCSqrTheta12*fSSqrTheta12*fCSqrTheta13*fCSqrTheta13*Sqrs_12
                    +  fCSqrTheta01*fCSqrTheta12*fSSqrTheta13*fCSqrTheta13*Sqrs_13
                    +  fSSqrTheta12*fSSqrTheta13*fCSqrTheta13*Sqrs_23);

    std::cout<<"Surv_Prob "<<fOscProb<<std::endl;
  }