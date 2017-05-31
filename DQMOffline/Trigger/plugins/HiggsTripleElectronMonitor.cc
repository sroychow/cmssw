#include "DQMOffline/Trigger/plugins/HiggsTripleElectronMonitor.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQM/TrackingMonitor/interface/GetLumi.h"

#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"
#include<algorithm>

// -----------------------------
//  constructors and destructor
// -----------------------------

HiggsTripleElectronMonitor::HiggsTripleElectronMonitor( const edm::ParameterSet& iConfig ) : 
  folderName_             ( iConfig.getParameter<std::string>("FolderName") )
  , eleToken_             ( mayConsume<edm::View<reco::GsfElectron> > (iConfig.getParameter<edm::InputTag>("electrons") ) )   
  , vtxToken_             ( mayConsume<reco::VertexCollection>       (iConfig.getParameter<edm::InputTag>("vertices")     ) )   
  , met_variable_binning_ ( iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<std::vector<double> >("lepPtBinning") )
  , met_binning_          ( getHistoPSet   (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("lepPSet")    ) )
  , ls_binning_           ( getHistoLSPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("lsPSet")     ) )
  , phi_binning_          ( getHistoPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("phiPSet")     ) )
  , eta_binning_          ( getHistoPSet (iConfig.getParameter<edm::ParameterSet>("histoPSet").getParameter<edm::ParameterSet>   ("etaPSet")     ) )
  , num_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("numGenericTriggerEventPSet"),consumesCollector(), *this))
  , den_genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig.getParameter<edm::ParameterSet>("denGenericTriggerEventPSet"),consumesCollector(), *this))
  , eleSelection_ ( iConfig.getParameter<std::string>("eleSelection") )
  , vtxSelection_ ( iConfig.getParameter<std::string>("vertexSelection") )
  , nelectrons_ ( iConfig.getParameter<int>("nelectrons" ) )
  , dxycut_     ( iConfig.getParameter<double>("leptondxycut" )     )
  , dzcut_      ( iConfig.getParameter<double>("leptondzcut" )     )
{

  l1ptME_.numerator   = nullptr;
  l1ptME_.denominator = nullptr;
  l1ptME_variableBinning_.numerator   = nullptr;
  l1ptME_variableBinning_.denominator = nullptr;
  l1ptVsLS_.numerator   = nullptr;
  l1ptVsLS_.denominator = nullptr;
  l1PhiME_.numerator   = nullptr;
  l1PhiME_.denominator = nullptr;
  l1EtaME_.numerator   = nullptr;
  l1EtaME_.denominator = nullptr;
  l1PtEta2DME_.numerator   = nullptr;
  l1PtEta2DME_.denominator = nullptr;
  
  l2ptME_.numerator   = nullptr;
  l2ptME_.denominator = nullptr;
  l2ptME_variableBinning_.numerator   = nullptr;
  l2ptME_variableBinning_.denominator = nullptr;
  l2ptVsLS_.numerator   = nullptr;
  l2ptVsLS_.denominator = nullptr;
  l2PhiME_.numerator   = nullptr;
  l2PhiME_.denominator = nullptr;
  l2EtaME_.numerator   = nullptr;
  l2EtaME_.denominator = nullptr;
  l2PtEta2DME_.numerator   = nullptr;
  l2PtEta2DME_.denominator = nullptr;

  l3ptME_.numerator   = nullptr;
  l3ptME_.denominator = nullptr;
  l3ptME_variableBinning_.numerator   = nullptr;
  l3ptME_variableBinning_.denominator = nullptr;
  l3ptVsLS_.numerator   = nullptr;
  l3ptVsLS_.denominator = nullptr;
  l3PhiME_.numerator   = nullptr;
  l3PhiME_.denominator = nullptr;
  l3EtaME_.numerator   = nullptr;
  l3EtaME_.denominator = nullptr;
  l3PtEta2DME_.numerator   = nullptr;
  l3PtEta2DME_.denominator = nullptr;
}

