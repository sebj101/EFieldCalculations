/*
  InelasticScatter.cxx
*/

#include "Scattering/InelasticScatter.h"

#include <cmath>
#include <random>
#include <stdexcept>

#include "BasicFunctions/Constants.h"

rad::InelasticScatter::InelasticScatter(double T, CalcType calc, Species spec)
    : BaseScatter(T), calcType(calc), species(spec) {}

inline double rad::InelasticScatter::BETA() { return 0.60; }

inline double rad::InelasticScatter::GAMMA() { return 10.0; }

inline double rad::InelasticScatter::G_B() { return 2.9; }

inline double rad::InelasticScatter::n() { return 2.5; }

inline double rad::InelasticScatter::A1() { return 0.74; }

inline double rad::InelasticScatter::A2() { return 0.87; }

inline double rad::InelasticScatter::A3() { return -0.6; }

double rad::InelasticScatter::S() {
  if (calcType == CalcType::Rudd1991) {
    return 4 * TMath::Pi() * pow(A0, 2);
  } else if (calcType == CalcType::Kim1994) {
    const double bohrXSec{4 * M_PI * pow(A0, 2)};
    return bohrXSec * N() * pow(RYDBERG_EV / B(), 2);
  } else {
    throw std::invalid_argument("Calculation type not recognised");
  }
}

double rad::InelasticScatter::GetTotalXSec() {
  if (calcType == CalcType::Rudd1991) {
    double U{GetIncidentKE() / RYDBERG_EV};
    return 1.3 * TMath::Pi() * A0 * A0 * (log(U) + 4) / U;
  } else if (calcType == CalcType::Kim1994) {
    const double t{GetIncidentKE() / B()};
    const double u{U() / B()};
    const double prefac{S() / (t + u + 1)};
    return prefac *
           (D() * log(t) + (2 - Ni() / N()) * ((t - 1) / t - log(t) / (t + 1)));
  } else {
    throw std::invalid_argument("Calculation type not recognised");
  }
}

double rad::InelasticScatter::G2(double omega, double t) {
  return sqrt((omega + 1) / t);
}

double rad::InelasticScatter::G3(double omega, double t) {
  return BETA() * sqrt((1 - pow(G2(omega, t), 2)) / omega);
}

double rad::InelasticScatter::G4(double omega, double t) {
  return GAMMA() * pow(1 - omega / t, 3) / (t * (omega + 1));
}

inline double rad::InelasticScatter::G5() { return 0.33; }

double rad::InelasticScatter::f_BE(double omega, double t, double theta) {
  return 1.0 / (1 + pow((cos(theta) - G2(omega, t)) / G3(omega, t), 2));
}

double rad::InelasticScatter::f_b(double theta) {
  return 1.0 / (1 + pow((cos(theta) + 1) / G5(), 2));
}

double rad::InelasticScatter::g_BE(double omega, double t) {
  double g2{G2(omega, t)};
  double g3{G3(omega, t)};
  double term1{atan2((1 - g2), g3)};
  double term2{atan2((1 + g2), g3)};
  return 2 * TMath::Pi() * g3 * (term1 + term2);
}

double rad::InelasticScatter::F(double t) {
  return (A1() * log(t) + A2() + A3() / t) / t;
}

double rad::InelasticScatter::f_1(double omega, double t) {
  double term1{1 / pow(omega + 1, n())};
  double term2{1 / pow(t - omega, n())};
  double term3{1 / pow((omega + 1) * (t - omega), n() / 2)};
  return term1 + term2 + term3;
}

double rad::InelasticScatter::G1(double omega, double t) {
  double numerator{S() * F(t) * f_1(omega, t) / RYDBERG_EV};
  double denom{g_BE(omega, t) + G4(omega, t) * G_B()};
  return numerator / denom;
}

