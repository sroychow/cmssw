#ifndef HiggsTripleElectronMonitor_H
#define HiggsTripleElectronMonitor_H

#include <string>
#include <vector>
#include <map>

#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include <DQMServices/Core/interface/DQMEDAnalyzer.h>

#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/Registry.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

//DataFormats
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

class GenericTriggerEventFlag;

struct MEbinning {
  int nbins;
  double xmin;
  double xmax;
};


struct LEPME {
  MonitorElement* numerator;
  MonitorElement* denominator;
};

template <class T>
class PtComparator {
  public:
  bool operator()(const T &a, const T &b) const {
      return a.pt() > b.pt();
  }
};
//
// class declaration
//

class HiggsTripleElectronMonitor : public DQMEDAnalyzer 
{
public:
  HiggsTripleElectronMonitor( const edm::ParameterSet& );
  ~HiggsTripleElectronMonitor();
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
  static void fillHistoPSetDescription(edm::ParameterSetDescription & pset);
  static void fillHistoLSPSetDescription(edm::ParameterSetDescription & pset);

protected:

  void bookHistograms(DQMStore::IBooker &, edm::Run const &, edm::EventSetup const &) override;
  void bookME(DQMStore::IBooker &, LEPME& me, const std::string& histname, const std::string& histtitle, int nbins, double xmin, double xmax);
  void bookME(DQMStore::IBooker &, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX);
  void bookME(DQMStore::IBooker &, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, double ymin, double ymax);
  void bookME(DQMStore::IBooker &, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, int nbinsY, double ymin, double ymax);
  void bookME(DQMStore::IBooker &, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX, const std::vector<double>& binningY);
  void setMETitle(LEPME& me, std::string titleX, std::string titleY);
  void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) override;

private:
  static MEbinning getHistoPSet    (edm::ParameterSet pset);
  static MEbinning getHistoLSPSet  (edm::ParameterSet pset);

  std::string folderName_;
  std::string histoSuffix_;

  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  edm::EDGetTokenT<reco::VertexCollection> vtxToken_;

  std::vector<double> met_variable_binning_;
  MEbinning           met_binning_;
  MEbinning           ls_binning_;
  MEbinning           phi_binning_;
  MEbinning           eta_binning_;
  
  LEPME l1ptME_;
  LEPME l1ptME_variableBinning_;
  LEPME l1ptVsLS_;
  LEPME l1PhiME_;
  LEPME l1EtaME_;
  LEPME l1PtEta2DME_;

  LEPME l2ptME_;
  LEPME l2ptME_variableBinning_;
  LEPME l2ptVsLS_;
  LEPME l2PhiME_;
  LEPME l2EtaME_;
  LEPME l2PtEta2DME_;

  LEPME l3ptME_;
  LEPME l3ptME_variableBinning_;
  LEPME l3ptVsLS_;
  LEPME l3PhiME_;
  LEPME l3EtaME_;
  LEPME l3PtEta2DME_;

  GenericTriggerEventFlag* num_genTriggerEventFlag_;
  GenericTriggerEventFlag* den_genTriggerEventFlag_;

  StringCutObjectSelector<reco::GsfElectron,true> eleSelection_;
  StringCutObjectSelector<reco::Vertex,true>      vtxSelection_;
  int nelectrons_;
  double dxycut_;
  double dzcut_;
};

#endif // HiggsTripleElectronMonitor_H
