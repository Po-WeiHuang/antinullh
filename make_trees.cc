/*
MakeTrees takes in ntuples and prunes out all the branches
we're not interested in so we only have nice lightweight files
to carry around in the fit.
It reads in input files specified in the event config and
literally just loops over events, filling new ntuples
with the quantities we want. These get written to wherever
was specified in the config file.
*/

// Antinu headers
#include <EventConfigLoader.hh>
#include <OscGridConfigLoader.hh>
#include <Utilities.hh>

// ROOT headers
#include <TChain.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TVector3.h>

// c++ headers
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

using namespace antinufit;

typedef std::map<std::string, std::string> StringMap;

// Taken from antinu rat-tools
TVector3 LLAtoECEF(double longitude, double latitude, double altitude)
{
  // reference http://www.mathworks.co.uk/help/aeroblks/llatoecefposition.html
  static double toRad = TMath::Pi() / 180.;
  static double Earthradius = 6378137.0; // Radius of the Earth (in meters)
  static double f = 1. / 298.257223563;  // Flattening factor WGS84 Model
  static double L, rs, x, y, z;
  L = atan(pow((1. - f), 2) * tan(latitude * toRad)) * 180. / TMath::Pi();
  rs = sqrt(pow(Earthradius, 2) / (1. + (1. / pow((1. - f), 2) - 1.) * pow(sin(L * toRad), 2)));
  x = (rs * cos(L * toRad) * cos(longitude * toRad) + altitude * cos(latitude * toRad) * cos(longitude * toRad)) / 1000; // in km
  y = (rs * cos(L * toRad) * sin(longitude * toRad) + altitude * cos(latitude * toRad) * sin(longitude * toRad)) / 1000; // in km
  z = (rs * sin(L * toRad) + altitude * sin(latitude * toRad)) / 1000;                                                   // in km

  TVector3 ECEF = TVector3(x, y, z);

  return ECEF;
}

// Taken from antinu rat-tools
double GetReactorDistanceLLA(const double &longitude, const double &latitude, const double &altitude)
{
  const TVector3 SNO_ECEF_coord_ = TVector3(672.87, -4347.18, 4600.51);
  double dist = (LLAtoECEF(longitude, latitude, altitude) - SNO_ECEF_coord_).Mag();
  return dist;
}

void MakeDataSet(const std::vector<std::string> &filenames_,
                 const std::string &baseDir_,
                 const std::string &outFilename_,
                 std::unordered_map<std::string, int> &reactorNameIndex)
{

  // Output ntuple
  TFile outp(outFilename_.c_str(), "RECREATE");
  TNtuple nt("pruned", "", "energy:nu_energy:reactorIndex:alphaNClassifier");

  // Read the original data
  const std::string treeName = "output";
  for (size_t iFile = 0; iFile < filenames_.size(); iFile++)
  {
    std::string fileName = baseDir_ + "/" + filenames_.at(iFile);
    TChain chain(treeName.c_str());
    chain.Add(fileName.c_str());
    std::cout << fileName << "\t" << chain.GetEntries() << "  entries" << std::endl;

    int tenPercent = chain.GetEntries() / 10;

    Double_t energy;
    Double_t alphaNClassifier;
    TString *reactorName = NULL;
    Int_t reactorIndex;
    Double_t distance;
    Double_t neutrinoEnergy;

    chain.SetBranchAddress("energy", &energy);
    chain.SetBranchAddress("alphaNReactorIBD", &alphaNClassifier);
    chain.SetBranchAddress("parentKE1", &neutrinoEnergy);
    chain.SetBranchAddress("parentMeta1", &reactorName);

    // Read and write
    for (int i = 0; i < chain.GetEntries(); i++)
    {
      if (!(i % tenPercent))
      {
        std::cout << i << " / " << chain.GetEntries() << "\t ( " << 10 * i / tenPercent << " %)" << std::endl;
      }
      chain.GetEntry(i);
      std::string originReactorString(reactorName->Data());
      if (originReactorString != "")
      {
        reactorIndex = reactorNameIndex[originReactorString];
      }
      else
        reactorIndex = 999;
      outp.cd();
      nt.Fill(energy, neutrinoEnergy, reactorIndex, alphaNClassifier);
    }
  }
  outp.cd();
  nt.Write();
  return;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Usage: make_trees <event_config_file> <osc_grid_config_file>" << std::endl;
    return 1;
  }

  // Load up the reactors json file
  std::string oscConfigFile(argv[2]);
  OscGridConfigLoader oscGridLoader(oscConfigFile);
  OscGridConfig oscGridConfig = oscGridLoader.Load();
  std::string reactorsJSONFile = oscGridConfig.GetReactorsJsonFile();
  std::unordered_map<std::string, int> reactorNameIndex = LoadNameIndexMap(reactorsJSONFile);

  // Create the results directory if it doesn't already exist
  std::string outDir;
  std::string eveConfigFile(argv[1]);
  ConfigLoader::Open(eveConfigFile);
  ConfigLoader::Load("summary", "pruned_ntup_dir", outDir);
  ConfigLoader::Close();
  EventConfigLoader eveLoader(eveConfigFile);
  typedef std::map<std::string, antinufit::EventConfig> EvMap;
  typedef std::vector<std::string> StringVec;
  EvMap toGet = eveLoader.LoadActive();

  struct stat st = {0};
  if (stat(outDir.c_str(), &st) == -1)
  {
    mkdir(outDir.c_str(), 0700);
  }
  std::cout << "Made " << outDir << std::endl;

  // If there is a common path preprend it
  for (EvMap::iterator it = toGet.begin(); it != toGet.end(); ++it)
  {
    const std::string &name = it->first;
    std::cout << "Doing " << name << std::endl;
    const std::string &baseDir = it->second.GetNtupBaseDir();
    const StringVec &files = it->second.GetNtupFiles();
    const std::string &outName = it->second.GetPrunedPath();

    std::cout << "Writing from :" << std::endl;
    for (size_t i = 0; i < files.size(); i++)
      std::cout << "\t" << files.at(i) << std::endl;
    std::cout << "to " << outName << std::endl;

    MakeDataSet(files, baseDir, outName, reactorNameIndex);
  }

  return 0;
}
