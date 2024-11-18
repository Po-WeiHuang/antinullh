#include "OscGrid.hh"
#include "TFile.h"

namespace antinufit
{

  OscGrid::OscGrid(const std::string &filename_)
  {

    fFilename = filename_;
    Load();
  }

  void OscGrid::Load()
  {

    std::cout << "Loading probability information from " << fFilename << std::endl;

    TFile *file = new TFile(fFilename.c_str(), "READ");

    fEnergyVals.clear();
    fSsqth12Vals.clear();
    fDm21sqVals.clear();

    std::vector<Double_t> *tmpEnergyVals;
    std::vector<Double_t> *tmpSsqth12Vals;
    std::vector<Double_t> *tmpDm21sqVals;
    std::vector<Double_t> *tmpProbVals;
    std::vector<Double_t> *tmpDistance;

    file->GetObject("fEnergyVals", tmpEnergyVals);
    file->GetObject("fSsqth12Vals", tmpSsqth12Vals);
    file->GetObject("fDm21sqVals", tmpDm21sqVals);
    file->GetObject("fProbVals", tmpProbVals);
    file->GetObject("fDistance", tmpDistance);

    fEnergyVals = *tmpEnergyVals;
    fSsqth12Vals = *tmpSsqth12Vals;
    fDm21sqVals = *tmpDm21sqVals;
    fProbVals = *tmpProbVals;
    fDistance = tmpDistance->at(0);

    fMinE = fEnergyVals.at(0);
    fMaxE = fEnergyVals.back();
    fNumValsE = fEnergyVals.size();
    fMinDm21sq = fDm21sqVals.at(0);
    fMaxDm21sq = fDm21sqVals.back();
    fNumValsDm21sq = fDm21sqVals.size();
    fMinSsqth12 = fSsqth12Vals.at(0);
    fMaxSsqth12 = fSsqth12Vals.back();
    fNumValsSsqth12 = fSsqth12Vals.size();

    std::cout << "Loading of OscGrid complete!" << std::endl
              << std::endl;
  }

  void OscGrid::CalcGrid()
  {

    fEnergyVals = linspace(fMinE, fMaxE, fNumValsE);
    fDm21sqVals = linspace(fMinDm21sq, fMaxDm21sq, fNumValsDm21sq);
    fSsqth12Vals = linspace(fMinSsqth12, fMaxSsqth12, fNumValsSsqth12);

    const size_t numVals = fNumValsE * fNumValsDm21sq * fNumValsSsqth12;
    fProbVals.reserve(numVals);
    size_t i = 0;

    for (int idmsq21 = 0; idmsq21 < fDm21sqVals.size(); idmsq21++)
    {
      for (int issqth12 = 0; issqth12 < fSsqth12Vals.size(); issqth12++)
      {
        for (int ienergy = 0; ienergy < fEnergyVals.size(); ienergy++)
        {

          if (i % (numVals / 10) == 0)
          {
            std::cout << (i * 100) / numVals << "% events complete (" << i << "/" << numVals << ")" << std::endl;
          }
          double prob = OscProb2(fDistance, fEnergyVals.at(ienergy), fDm21sqVals.at(idmsq21), fSsqth12Vals.at(issqth12));
          if (prob > 1.0 || prob < 0.0)
          {
            std::cout << "hang on prob(" << i << ") = " << prob << std::endl;
          }
          fProbVals.push_back(prob);
          i++;
        }
      }
    }
  }