HiggsTripleElectronMonitor::~HiggsTripleElectronMonitor()
{
  if (num_genTriggerEventFlag_) delete num_genTriggerEventFlag_;
  if (den_genTriggerEventFlag_) delete den_genTriggerEventFlag_;
}

MEbinning HiggsTripleElectronMonitor::getHistoPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      pset.getParameter<double>("xmin"),
      pset.getParameter<double>("xmax"),
      };
}

MEbinning HiggsTripleElectronMonitor::getHistoLSPSet(edm::ParameterSet pset)
{
  return MEbinning{
    pset.getParameter<int32_t>("nbins"),
      0.,
      double(pset.getParameter<int32_t>("nbins"))
      };
}

void HiggsTripleElectronMonitor::setMETitle(LEPME& me, std::string titleX, std::string titleY)
{
  me.numerator->setAxisTitle(titleX,1);
  me.numerator->setAxisTitle(titleY,2);
  me.denominator->setAxisTitle(titleX,1);
  me.denominator->setAxisTitle(titleY,2);
}

void HiggsTripleElectronMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbins, double min, double max)
{
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, min, max);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, min, max);
}
void HiggsTripleElectronMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binning)
{
  int nbins = binning.size()-1;
  std::vector<float> fbinning(binning.begin(),binning.end());
  float* arr = &fbinning[0];
  me.numerator   = ibooker.book1D(histname+"_numerator",   histtitle+" (numerator)",   nbins, arr);
  me.denominator = ibooker.book1D(histname+"_denominator", histtitle+" (denominator)", nbins, arr);
}
void HiggsTripleElectronMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, double ymin, double ymax)
{
  me.numerator   = ibooker.bookProfile(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, ymin, ymax);
  me.denominator = ibooker.bookProfile(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, ymin, ymax);
}
void HiggsTripleElectronMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, int nbinsX, double xmin, double xmax, int nbinsY, double ymin, double ymax)
{
  me.numerator   = ibooker.book2D(histname+"_numerator",   histtitle+" (numerator)",   nbinsX, xmin, xmax, nbinsY, ymin, ymax);
  me.denominator = ibooker.book2D(histname+"_denominator", histtitle+" (denominator)", nbinsX, xmin, xmax, nbinsY, ymin, ymax);
}
void HiggsTripleElectronMonitor::bookME(DQMStore::IBooker &ibooker, LEPME& me, const std::string& histname, const std::string& histtitle, const std::vector<double>& binningX, const std::vector<double>& binningY)
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