double rad::InelasticScatter::GetSingleDiffXSec_W(double W) {
  if (calcType == CalcType::Rudd1991) {
    const double omega{W / RYDBERG_EV};
    const double t{GetIncidentKE() / RYDBERG_EV};
    return G1(omega, t) * (g_BE(omega, t) + G4(omega, t) * G_B());
  } else if (calcType == CalcType::Kim1994) {
    const double t{GetIncidentKE() / B()};
    const double u{U() / B()};
    const double omega{W / B()};
    const double prefac{S() / (B() * (t + u + 1))};
    const double term1{(Ni() / N() - 2) / (t + 1) *
                       (1 / (omega + 1) + 1 / (t - omega))};
    const double term2{(2 - Ni() / N()) *
                       (1 / pow(omega + 1, 2) + 1 / pow(t - omega, 2))};
    const double term3{log(t) / (N() * (omega + 1)) *
                       DiffOscillatorStrength(omega)};
    return prefac * (term1 + term2 + term3);
  } else {
    throw std::invalid_argument("Calculation type not recognised");
  }
}

double rad::InelasticScatter::CDF_SingleDiffXSec_W(double omega, double t) {
  const double term1{(1 - pow(omega + 1, 1 - n()) + pow(t - omega, 1 - n()) -
                      pow(t, 1 - n())) /
                     (n() - 1)};
  const double term2{(2 / (n() - 2)) * pow(2 / (t + 1), n() / 2) *
                     (1 - pow(omega + 1, 1 - n() / 2))};
  const double g_1{(1 - pow(t, 1 - n())) / (n() - 1) -
                   pow(2 / (t + 1), n() / 2) * (1 - pow(t, 1 - n() / 2)) /
                       (n() - 2)};
  return 0.5 * (term1 - term2) / g_1;
}

double rad::InelasticScatter::CDF_DoubleDiffXSec_theta(double omega, double t,
                                                       double theta) {
  const double term1{-2 * M_PI * G3(omega, t) *
                     (atan((cos(theta) - G2(omega, t)) / G3(omega, t)) +
                      atan((G2(omega, t) - 1) / G3(omega, t)))};
  const double term2{2 * M_PI * G5() *
                     (atan(2 / G5()) - atan((1 + cos(theta)) / G5()))};
  return (term1 + G4(omega, t) * term2) /
         GetSingleDiffXSec_W(omega * RYDBERG_EV);
}

double rad::InelasticScatter::GetDoubleDiffXSec(double W, double theta) {
  double omega{W / RYDBERG_EV};
  double t{GetIncidentKE() / RYDBERG_EV};
  return G1(omega, t) * (f_BE(omega, t, theta) + G4(omega, t) * f_b(theta));
}

double rad::InelasticScatter::GetRandomW() {
  const unsigned int nBins{5000};
  const double omegaMin{0};
  const double omegaMax{8};
  const double wMax{GetIncidentKE() - RYDBERG_EV};
  const double wMin{(GetIncidentKE() - RYDBERG_EV) / 2};
  const double wBinWidth{(wMax - wMin) / double(nBins)};

  // Build the distribution of differential cross-section
  std::vector<double> wVec{};
  std::vector<double> xsecVec{};
  for (unsigned int iW{0}; iW < nBins; iW++) {
    double w{wMin + wBinWidth / 2 + double(iW) * wBinWidth};
    double diffXSec{GetSingleDiffXSec_W(w)};
    wVec.push_back(w);
    xsecVec.push_back(diffXSec);
  }

  // Sample from the distribution
  std::piecewise_linear_distribution<double> diffXSecDist(
      wVec.begin(), wVec.end(), xsecVec.begin());
  std::random_device rd;
  std::mt19937 mt(rd());
  return diffXSecDist(mt);
}

double rad::InelasticScatter::GetRandomTheta(double W) {
  const unsigned int nBins{400};
  const double thetaMin{0};
  const double thetaMax{TMath::PiOver2()};
  const double thetaBinWidth{(thetaMax - thetaMin) / double(nBins)};

  std::vector<double> thetaVec{};
  std::vector<double> xsecVec{};
  for (unsigned int iT{0}; iT < nBins; iT++) {
    double theta{thetaMin + thetaBinWidth / 2 + double(iT) * thetaBinWidth};
    thetaVec.push_back(theta);
    xsecVec.push_back(GetDoubleDiffXSec(W, theta));
  }

  // Sample from the distribution
  std::piecewise_linear_distribution<double> xsecDist(
      thetaVec.begin(), thetaVec.end(), xsecVec.begin());
  std::random_device rd;
  std::mt19937 mt(rd());
  return xsecDist(mt);
}

