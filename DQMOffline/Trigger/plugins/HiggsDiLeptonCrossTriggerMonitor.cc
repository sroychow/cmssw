#include "DQMOffline/Trigger/plugins/HiggsDiLeptonCrossTriggerMonitor.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQM/TrackingMonitor/interface/GetLumi.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"
#include<algorithm>

// -----------------------------
//  constructors and destructor
// -----------------------------

HiggsDiLeptonCrossTriggerMonitor::HiggsDiLeptonCrossTriggerMonitor( const edm::ParameterSet& iConfig ) : 
  folderName_             ( iConfig.getParameter<std::string>("FolderName") )
  , eleToken_             ( mayConsume<edm::View<reco::GsfElectron> > (iConfig.getParameter<edm::InputTag>("electrons") ) )   
  , muoToken_             ( mayConsume<reco::MuonCollection>         (iConfig.getParameter<edm::InputTag>("muons")     ) )   
  , vtxToken_             ( mayConsume<reco::VertexCollection>       (iConfig.getParameter<edm::InputTag>("vertices")     ) )   
  , met_variable_binning_ ( iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<std::vector<double> >("lepPtBinning") )
  , met_binning_          ( getHistoPSet   (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("lepPSet")    ) )
  , ls_binning_           ( getHistoLSPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("lsPSet")     ) )
  , phi_binning_          ( getHistoPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("phiPSet")     ) )
  , eta_binning_          ( getHistoPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("etaPSet")     ) )
  , num_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("numGenericTriggerEventPSet"),consumesCollector(), *this))
  , den_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("denGenericTriggerEventPSet"),consumesCollector(), *this))
  , eleSelection_ ( iConfig.getParameter<std::string>("eleSelection") )
  , muoSelection_ ( iConfig.getParameter<std::string>("muoSelection") )
  , vtxSelection_ ( iConfig.getParameter<std::string>("vertexSelection") )
  , nelectrons_ ( iConfig.getParameter<int>("nelectrons" ) )
  , nmuons_     ( iConfig.getParameter<int>("nmuons" )     )
  , dxycut_     ( iConfig.getParameter<double>("leptondxycut" )     )
  , dzcut_      ( iConfig.getParameter<double>("leptondzcut" )     )
{
  eleptME_.numerator   = nullptr;
  eleptME_.denominator = nullptr;
  eleptME_variableBinning_.numerator   = nullptr;
  eleptME_variableBinning_.denominator = nullptr;
  eleptVsLS_.numerator   = nullptr;
  eleptVsLS_.denominator = nullptr;
  elePhiME_.numerator   = nullptr;
  elePhiME_.denominator = nullptr;
  eleEtaME_.numerator   = nullptr;
  eleEtaME_.denominator = nullptr;
  elePtEta2DME_.numerator   = nullptr;
  elePtEta2DME_.denominator = nullptr;

  muptME_.numerator   = nullptr;
  muptME_.denominator = nullptr;
  muptME_variableBinning_.numerator   = nullptr;
  muptME_variableBinning_.denominator = nullptr;
  muptVsLS_.numerator   = nullptr;
  muptVsLS_.denominator = nullptr;
  muPhiME_.numerator   = nullptr;
  muPhiME_.denominator = nullptr;
  muEtaME_.numerator   = nullptr;
  muEtaME_.denominator = nullptr;
  muPtEta2DME_.numerator   = nullptr;
  muPtEta2DME_.denominator = nullptr;
}

HiggsDiLeptonCrossTriggerMonitor::~HiggsDiLeptonCrossTriggerMonitor()
{
  if (num_genTriggerEventFlag_) delete num_genTriggerEventFlag_;
  if (den_genTriggerEventFlag_) delete den_genTriggerEventFlag_;
}

MEbinning HiggsDiLeptonCrossTriggerMonitor::getHistoPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      pset.getParameter<double>("xmin"),
      pset.getParameter<double>("xmax"),
      };
}

MEbinning HiggsDiLeptonCrossTriggerMonitor::getHistoLSPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      0.,
      double(pset.getParameter<int32_t>("nbins"))
      };
}