void HiggsTripleElectronMonitor::bookHistograms(DQMStore::IBooker     & ibooker,
				 edm::Run const        & iRun,
				 edm::EventSetup const & iSetup) 
{  
  std::string histname, histtitle;

  std::string currentFolder = folderName_ ;
  ibooker.setCurrentFolder(currentFolder.c_str());
  //same flavour lead and sub-lead lepton

  histname = "lep1Pt"; histtitle = "Leading electron p_T";
  bookME(ibooker,l1ptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l1ptME_,"p_T [GeV]","events / [GeV]");

  histname = "lep1Pt_variable"; histtitle = "Leading electron p_{T}";
  bookME(ibooker,l1ptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(l1ptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "lep1PtVsLS"; histtitle = "Laading electron p_T vs LS";
  bookME(ibooker,l1ptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l1ptVsLS_,"LS","p_T [GeV]");
  
  histname = "lep1Phi"; histtitle = "Leading electron #phi";
  bookME(ibooker,l1PhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(l1PhiME_,"#phi","events / 0.1 rad");

  histname = "lep1Eta"; histtitle = "Leading electron #eta";
  bookME(ibooker,l1EtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l1EtaME_,"#eta","events / 0.1 rad");

  histname = "lep1PtEta"; histtitle = "Leading electron pt-#eta";
  bookME(ibooker,l1PtEta2DME_,histname,histtitle, met_binning_.nbins,met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l1PtEta2DME_, "p_T", "#eta");

  //Subleading
  histname = "lep2Pt"; histtitle = "Sub-leading electron p_T";
  bookME(ibooker,l2ptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l2ptME_,"p_T [GeV]","events / [GeV]");

  histname = "lep2Pt_variable"; histtitle = "Sub-leading electron p_T";
  bookME(ibooker,l2ptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(l2ptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "lep2PtVsLS"; histtitle = "Sub-leading electron p_T vs LS";
  bookME(ibooker,l2ptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l2ptVsLS_,"LS","p_T [GeV]");
  
  histname = "lep2Phi"; histtitle = "Sub-leading electron phi";
  bookME(ibooker,l2PhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(l2PhiME_,"#phi","events / 0.1 rad");

  histname = "lep2Eta"; histtitle = "Sub-leading electron #eta";
  bookME(ibooker,l2EtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l2EtaME_,"#eta","events");

  histname = "lep2PtEta"; histtitle = "Sub-leading electron pt-#eta";
  bookME(ibooker,l2PtEta2DME_,histname,histtitle, met_binning_.nbins,met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l2PtEta2DME_, "p_T", "#eta");

  //histograms for lepton of different flavour  
  histname = "lep3Pt"; histtitle = "Trailing electron p_T";
  bookME(ibooker,l3ptME_,histname,histtitle,met_binning_.nbins,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l3ptME_,"p_T [GeV]","events / [GeV]");

  histname = "lep3Pt_variable"; histtitle = "Trailing electron p_{T}";
  bookME(ibooker,l3ptME_variableBinning_,histname,histtitle,met_variable_binning_);
  setMETitle(l3ptME_variableBinning_,"p_T [GeV]","events / [GeV]");
  
  histname = "lep3PtVsLS"; histtitle = "Trailing electron p_T vs LS";
  bookME(ibooker,l3ptVsLS_,histname,histtitle,ls_binning_.nbins, ls_binning_.xmin, ls_binning_.xmax,met_binning_.xmin, met_binning_.xmax);
  setMETitle(l3ptVsLS_,"LS","p_T [GeV]");
  
  histname = "lep3Phi"; histtitle = "Trailing electron phi";
  bookME(ibooker,l3PhiME_,histname,histtitle, phi_binning_.nbins, phi_binning_.xmin, phi_binning_.xmax);
  setMETitle(l3PhiME_,"#phi","events / 0.1 rad");

  histname = "lep3Eta"; histtitle = "Trailing electron #eta";
  bookME(ibooker,l3EtaME_,histname,histtitle, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l3EtaME_,"#eta","events");

  histname = "lep3PtEta"; histtitle = "Trailing electron pt-#eta";
  bookME(ibooker,l3PtEta2DME_,histname,histtitle, met_binning_.nbins,met_binning_.xmin, met_binning_.xmax, eta_binning_.nbins, eta_binning_.xmin, eta_binning_.xmax);
  setMETitle(l3PtEta2DME_, "p_T", "#eta");

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
void HiggsTripleElectronMonitor::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup)  {

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

  //check the no. of electrons and muons after selection is applied
  if ( int(electrons.size()) < nelectrons_ ) return;

  std::sort(electrons.begin(), electrons.end(), PtComparator<reco::GsfElectron>());  

  float l1_pt = -999.;
  float l1_phi = -999.;
  float l1_eta = -999.;
  float l2_pt = -999.;
  float l2_phi = -999.;
  float l2_eta = -999.;
  float l3_pt = -999.;
  float l3_phi = -999.;
  float l3_eta = -999.;

  l1_pt = electrons[0].pt();
  l1_phi = electrons[0].phi();
  l1_eta = electrons[0].eta();
  l2_pt = electrons[1].pt();
  l2_phi = electrons[1].phi();
  l2_eta = electrons[1].eta();
  l3_pt = electrons[2].pt();
  l3_phi = electrons[2].phi();
  l3_eta = electrons[2].eta();

  // filling histograms (denominator)  
  l1ptME_.denominator->Fill(l1_pt);
  l1ptME_variableBinning_.denominator->Fill(l1_pt);
  l1PhiME_.denominator->Fill(l1_phi);
  l1EtaME_.denominator->Fill(l1_eta);
  l1PtEta2DME_.denominator->Fill(l1_pt,l1_eta);

  l2ptME_.denominator->Fill(l2_pt);
  l2ptME_variableBinning_.denominator->Fill(l2_pt);
  l2PhiME_.denominator->Fill(l2_phi);
  l2EtaME_.denominator->Fill(l2_eta);
  l2PtEta2DME_.denominator->Fill(l2_pt,l2_eta);

  l3ptME_.denominator->Fill(l3_pt);
  l3ptME_variableBinning_.denominator->Fill(l3_pt);
  l3PhiME_.denominator->Fill(l3_phi);
  l3EtaME_.denominator->Fill(l3_eta);
  l3PtEta2DME_.denominator->Fill(l3_pt,l3_eta);


  int ls = iEvent.id().luminosityBlock();
  l1ptVsLS_.denominator->Fill(ls, l1_pt);
  l2ptVsLS_.denominator->Fill(ls, l2_pt);
  l3ptVsLS_.denominator->Fill(ls, l3_pt);
  
  // applying selection for numerator
  if (num_genTriggerEventFlag_->on() && ! num_genTriggerEventFlag_->accept( iEvent, iSetup) ) return;

  // filling histograms (num_genTriggerEventFlag_)  
  l1ptME_.numerator->Fill(l1_pt);
  l1ptME_variableBinning_.numerator->Fill(l1_pt);
  l1ptVsLS_.numerator->Fill(ls, l1_pt);
  l1PhiME_.numerator->Fill(l1_phi);
  l1EtaME_.numerator->Fill(l1_eta);
  l1PtEta2DME_.numerator->Fill(l1_pt,l1_eta);

  l2ptME_.numerator->Fill(l2_pt);
  l2ptME_variableBinning_.numerator->Fill(l2_pt);
  l2ptVsLS_.numerator->Fill(ls, l2_pt);
  l2PhiME_.numerator->Fill(l2_phi);
  l2EtaME_.numerator->Fill(l2_eta);
  l2PtEta2DME_.numerator->Fill(l2_pt,l2_eta);

  l3ptME_.numerator->Fill(l3_pt);
  l3ptME_variableBinning_.numerator->Fill(l3_pt);
  l3ptVsLS_.numerator->Fill(ls, l3_pt);
  l3PhiME_.numerator->Fill(l3_phi);
  l3EtaME_.numerator->Fill(l3_eta);
  l3PtEta2DME_.numerator->Fill(l3_pt,l3_eta);

}

void HiggsTripleElectronMonitor::fillHistoPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins");
  pset.add<double>( "xmin" );
  pset.add<double>( "xmax" );
}

void HiggsTripleElectronMonitor::fillHistoLSPSetDescription(edm::ParameterSetDescription & pset)
{
  pset.add<int>   ( "nbins", 2500);
}

void HiggsTripleElectronMonitor::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription desc;
  desc.add<std::string>  ( "FolderName", "HLT/Higgs/TriLepton/" );

  desc.add<edm::InputTag>( "electrons",edm::InputTag("gedGsfElectrons") );
  desc.add<edm::InputTag>( "vertices",    edm::InputTag("offlinePrimaryVertices") );
  desc.add<std::string>("eleSelection", "pt > 7 && abs(eta) < 2.5");
  desc.add<std::string>("vertexSelection", "!isFake && ndof > 4 && abs(z) <= 24 && position.Rho <= 2");
  desc.add<int>("nelectrons", 0);
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

  descriptions.add("higgsTripleElectronMonitoring", desc);
}

// Define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HiggsTripleElectronMonitor);
