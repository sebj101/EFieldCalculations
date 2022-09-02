/*
  RectangularWaveguide.h

  Derived class for a rectangular waveguide
*/

#ifndef RECTANGULAR_WAVEGUIDE_H
#define RECTANGULAR_WAVEGUIDE_H

#include "Waveguides/IWaveguide.h"

namespace rad {

  /// Class describing a rectangular waveguide
  /// Centre of waveguide located at x = y = z = 0
  class RectangularWaveguide : public IWaveguide {

  private:
    // Convention is that a > b and that longest side of waveguide is x-axis
    double a; // Side length in metres
    double b; // Side length in metres
    double d; // Axial length in metres

    /// Gets the cutoff wavenumber for a given mode
    /// \param m Mode order in the x direction of the waveguide 
    /// \param n Mode order in the y direction of the waveguide
    /// \Returns The cutoff wavenumber in units of m^{-1}
    double GetCutoffWavenumber(unsigned int m, unsigned int n);
    
  public:
    /// Parametrised constructor
    /// \param longSide The longer side of the waveguide (in metres)
    /// \param shortSide The shorter side of the waveguide (in metres)
    /// \param length The axial length of the waveguide (in metres);
    RectangularWaveguide(double longSide, double shortSide, double length);

    /// Gets the long dimension of the waveguide
    /// \Returns The long (x) dimension of the waveguide (in metres)
    double GetLongDimension() { return a; }

    /// Gets the short dimension of the waveguide
    /// \Returns The short (y) dimension of the waveguide (in metres)
    double GetShortDimension() { return b; }

    /// Gets the complex electric field vector for a given mode at a point
    /// \param modeType The mode type to get (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide 
    /// \param n The mode number in the y direction of the waveguide 
    /// \param pos The position vector (in metres)
    /// \param omega Angular frequency of the chosen wave
    /// \param A Arbitrary amplitude for solution (default = 1)
    /// \param B Arbitrary amplitude for solution has no effect
    /// \Returns The mode electric field vector at the supplied point
    ComplexVector3 GetModeEFieldComplex(Mode_t modeType, int m, int n, TVector3 pos, double omega, double A=1, double B=0);
    
    /// Gets the real electric field vector for a given mode at a point
    /// \param modeType The mode type to get (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide 
    /// \param n The mode number in the y direction of the waveguide 
    /// \param pos The position vector (in metres)
    /// \param omega Angular frequency of the chosen wave
    /// \param A Arbitrary amplitude for solution (default = 1)
    /// \param B Arbitrary amplitude for solution has no effect
    /// \Returns The mode electric field vector at the supplied point
    TVector3 GetModeEField(Mode_t modeType, int m, int n, TVector3 pos, double omega, double A=1, double B=0);

    /// Gets the complex magnetic field strength vector for a given mode at a point
    /// \param modeType The mode type to get (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide 
    /// \param n The mode number in the y direction of the waveguide 
    /// \param pos The position vector (in metres)
    /// \param omega Angular frequency of the chosen wave
    /// \param A Arbitrary amplitude for solution (default = 1)
    /// \Returns The mode H field vector at the supplied point
    ComplexVector3 GetModeHFieldComplex(Mode_t modeType, int m, int n, TVector3 pos, double omega, double A=1, double B=0);
    
    /// Gets the H field vector for a given mode at a point
    /// \param modeType The mode type to get (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide 
    /// \param n The mode number in the y direction of the waveguide 
    /// \param pos The position vector (in metres)
    /// \param omega Angular frequency of the chosen wave
    /// \param A Arbitrary amplitude for solution (default = 1)
    /// \Returns The mode H field vector at the supplied point
    TVector3 GetModeHField(Mode_t modeType, int m, int n, TVector3 pos, double omega, double A=1, double B=0);

    /// Gets the characteristic impedance for a particular mode
    /// \param modeType The mode type to use (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide
    /// \param n The mode number in the y direction of the waveguide
    /// \param omega Angular frequency of the chosen wave
    /// \Returns The impedance of the mode (in Ohms)
    double GetModeImpedance(Mode_t modeType, int m, int n, double omega);
    
    /// Gets the cutoff frequency for a particular mode
    /// \param modeType The mode type to use (either TE or TM)
    /// \param m The mode number in the x direction of the waveguide
    /// \param n The mode number in the y direction of the waveguide
    /// \Returns The cutoff frequency of the mode in Hertz
    double GetCutoffFrequency(Mode_t modeType, int m, int n);

    /// Gets the resonant frequency for a particle mode
    /// \param modeType The type of mode (TE or TM)
    /// \param m The mode number in the x direction of the waveguide
    /// \param n The mode number in the y direction of the waveguide
    /// \param l The mode number in the z direction of the waveguide
    /// \Returns The resonant frequency of the chosen mode (in Hertz)
    double GetResonantModeFrequency(Mode_t modeType, int m, int n, int l);
  };
}

#endif
