// Antinu headers
#include <DistConfigLoader.hh>
#include <DistBuilder.hh>
#include <FitConfigLoader.hh>
#include <EventConfigLoader.hh>
#include <SystConfigLoader.hh>
#include <SystFactory.hh>
#include <OscGridConfigLoader.hh>

// OXO headers
#include <ROOTNtuple.h>
#include <BinnedNLLH.h>
#include <IO.h>
#include <Rand.h>

// ROOT headers
#include <TH1D.h>

// c++ headers
#include <sys/stat.h>

using namespace antinufit;

void llh_scan(const std::string &mcmcConfigFile_,
              const std::string &evConfigFile_,
              const std::string &pdfConfigFile_,
              const std::string &systConfigFile_,
              const std::string &oscGridConfigFile_)
{
  Rand::SetSeed(0);
      // Load up the fit configuration information
  // Load up the systematics
  SystConfigLoader systLoader(systConfigFile_);
  SystConfig systConfig = systLoader.LoadActive();
  std::map<std::string, std::string> systParamNames = systConfig.GetParamNames();
  std::map<std::string, std::string> systGroup = systConfig.GetGroup();
  std::map<std::string, std::string> systType = systConfig.GetType();
  std::map<std::string, std::vector<std::string>> systDistObs = systConfig.GetDistObs();
  std::map<std::string, std::vector<std::string>> systTransObs = systConfig.GetTransObs();
  std::map<std::string, std::string> systFunctionNames = systConfig.GetFunctionNames();
  std::vector<std::string> fullParamNameVec;

  for (std::map<std::string, std::string>::iterator it = systGroup.begin(); it != systGroup.end(); ++it) {
        std::cout << it->first <<" "<< it->second <<std::endl;
    }
  

  
}

int main(int argc, char *argv[])
{
  if (argc != 6)
  {
    std::cout << "\nUsage: llh_scan <fit_config_file> <eve_config_file> <pdf_config_file> <syst_config_file> <oscgrid_config_file>" << std::endl;
    return 1;
  }

  std::string fitConfigFile(argv[1]);
  std::string eveConfigFile(argv[2]);
  std::string pdfConfigPath(argv[3]);
  std::string systConfigFile(argv[4]);
  std::string oscgridConfigFile(argv[5]);

  llh_scan(fitConfigFile, eveConfigFile, pdfConfigPath, systConfigFile, oscgridConfigFile);

  return 0;
}