double rad::InelasticScatter::GetPrimaryScatteredE(double W, double theta) {
  double E1{GetIncidentKE()};
  double E2Prime{W};
  return E1 - E2Prime - RYDBERG_EV;
}

double rad::InelasticScatter::GetPrimaryScatteredAngle(double W, double theta) {
  double E1{GetIncidentKE()};
  double E1Prime{GetPrimaryScatteredE(W, theta)};
  double E2Prime{W};
  double cTheta1{(E1 + E1Prime - E2Prime) / (2 * sqrt(E1 * E1Prime))};
  return acos(cTheta1);
}

double rad::InelasticScatter::B() {
  if (species == Species::H) {
    return 1.36057e1;
  } else if (species == Species::He) {
    return 2.459e1;
  } else if (species == Species::H2) {
    return 1.543e1;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
}

double rad::InelasticScatter::U() {
  if (species == Species::H) {
    return 1.36057e1;
  } else if (species == Species::He) {
    return 3.951e1;
  } else if (species == Species::H2) {
    return 2.568e1;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
}

double rad::InelasticScatter::N() {
  if (species == Species::H) {
    return 1.0;
  } else if (species == Species::He) {
    return 2.0;
  } else if (species == Species::H2) {
    return 2.0;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
}

double rad::InelasticScatter::Ni() {
  if (species == Species::H) {
    return 0.4343;
  } else if (species == Species::He) {
    return 1.605;
  } else if (species == Species::H2) {
    return 1.173;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
}

double rad::InelasticScatter::D() {
  double b{0}, c{0}, d{0}, e{0}, f{0};
  if (species == Species::H) {
    b = -2.2473e-2;
    c = 1.1775;
    d = -4.6264e-1;
    e = 8.9064e-2;
    f = 0.0;
  } else if (species == Species::He) {
    b = 0.0;
    c = 1.2178e1;
    d = -2.9585e1;
    e = 3.1251e1;
    f = -1.2175e1;
  } else if (species == Species::H2) {
    b = 0.0;
    c = 1.1262;
    d = 6.3982;
    e = -7.8055;
    f = 2.1440;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
  const double tTerm{(GetIncidentKE() / B() + 1) / 2};
  const double bTerm{(b / 2) * (1 - pow(tTerm, -2))};
  const double cTerm{(c / 3) * (1 - pow(tTerm, -3))};
  const double dTerm{(d / 4) * (1 - pow(tTerm, -4))};
  const double eTerm{(e / 5) * (1 - pow(tTerm, -5))};
  const double fTerm{(f / 6) * (1 - pow(tTerm, -6))};
  return (bTerm + cTerm + dTerm + eTerm + fTerm) / N();
}

double rad::InelasticScatter::DiffOscillatorStrength(double omega) {
  double b{0}, c{0}, d{0}, e{0}, f{0};
  if (species == Species::H) {
    b = -2.2473e-2;
    c = 1.1775;
    d = -4.6264e-1;
    e = 8.9064e-2;
    f = 0.0;
  } else if (species == Species::He) {
    b = 0.0;
    c = 1.2178e1;
    d = -2.9585e1;
    e = 3.1251e1;
    f = -1.2175e1;
  } else if (species == Species::H2) {
    b = 0.0;
    c = 1.1262;
    d = 6.3982;
    e = -7.8055;
    f = 2.1440;
  } else {
    throw std::invalid_argument("Species not recognised");
  }
  return b / pow(omega + 1, 2) + c / pow(omega + 1, 3) + d / pow(omega + 1, 4) +
         e / pow(omega + 1, 5) + f / pow(omega + 1, 6);
}