void HiggsDiLeptonCrossTriggerMonitor::setMETitle(LEPME& me, std::string titleX, std::string titleY)
{
  me.numerator->setAxisTitle(titleX,1);
  me.numerator->setAxisTitle(titleY,2);
  me.denominator->setAxisTitle(titleX,1);
  me.denominator->setAxisTitle(titleY,2);

}

void HiggsDiLeptonCrossTriggerMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbins, double min, double max)
{
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, min, max);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, min, max);
}
void HiggsDiLeptonCrossTriggerMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binning)
{
  int nbins = binning.size()-1;
  std::vector<float> fbinning(binning.begin(),binning.end());
  float* arr = &fbinning[0];
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, arr);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, arr);
}
void HiggsDiLeptonCrossTriggerMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, double ymin, double ymax)
{
  me.numerator   = ibooker.bookProfile(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, ymin, ymax);
  me.denominator = ibooker.bookProfile(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, ymin, ymax);
}
void HiggsDiLeptonCrossTriggerMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, int nbinsY, double ymin, double ymax)
{
  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, nbinsY, ymin, ymax);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, nbinsY, ymin, ymax);
}
void HiggsDiLeptonCrossTriggerMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX, const std::vector<double>& binningY)
{
  int nbinsX = binningX.size()-1;
  std::vector<float> fbinningX(binningX.begin(),binningX.end());
  float* arrX = &fbinningX[0];
  int nbinsY = binningY.size()-1;
  std::vector<float> fbinningY(binningY.begin(),binningY.end());
  float* arrY = &fbinningY[0];

  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, arrX, nbinsY, arrY);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, arrX, nbinsY, arrY);
}

