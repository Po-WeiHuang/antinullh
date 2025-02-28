#ifndef __ANTINUFIT__FitConfig__
#define __ANTINUFIT__FitConfig__

// OXO headers
#include <ParameterDict.h>
#include <ContainerTools.hpp>

namespace antinufit
{
  class FitConfig
  {
  public:
    ParameterDict GetMinima() const;
    ParameterDict GetMaxima() const;
    ParameterDict GetSigmas() const;
    ParameterDict GetNBins() const;
    ParameterDict GetNominals() const;
    ParameterDict GetFakeDataVals() const;

    ParameterDict GetConstrMeans() const;
    ParameterDict GetConstrSigmas() const;

    ParameterDict GetConstrRatioMeans() const;
    ParameterDict GetConstrRatioSigmas() const;
    std::map<std::string,std::string>   GetConstrRatioParName() const;

    int GetIterations() const;
    void SetIterations(int);
    int GetHMCIterations() const;
    void SetHMCIterations(int);

    int GetBurnIn() const;
    void SetBurnIn(int);
    int GetHMCBurnIn() const;
    void SetHMCBurnIn(int);

    void AddParameter(const std::string &name_, double mean_, double min_, double max_, double sigma_, int nbins_, double fakedata_);
    void AddParameter(const std::string &name_, double mean_, double min_, double max_, double sigma_, int nbins_, double fakedata_,
                      double constrMean_, double constrSigma_);
    void AddParameter(const std::string &name_, double nom_, double min_, double max_, double sigma_, int nbins_, double fakedata_,
                      double constrRatioMean_, double constrRatioSigma_,std::string constrRatioParName_);

    std::set<std::string> GetParamNames() const;

    const std::string &GetOutDir() const;
    void SetOutDir(const std::string &);

    int GetNSteps() const;
    void SetNSteps(int);

    double GetEpsilon() const;
    void SetEpsilon(double);

    double GetSigmaScale() const;
    void SetSigmaScale(double);

    bool GetBeestonBarlow() const;
    void SetBeestonBarlow(bool);

    bool GetAsimov() const;
    void SetAsimov(bool);

    bool GetFakeData() const;
    void SetFakeData(bool);

    std::string GetDatafile() const;
    void SetDatafile(std::string);

    double GetLivetime() const;
    void SetLivetime(double);

  private:
    std::string fOutDir;
    ParameterDict fConstrMeans;
    ParameterDict fConstrSigmas;
    ParameterDict fConstrRatioMeans;
    ParameterDict fConstrRatioSigmas;
    ParameterDict fNominals;
    ParameterDict fFakeDataVals;
    ParameterDict fMinima;
    ParameterDict fMaxima;
    ParameterDict fSigmas;
    ParameterDict fNbins;
    int fIterations;
    int fBurnIn;
    int fHMCIterations;
    int fHMCBurnIn;
    int fNsteps;
    bool fBeestonBarlow;
    double fEpsilon;
    double fSigmaScale;
    bool fAsimov;
    bool fFakeData;
    double fLivetime;
    std::string fDatafile;
    std::map<std::string,std::string> fConstrRatioParName;
  };
}
#endif