  void OscGrid::Write()
  {

    std::cout << std::endl
              << "Writing Oscillation Grid information to " << fFilename << std::endl;

    // Open output file for writing
    TFile *file = new TFile(fFilename.c_str(), "RECREATE");

    file->WriteObject(&fEnergyVals, "fEnergyVals");
    file->WriteObject(&fDm21sqVals, "fDm21sqVals");
    file->WriteObject(&fSsqth12Vals, "fSsqth12Vals");
    file->WriteObject(&fProbVals, "fProbVals");

    std::vector<double> distvec;
    distvec.push_back(fDistance);
    file->WriteObject(&distvec, "fDistance");

    std::cout << "Writing complete!" << std::endl
              << std::endl;
  }
  // output a surv_prob for given nuE, sintheta21 and delM21^2
  double OscGrid::Evaluate(double nuEnergy, double dmsq21, double ssqth12)
  {
    /*
     * Use trilinear interpolation in order to estimate the Pee survival probability for a given
     * neutrino energy, dm^2_{21}, and sin^2(theta_{21}).
     */
    // First confirm input values are in the interpolation range

    if (nuEnergy < fMinE || nuEnergy > fMaxE)
    {
      std::cerr << "ERROR: Neutrino Energy, " << nuEnergy << " MeV, outside bounds: " << fMinE << "-" << fMaxE << " MeV" << std::endl;
      throw;
    }
    if (dmsq21 < fMinDm21sq || dmsq21 > fMaxDm21sq)
    {
      std::cerr << "ERROR: #Delta m^2_{21}, " << dmsq21 << " x 10^-5 MeV^2, outside bounds: " << fMinDm21sq << "-" << fMaxDm21sq << " x 10^-5 MeV^2" << std::endl;
      throw;
    }
    if (ssqth12 < fMinSsqth12 || ssqth12 > fMaxSsqth12)
    {
      std::cerr << "ERROR: sin^2 #theta_{12}, " << ssqth12 << ", outside bounds: " << fMinSsqth12 << "-" << fMaxSsqth12 << std::endl;
      throw;
    }

    // Now, run binary searches to find the relevant 8 vertices within which the position lies
    const auto energyBounds = GetLowerUpperIndices(fEnergyVals, nuEnergy);
    const auto dmsqBounds = GetLowerUpperIndices(fDm21sqVals, dmsq21);
    const auto thetaBounds = GetLowerUpperIndices(fSsqth12Vals, ssqth12);

    // Actually get the vertex positions & associated probabilities
    std::vector<FL::point<3, double>> vertices;
    int numbounds = 2;
    for (size_t i = 0; i < numbounds; ++i)
    {
      const size_t thetaIndex = (i == 0) ? thetaBounds.first : thetaBounds.second;
      const double theta = fSsqth12Vals.at(thetaIndex);
      for (size_t j = 0; j < numbounds; ++j)
      {
        const size_t dmsqIndex = (j == 0) ? dmsqBounds.first : dmsqBounds.second;
        const double dmsq = fDm21sqVals.at(dmsqIndex);
        for (size_t k = 0; k < numbounds; ++k)
        {
          const size_t eIndex = (k == 0) ? energyBounds.first : energyBounds.second;
          const double e = fEnergyVals.at(eIndex);

          FL::point<3, double> vertex{{e, dmsq, theta},
                                      fProbVals.at(eIndex + thetaIndex * fEnergyVals.size() +
                                                   dmsqIndex * fEnergyVals.size() * fSsqth12Vals.size())};
          vertices.push_back(vertex);
        }
      }
    }
    // Finally, perform the trilinear interpolation!
    FL::point<3, double> p = {{nuEnergy, dmsq21, ssqth12}, 0};
    return FL::LinearInterpolator<double>::Trilinear(p, vertices.data()).val;
  }

  TH3D *OscGrid::MakeHist()
  {

    hist = new TH3D("probhist", "probhist:dm21sq,sinssqth21,energy", fDm21sqVals.size(), fDm21sqVals.front(), fDm21sqVals.back(), fSsqth12Vals.size(), fSsqth12Vals.front(), fSsqth12Vals.back(), fEnergyVals.size(), fEnergyVals.front(), fEnergyVals.back());

    for (int iDmsq21 = 0; iDmsq21 < fDm21sqVals.size(); iDmsq21++)
    {
      for (int iSsqth21 = 0; iSsqth21 < fSsqth12Vals.size(); iSsqth21++)
      {
        for (int iEnergy = 0; iEnergy < fEnergyVals.size(); iEnergy++)
        {

          int probIndex = iDmsq21 * (fSsqth12Vals.size() * fEnergyVals.size()) + iSsqth21 * fEnergyVals.size() + iEnergy;
          hist->Fill(fDm21sqVals.at(iDmsq21), fSsqth12Vals.at(iSsqth21), fEnergyVals.at(iEnergy), fProbVals.at(probIndex));
        }
      }
    }

    return hist;
  }
}