void HiggsDiLeptonCrossTriggerMonitor::bookHistograms(DQMStore::IBooker     & ibooker,
				 edm::Run const        & iRun,
				 edm::EventSetup const & iSetup) 
{  
  std::string histname, histtitle;

  std::string currentFolder = folderName_ ;
  ibooker.setCurrentFolder(currentFolder.c_str());
  
  //book electron histograms
  histname = "elePt"; histtitle = "Electron p_T";
  bookME(ibooker,eleptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(eleptME_,"p_T [GeV]","events / [GeV]");

  histname = "elePt_variable"; histtitle = "Electron p_{T}";
  bookME(ibooker,eleptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(eleptME_variableBinning_,"p_T [GeV]","events / [GeV]");

  histname = "elePtVsLS"; histtitle = "Electron p_T vs LS";
  bookME(ibooker,eleptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(eleptVsLS_,"LS","p_T [GeV]");

  histname = "elePhi"; histtitle = "Electron phi";
  bookME(ibooker,elePhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(elePhiME_,"#phi","events / 0.1 rad");

  histname = "eleEta"; histtitle = "Electron #eta";
  bookME(ibooker,eleEtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(eleEtaME_,"#eta","events");

  histname = "elePtEta"; histtitle = "Electron p_T-#eta";
  bookME(ibooker,elePtEta2DME_,histname,histtitle, met_binning_.nbins, met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(elePtEta2DME_, "p_T", "#eta");

  //book muon hisograms
  histname = "muPt"; histtitle = "Muon p_T";
  bookME(ibooker,muptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(muptME_,"p_T [GeV]","events / [GeV]");

  histname = "muPt_variable"; histtitle = "Muon p_{T}";
  bookME(ibooker,muptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(muptME_variableBinning_,"p_T [GeV]","events / [GeV]");

  histname = "muPtVsLS"; histtitle = "Muon p_T vs LS";
  bookME(ibooker,muptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(muptVsLS_,"LS","p_T [GeV]");

  histname = "muPhi"; histtitle = "Muon phi";
  bookME(ibooker,muPhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(muPhiME_,"#phi","events / 0.1 rad");

  histname = "muEta"; histtitle = "Muon #eta";
  bookME(ibooker,muEtaME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(muEtaME_,"#eta","events");

  histname = "muPtEta"; histtitle = "Muon p_T-#eta";
  bookME(ibooker,muPtEta2DME_,histname,histtitle, met_binning_.nbins, met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(muPtEta2DME_, "p_T", "#eta");

  // Initialize the GenericTriggerEventFlag
  if ( num_genTriggerEventFlag_ && num_genTriggerEventFlag_->on() ) num_genTriggerEventFlag_->initRun( iRun, iSetup );
  if ( den_genTriggerEventFlag_ && den_genTriggerEventFlag_->on() ) den_genTriggerEventFlag_->initRun( iRun, iSetup );

}

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrackFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
void HiggsDiLeptonCrossTriggerMonitor::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup)  {

  // Filter out events if Trigger Filtering is requested
  if (den_genTriggerEventFlag_->on() && ! den_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;

  edm::Handle<reco::VertexCollection> primaryVertices;
  iEvent.getByToken(vtxToken_, primaryVertices);
  //Primary Vertex selection
  const reco::Vertex* pv = nullptr;
  for(auto const& v: *primaryVertices) {
    if ( !vtxSelection_( v ) )      continue;
    pv = &v;
    break;
  }
  
  if( pv == nullptr)            return;
   edm::Handle<edm::View<reco::GsfElectron> > electronHandle;
  iEvent.getByToken( eleToken_, electronHandle );
  if ( int(electronHandle->size()) < nelectrons_ ) return;    
  std::vector<reco::GsfElectron> electrons;
  for(auto const & e : *electronHandle) {
    //preselection
    if ( !eleSelection_( e ) )      continue;
    //implement hzz ip cuts
    if( std::fabs(e.gsfTrack()->dxy(pv->position())) >= dxycut_ || std::fabs(e.gsfTrack()->dz(pv->position())) >= dzcut_ )   continue;
    electrons.push_back(e);
  }

  edm::Handle<reco::MuonCollection> muoHandle;
  iEvent.getByToken( muoToken_, muoHandle );
  if ( int(muoHandle->size()) < nmuons_ ) return;
  std::vector<reco::Muon> muons;
  for ( auto const & m : *muoHandle ) {
    //preselection
    if ( !muoSelection_( m ) )           continue;
    //hzz ip cuts
    if( std::fabs(m.muonBestTrack()->dxy(pv->position())) >= dxycut_ || std::fabs(m.muonBestTrack()->dz(pv->position())) >= dzcut_ )    continue;
    muons.push_back(m);
  }
  //check the no. of electrons and muons after selection is applied
  if ( int(electrons.size()) < nelectrons_ ) return;
  if ( int(muons.size()) < nmuons_ ) return;

  float ele_pt  = electrons[0].pt();
  float ele_phi = electrons[0].phi();
  float ele_eta = electrons[0].eta();
  float mu_pt   = muons[0].pt();
  float mu_phi  = muons[0].phi();
  float mu_eta  = muons[0].eta();
  // filling histograms (denominator)  
  eleptME_.denominator->Fill(ele_pt);
  eleptME_variableBinning_.denominator->Fill(ele_pt);
  elePhiME_.denominator->Fill(ele_phi);
  eleEtaME_.denominator->Fill(ele_eta);
  elePtEta2DME_.denominator->Fill(ele_pt,ele_eta);

  muptME_.denominator->Fill(mu_pt);
  muptME_variableBinning_.denominator->Fill(mu_pt);
  muPhiME_.denominator->Fill(mu_phi);
  muEtaME_.denominator->Fill(mu_eta);
  muPtEta2DME_.denominator->Fill(mu_pt,mu_eta);

  int ls = iEvent.id().luminosityBlock();
  eleptVsLS_.denominator->Fill(ls, ele_pt);
  muptVsLS_.denominator->Fill(ls, mu_pt);
  
  // applying selection for numerator
  if (num_genTriggerEventFlag_->on() && ! num_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;
  // filling histograms (num_genTriggerEventFlag_)  
  eleptME_.numerator->Fill(ele_pt);
  eleptME_variableBinning_.numerator->Fill(ele_pt);
  elePhiME_.numerator->Fill(ele_phi);
  eleEtaME_.numerator->Fill(ele_eta);
  elePtEta2DME_.numerator->Fill(ele_pt, ele_eta);

  muptME_.numerator->Fill(mu_pt);
  muptME_variableBinning_.numerator->Fill(mu_pt);
  muPhiME_.numerator->Fill(mu_phi);
  muEtaME_.numerator->Fill(mu_eta);
  muPtEta2DME_.numerator->Fill(mu_pt,mu_eta);
 
  eleptVsLS_.numerator->Fill(ls, ele_pt);
  muptVsLS_.numerator->Fill(ls, mu_pt);
}

void HiggsDiLeptonCrossTriggerMonitor::fillHistoPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins");
  pset.add<double>( "xmin" );
  pset.add<double>( "xmax" );
}

void HiggsDiLeptonCrossTriggerMonitor::fillHistoLSPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins", 2500);
}

void HiggsDiLeptonCrossTriggerMonitor::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<std::string>  ( "FolderName", "HLT/Higgs/DiLepton/" );

  desc.add<edm::InputTag>( "electrons",edm::InputTag("gedGsfElectrons") );
  desc.add<edm::InputTag>( "muons",    edm::InputTag("muons") );
  desc.add<edm::InputTag>( "vertices",    edm::InputTag("offlinePrimaryVertices") );
  desc.add<std::string>("eleSelection", "pt > 7 && abs(eta) < 2.5");
  desc.add<std::string>("muoSelection", "pt > 5 &&  abs(eta) < 2.4 && (isGlobalMuon || (isTrackerMuon && numberOfMatches>0)) && muonBestTrackType != 2 ");
  desc.add<std::string>("vertexSelection", "!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2");
  desc.add<int>("nelectrons", 0);
  desc.add<int>("nmuons",     0);
  desc.add<int>("leptonPDG",     0);
  desc.add<double>("leptondxycut",    0.5);
  desc.add<double>("leptondzcut",     1.);

  edm::ParameterSetDescription genericTriggerEventPSet;
  genericTriggerEventPSet.add<bool>("andOr");
  genericTriggerEventPSet.add<edm::InputTag>("dcsInputTag", edm::InputTag("scalersRawToDigi") );
  genericTriggerEventPSet.add<std::vector<int> >("dcsPartitions",{});
  genericTriggerEventPSet.add<bool>("andOrDcs", false);
  genericTriggerEventPSet.add<bool>("errorReplyDcs", true);
  genericTriggerEventPSet.add<std::string>("dbLabel","");
  genericTriggerEventPSet.add<bool>("andOrHlt", true);
  genericTriggerEventPSet.add<edm::InputTag>("hltInputTag", edm::InputTag("TriggerResults::HLT") );
  genericTriggerEventPSet.add<std::vector<std::string> >("hltPaths",{});
  genericTriggerEventPSet.add<std::string>("hltDBKey","");
  genericTriggerEventPSet.add<bool>("errorReplyHlt",false);
  genericTriggerEventPSet.add<unsigned int>("verbosityLevel",1);

  desc.add<edm::ParameterSetDescription>("numGenericTriggerEventPSet", genericTriggerEventPSet);
  desc.add<edm::ParameterSetDescription>("denGenericTriggerEventPSet", genericTriggerEventPSet);

  edm::ParameterSetDescription histoPSet;
  edm::ParameterSetDescription lepPSet;
  edm::ParameterSetDescription phiPSet;
  edm::ParameterSetDescription etaPSet;
  fillHistoPSetDescription(lepPSet);
  fillHistoPSetDescription(phiPSet);
  fillHistoPSetDescription(etaPSet);
  histoPSet.add<edm::ParameterSetDescription>("lepPSet", lepPSet);
  histoPSet.add<edm::ParameterSetDescription>("phiPSet", phiPSet);
  histoPSet.add<edm::ParameterSetDescription>("etaPSet", etaPSet);
  std::vector<double> bins = {0.,20.,40.,60.,80.,90.,100.,110.,120.,130.,140.,150.,160.,170.,180.,190.,200.,220.,240.,260.,280.,300.,350.,400.,450.,1000.};
  histoPSet.add<std::vector<double> >("lepPtBinning", bins);

  edm::ParameterSetDescription lsPSet;
  fillHistoLSPSetDescription(lsPSet);
  histoPSet.add<edm::ParameterSetDescription>("lsPSet", lsPSet);

  desc.add<edm::ParameterSetDescription>("histoPSet",histoPSet);

  descriptions.add("higgsDiLeptonCrossTriggerMonitoring", desc);
}

// Define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HiggsDiLeptonCrossTriggerMonitor);
