#include "DQMOffline/Trigger/plugins/HiggsTriLeptonCrossTriggerOfflineMonitor.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQM/TrackingMonitor/interface/GetLumi.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"
#include<algorithm>

// -----------------------------
//  constructors and destructor
// -----------------------------

HiggsTriLeptonCrossTriggerOfflineMonitor::HiggsTriLeptonCrossTriggerOfflineMonitor( const edm::ParameterSet& iConfig ) : 
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
  , lepPdg_     ( iConfig.getParameter<int>("leptonPDG" )     )
  , dxycut_     ( iConfig.getParameter<double>("leptondxycut" )     )
  , dzcut_      ( iConfig.getParameter<double>("leptondzcut" )     )
{

  sfl1ptME_.numerator   = nullptr;
  sfl1ptME_.denominator = nullptr;
  sfl1ptME_variableBinning_.numerator   = nullptr;
  sfl1ptME_variableBinning_.denominator = nullptr;
  sfl1ptVsLS_.numerator   = nullptr;
  sfl1ptVsLS_.denominator = nullptr;
  sfl1PhiME_.numerator   = nullptr;
  sfl1PhiME_.denominator = nullptr;
  sfl1EtaME_.numerator   = nullptr;
  sfl1EtaME_.denominator = nullptr;
  sfl1PtEta2DME_.numerator   = nullptr;
  sfl1PtEta2DME_.denominator = nullptr;

  sfl2ptME_.numerator   = nullptr;
  sfl2ptME_.denominator = nullptr;
  sfl2ptME_variableBinning_.numerator   = nullptr;
  sfl2ptME_variableBinning_.denominator = nullptr;
  sfl2ptVsLS_.numerator   = nullptr;
  sfl2ptVsLS_.denominator = nullptr;
  sfl2PhiME_.numerator   = nullptr;
  sfl2PhiME_.denominator = nullptr;
  sfl2EtaME_.numerator   = nullptr;
  sfl2EtaME_.denominator = nullptr;
  sfl2PtEta2DME_.numerator   = nullptr;
  sfl2PtEta2DME_.denominator = nullptr;

  cflptME_.numerator   = nullptr;
  cflptME_.denominator = nullptr;
  cflptME_variableBinning_.numerator   = nullptr;
  cflptME_variableBinning_.denominator = nullptr;
  cflptVsLS_.numerator   = nullptr;
  cflptVsLS_.denominator = nullptr;
  cflPhiME_.numerator   = nullptr;
  cflPhiME_.denominator = nullptr;
  cflEtaME_.numerator   = nullptr;
  cflEtaME_.denominator = nullptr;
  cflPtEta2DME_.numerator   = nullptr;
  cflPtEta2DME_.denominator = nullptr;
  
}

HiggsTriLeptonCrossTriggerOfflineMonitor::~HiggsTriLeptonCrossTriggerOfflineMonitor()
{
  if (num_genTriggerEventFlag_) delete num_genTriggerEventFlag_;
  if (den_genTriggerEventFlag_) delete den_genTriggerEventFlag_;
}

MEbinning HiggsTriLeptonCrossTriggerOfflineMonitor::getHistoPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      pset.getParameter<double>("xmin"),
      pset.getParameter<double>("xmax"),
      };
}

MEbinning HiggsTriLeptonCrossTriggerOfflineMonitor::getHistoLSPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      0.,
      double(pset.getParameter<int32_t>("nbins"))
      };
}

void HiggsTriLeptonCrossTriggerOfflineMonitor::setMETitle(LEPME& me, std::string titleX, std::string titleY)
{
  me.numerator->setAxisTitle(titleX,1);
  me.numerator->setAxisTitle(titleY,2);
  me.denominator->setAxisTitle(titleX,1);
  me.denominator->setAxisTitle(titleY,2);

}

void HiggsTriLeptonCrossTriggerOfflineMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbins, double min, double max)
{
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, min, max);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, min, max);
}
void HiggsTriLeptonCrossTriggerOfflineMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binning)
{
  int nbins = binning.size()-1;
  std::vector<float> fbinning(binning.begin(),binning.end());
  float* arr = &fbinning[0];
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, arr);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, arr);
}
void HiggsTriLeptonCrossTriggerOfflineMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, double ymin, double ymax)
{
  me.numerator   = ibooker.bookProfile(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, ymin, ymax);
  me.denominator = ibooker.bookProfile(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, ymin, ymax);
}
void HiggsTriLeptonCrossTriggerOfflineMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, int nbinsY, double ymin, double ymax)
{
  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, nbinsY, ymin, ymax);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, nbinsY, ymin, ymax);
}
void HiggsTriLeptonCrossTriggerOfflineMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX, const std::vector<double>& binningY)
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

