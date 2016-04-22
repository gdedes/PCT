#ifndef __pctScatteringWEPLFunction_h
#define __pctScatteringWEPLFunction_h

#include "pctBetheBlochFunctor.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <iostream>

namespace pct
{

namespace Functor
{

namespace ScatteringWEPL
{

std::vector<double> sigma2_lut;
std::vector<int> thickness_lut;
unsigned int range;

class ScatteringLUT
{
public:
  static double SetLUT(const double eIn)
    {
    double m_IonizationPotential = 78; 
    pct::Functor::IntegratedBetheBlochProtonStoppingPowerInverse<float, double> bethe( m_IonizationPotential * CLHEP::eV, 500 * CLHEP::MeV );

    unsigned int max_waterthickness = 500*CLHEP::mm;
    const double E0 = 13.6*CLHEP::MeV;
    const double X0 = 38.08*CLHEP::cm;
    const double proton_mass_c2 = CLHEP::proton_mass_c2;
    double integratedinvbeta2p2 = 0.;
    double invbeta2p2  = 0.;
    double energycurrent  = 0.;
    double sigma2 = 0.;

    for(unsigned int waterthickness = 1; waterthickness<max_waterthickness; waterthickness++)
      {    
      energycurrent = bethe.GetEnergy(waterthickness*CLHEP::mm, eIn);
      if(energycurrent<0.001) break;        
      invbeta2p2 = (energycurrent+proton_mass_c2)*(energycurrent+proton_mass_c2) / 
                  ((energycurrent+2*proton_mass_c2)*(energycurrent+2*proton_mass_c2) * energycurrent*energycurrent); 
      integratedinvbeta2p2 += invbeta2p2;
      //sigma2 = E0*E0/X0 * (1+0.038*vcl_log(waterthickness/X0))* (1+0.038*vcl_log(waterthickness/X0)) * integratedinvbeta2p2; 
      sigma2 = E0*E0/X0 * integratedinvbeta2p2; 
      sigma2_lut.push_back( sigma2 );
      thickness_lut.push_back(waterthickness);
      range = waterthickness; 
      }
    //return sigma2_lut, thickness_lut ; 
    }
}; // end class Scattering LUT

class ConvertToScatteringWEPL
{

public:
  static double GetValue(const double sigma2)
    {
    double value; 
    double x0=0.,y0=0.,x1=0.,y1=0.;

    for(unsigned int i=0; i<range; i++)
      {
      if(sigma2 < sigma2_lut[i])
      continue;
        {
        //x0 = y0 = x1= y1 = 0.;
        x0 = sigma2_lut[i];
        x1 = sigma2_lut[i+1];
        y0 = thickness_lut[i];
        y1 = thickness_lut[i+1];  
        }
       } 
    // calculate wepl using linear interpolation      
    value = y0 + (y1-y0) * (sigma2-x0) / (x1 - x0); 
    return value;
    }

}; // end class ConvertToScatteringWEPL

}; // end namespace ScatteringWEPL

}; // end namespace Functor

} // end namespace pct

#endif