void HiggsTriLeptonCrossTriggerOfflineMonitor::bookHistograms(DQMStore::IBooker     & ibooker,
				 edm::Run const        & iRun,
				 edm::EventSetup const & iSetup) 
{  
  std::string histname, histtitle, sfTag, cfTag;
  if(lepPdg_ == 13 ){
    sfTag = " muon ";
    cfTag= " electron ";
  } else {
    sfTag = " electron ";
    cfTag= " muon ";
  }
  std::string currentFolder = folderName_ ;
  ibooker.setCurrentFolder(currentFolder.c_str());
  //same flavour lead and sub-lead lepton

  histname = "sflep1Pt"; histtitle = "Leading" + sfTag + "p_T";
  bookME(ibooker,sfl1ptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(sfl1ptME_,"p_T [GeV]","events / [GeV]");

  histname = "sflep1Pt_variable"; histtitle = "Leading" + sfTag + " p_{T}";
  bookME(ibooker,sfl1ptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(sfl1ptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "sflep1PtVsLS"; histtitle = "Leading"  + sfTag + " p_T vs LS";
  bookME(ibooker,sfl1ptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(sfl1ptVsLS_,"LS","p_T [GeV]");
  
  histname = "sflep1Phi"; histtitle = "Leading" + sfTag + "#phi";
  bookME(ibooker,sfl1PhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(sfl1PhiME_,"#phi","events / 0.1 rad");

  histname = "sflep1Eta"; histtitle = "Leading" + sfTag + " #eta";
  bookME(ibooker,sfl1EtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(sfl1EtaME_,"#eta","events");

  histname = "sflep1PtEta"; histtitle = "Leading" + sfTag + " pt-#eta";
  bookME(ibooker,sfl1PtEta2DME_,histname,histtitle, met_binning_.nbins, met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(sfl1PtEta2DME_, "p_T", "#eta");
  
  //sub-lead same flavour
  histname = "sflep2Pt"; histtitle = "Trailing" + sfTag + "p_T";
  bookME(ibooker,sfl2ptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(sfl2ptME_,"p_T [GeV]","events / [GeV]");

  histname = "sflep2Pt_variable"; histtitle = "Trailing" + sfTag + " p_{T}";
  bookME(ibooker,sfl2ptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(sfl2ptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "sflep2PtVsLS"; histtitle = "Trailing"  + sfTag + " p_T vs LS";
  bookME(ibooker,sfl2ptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(sfl2ptVsLS_,"LS","p_T [GeV]");
  
  histname = "sflep2Phi"; histtitle = "Trailing" + sfTag + "phi";
  bookME(ibooker,sfl2PhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(sfl2PhiME_,"#phi","events / 0.1 rad");

  histname = "sflep2Eta"; histtitle = "Trailing" + sfTag + "#eta";
  bookME(ibooker,sfl2EtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(sfl2EtaME_,"#eta","events");

  histname = "sflep2PtEta"; histtitle = "Trailing " + sfTag + " pt-#eta";
  bookME(ibooker,sfl2PtEta2DME_,histname,histtitle, met_binning_.nbins, met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(sfl2PtEta2DME_, "p_T", "#eta");

  //histograms for lepton of different flavour  
  histname = "cflepPt"; histtitle = cfTag + "p_T";
  bookME(ibooker,cflptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(cflptME_,"p_T [GeV]","events / [GeV]");

  histname = "cflepPt_variable"; histtitle = cfTag + " p_{T}";
  bookME(ibooker,cflptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(cflptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "cflepPtVsLS"; histtitle = cfTag + " p_T vs LS";
  bookME(ibooker,cflptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(cflptVsLS_,"LS","p_T [GeV]");
  
  histname = "cflepPhi"; histtitle = cfTag + "phi";
  bookME(ibooker,cflPhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(cflPhiME_,"#phi","events / 0.1 rad");

  histname = "cflepEta"; histtitle = cfTag + "#eta";
  bookME(ibooker,cflEtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(cflEtaME_,"#eta","events");

  histname = "cflepPtEta"; histtitle = cfTag + " pt-#eta";
  bookME(ibooker,cflPtEta2DME_,histname,histtitle, met_binning_.nbins, met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(cflPtEta2DME_,"p_T", "#eta");

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
void HiggsTriLeptonCrossTriggerOfflineMonitor::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup)  {

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

  std::sort(electrons.begin(), electrons.end(), PtComparator<reco::GsfElectron>());  
  std::sort(muons.begin(), muons.end(), PtComparator<reco::Muon>());  

  float sfL1_pt = -999.;
  float sfL1_phi = -999.;
  float sfL1_eta = -999.;
  float sfL2_pt = -999.;
  float sfL2_phi = -999.;
  float sfL2_eta = -999.;
  float cfL_pt = -999.;
  float cfL_phi = -999.;
  float cfL_eta = -999.;
  if(lepPdg_ == 13) {
    sfL1_pt = muons[0].pt();
    sfL1_phi = muons[0].phi();
    sfL1_eta = muons[0].eta();
    sfL2_pt = muons[1].pt();
    sfL2_phi = muons[1].phi();
    sfL2_phi = muons[1].eta();
    cfL_pt = electrons[0].pt();
    cfL_phi = electrons[0].phi();
    cfL_phi = electrons[0].eta();
  } else if (lepPdg_ == 11) {
    sfL1_pt = electrons[0].pt();
    sfL1_phi = electrons[0].phi();
    sfL1_phi = electrons[0].eta();
    sfL2_pt = electrons[1].pt();
    sfL2_phi = electrons[1].phi();
    sfL2_phi = electrons[1].eta();
    cfL_pt = muons[0].pt();
    cfL_phi = muons[0].phi();
    cfL_phi = muons[0].eta();
  }

  // filling histograms (denominator)  
  sfl1ptME_.denominator->Fill(sfL1_pt);
  sfl1ptME_variableBinning_.denominator->Fill(sfL1_pt);
  sfl1PhiME_.denominator->Fill(sfL1_phi);
  sfl1EtaME_.denominator->Fill(sfL1_eta);
  sfl1PtEta2DME_.denominator->Fill(sfL1_pt, sfL1_eta);

  sfl2ptME_.denominator->Fill(sfL2_pt);
  sfl2ptME_variableBinning_.denominator->Fill(sfL2_pt);
  sfl2PhiME_.denominator->Fill(sfL2_phi);
  sfl2EtaME_.denominator->Fill(sfL2_eta);
  sfl2PtEta2DME_.denominator->Fill(sfL2_pt, sfL2_eta);

  cflptME_.denominator->Fill(cfL_pt);
  cflptME_variableBinning_.denominator->Fill(cfL_pt);
  cflPhiME_.denominator->Fill(cfL_phi);
  cflEtaME_.denominator->Fill(cfL_eta);
  cflPtEta2DME_.denominator->Fill(cfL_pt, cfL_eta);

  int ls = iEvent.id().luminosityBlock();
  sfl1ptVsLS_.denominator->Fill(ls, sfL1_pt);
  sfl2ptVsLS_.denominator->Fill(ls, sfL2_pt);
  cflptVsLS_.denominator->Fill(ls, cfL_pt);
  
  // applying selection for numerator
  if (num_genTriggerEventFlag_->on() && ! num_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;

  // filling histograms (num_genTriggerEventFlag_)  
  sfl1ptME_.numerator->Fill(sfL1_pt);
  sfl1ptME_variableBinning_.numerator->Fill(sfL1_pt);
  sfl1ptVsLS_.numerator->Fill(ls, sfL1_pt);
  sfl1PhiME_.numerator->Fill(sfL1_phi);
  sfl1EtaME_.numerator->Fill(sfL1_eta);
  sfl1PtEta2DME_.numerator->Fill(sfL1_pt, sfL1_eta);

  sfl2ptME_.numerator->Fill(sfL2_pt);
  sfl2ptME_variableBinning_.numerator->Fill(sfL2_pt);
  sfl2ptVsLS_.numerator->Fill(ls, sfL2_pt);
  sfl2PhiME_.numerator->Fill(sfL2_phi);
  sfl2EtaME_.numerator->Fill(sfL2_eta);
  sfl2PtEta2DME_.numerator->Fill(sfL2_pt, sfL2_eta);

  cflptME_.numerator->Fill(cfL_pt);
  cflptME_variableBinning_.numerator->Fill(cfL_pt);
  cflptVsLS_.numerator->Fill(ls, cfL_pt);
  cflPhiME_.numerator->Fill(cfL_phi);
  cflEtaME_.numerator->Fill(cfL_eta);
  cflPtEta2DME_.numerator->Fill(cfL_pt, cfL_eta);
}

void HiggsTriLeptonCrossTriggerOfflineMonitor::fillHistoPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins");
  pset.add<double>( "xmin" );
  pset.add<double>( "xmax" );
}

void HiggsTriLeptonCrossTriggerOfflineMonitor::fillHistoLSPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins", 2500);
}

void HiggsTriLeptonCrossTriggerOfflineMonitor::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<std::string>  ( "FolderName", "HLT/Higgs/TriLepton/" );

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

  descriptions.add("higgsTriLeptonCrossTriggerMonitoring", desc);
}

// Define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HiggsTriLeptonCrossTriggerOfflineMonitor);
