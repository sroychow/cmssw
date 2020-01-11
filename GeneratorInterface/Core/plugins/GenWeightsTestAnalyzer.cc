// -*- C++ -*-
//
// Package:    GenAnalysis/GenWeightsTestAnalyzer
// Class:      GenWeightsTestAnalyzer
//
/**\class GenWeightsTestAnalyzer GenWeightsTestAnalyzer.cc GenAnalysis/GenWeightsTestAnalyzer/plugins/GenWeightsTestAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andreas Artur Eugen Albert
//         Created:  Fri, 21 Jul 2017 11:42:52 GMT
//
//


// system include files
#include <memory>
#include <utility>      // std::pair
// user include files
#include <math.h>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightProduct.h"

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"

#include "Math/VectorUtil.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TTree.h>
#include <Math/GenVector/Boost.h>
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class GenWeightsTestAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchLuminosityBlocks>  {
   public:
      explicit GenWeightsTestAnalyzer(const edm::ParameterSet&);
      ~GenWeightsTestAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void beginLuminosityBlock(edm::LuminosityBlock const& iLumi, edm::EventSetup const&) override;
      virtual void endLuminosityBlock(edm::LuminosityBlock const& iLumi, edm::EventSetup const&) override {}
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      void setup_variables(const edm::Event& iEvent);
      std::vector<double> setup_weights(const edm::Event& iEvent);

      // ----------member data ---------------------------
      std::string tag_;
      bool isMiniaod_;
      std::vector<int> scaleWeightsOrder_;
      int scaleWeightsIndex_;
      int pdfWeightsIndex_;

      edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
      edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
      edm::EDGetTokenT<LHEEventProduct> LHEToken_;
      edm::EDGetTokenT<GenEventInfoProduct> GenToken_;
      edm::EDGetTokenT<GenWeightProduct> lheWeightToken_;
      edm::EDGetTokenT<GenWeightProduct> genWeightToken_;
      edm::EDGetTokenT<GenWeightInfoProduct> lheWeightInfoToken_;
      edm::Service<TFileService> fileservice;

      std::map<TString, TH2D*> histograms2d;
      std::map<TString, TH1D*> histograms1d;

      TTree * tree;
      std::map<TString, double> variables;

      TNamed * filelist;

      TH1D * h_count_event;
      TH1D * h_count_sumw;

      const unsigned int KEEP_LHAPDFID_ = 23000;
      int keepPdfSetIndex_ = 0;
      int keepPdfIndexInSet_ = -1;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
GenWeightsTestAnalyzer::GenWeightsTestAnalyzer(const edm::ParameterSet& iConfig) :
  tag_(iConfig.getParameter<std::string>("tag")),
  isMiniaod_(iConfig.getParameter<bool>("miniaod")),
  scaleWeightsIndex_(-1),
  pdfWeightsIndex_(-1),
  genParticleToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticleSrc"))),
  genJetToken_(consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJetSrc"))),
  LHEToken_(consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("LHESrc"))),
  GenToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("GenSrc"))),
  lheWeightToken_(consumes<GenWeightProduct>(edm::InputTag("testLheWeights"))),
  genWeightToken_(consumes<GenWeightProduct>(edm::InputTag("testGenWeights"))),
  lheWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InLumi>(edm::InputTag("testLheWeights")))

{
   //now do what ever initialization is needed
   //usesResource("TFileService");
   TFileDirectory subdirectory = fileservice->mkdir( tag_ );

   h_count_event = subdirectory.make<TH1D>("h_count_event",     "h_count_event;Dummy;Events",        1,0,1);
   h_count_sumw = subdirectory.make<TH1D>("h_count_sumw",     "h_count_sumw;Weight index;Events",        1,0,1);

   int nweights = 1200;
   histograms2d["pt_jet1"]     = subdirectory.make<TH2D>("pt_jet1",     "Leading jet pt;pt_{jet};weight;Events",200,0,1000, nweights, -0.5, nweights-0.5);
   histograms2d["pt_jet2"]     = subdirectory.make<TH2D>("pt_jet2",     "Trailing jet pt;pt_{jet};weight;Events",200,0,1000, nweights, -0.5, nweights-0.5);
   histograms2d["pt_jet3"]     = subdirectory.make<TH2D>("pt_jet3",     "3rd jet pt;pt_{jet};weight;Events",200,0,1000, nweights, -0.5, nweights-0.5);
   histograms2d["pt_jet4"]     = subdirectory.make<TH2D>("pt_jet4",     "4th jet pt;pt_{jet};weight;Events",200,0,1000, nweights, -0.5, nweights-0.5);
   histograms2d["ht"]          = subdirectory.make<TH2D>("ht",          "HT;HT (GeV);weight;Events",200,0,2000, nweights, -0.5, nweights-0.5);
   histograms2d["dilepton_pt"] = subdirectory.make<TH2D>("dilepton_pt", "Dilepton p_{T}; p_{T} (GeV);weight;Events",200,0,2000, nweights, -0.5, nweights-0.5);
   histograms2d["n_leptons"]   = subdirectory.make<TH2D>("n_leptons",   "number of leptons;n_lep;weight;Events",10,-0.5,9.5, nweights, -0.5, nweights-0.5);

   tree = subdirectory.make<TTree>("events","events");
   for(auto const entry : histograms1d){
      tree->Branch(entry.first, &variables[entry.first],(entry.first+"/D").Data());
   }
   filelist = subdirectory.make<TNamed>(tag_,tag_);
}


GenWeightsTestAnalyzer::~GenWeightsTestAnalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//
bool compare_pt(reco::GenParticle p1, reco::GenParticle p2){
   return p2.pt() < p1.pt();
}
std::vector<reco::GenParticle> select_leptons(const std::vector<reco::GenParticle> genParticles) {
   std::vector<reco::GenParticle> leptons;
   for (auto const p : genParticles){
      unsigned const int id = abs(p.pdgId());
      if(p.numberOfDaughters() > 0) continue;
      else if(p.pt()<20) continue;
      else if(not ((id==11)||(id==13))  ) continue;
      else leptons.push_back(p);
   }
   std::sort(leptons.begin(), leptons.end(), compare_pt );
   return leptons;
}
std::vector<reco::GenParticle> select_neutrinos(const std::vector<reco::GenParticle> genParticles) {
   std::vector<reco::GenParticle> neutrinos;
   for (auto const p : genParticles){
      unsigned const int id = abs(p.pdgId());
      if(p.pt() < 25) continue;
      else if(p.numberOfDaughters() > 0) continue;
      else if(not ((id==12)||(id==14)||(id==16)) ) continue;
      else neutrinos.push_back(p);
   }
   std::sort(neutrinos.begin(), neutrinos.end(), compare_pt );
   return neutrinos;
}

std::vector<reco::GenJet> select_jets(const std::vector<reco::GenJet> genJets) {
   std::vector<reco::GenJet> jets;
   for (auto const p : genJets){
      if(fabs(p.pt()) < 10) continue;
      else jets.push_back(p);
   }
   std::sort(jets.begin(), jets.end(), compare_pt );
   return jets;
}

std::pair<reco::GenParticle,reco::GenParticle> find_z_candidate(std::vector<reco::GenParticle> & genParticles) {
   double delta_mass_min = 9999;
   double m_z = 91;
   reco::GenParticle p1,p2;
   for( unsigned int i = 0; i < genParticles.size(); i++ ) {
      for( unsigned int j = i+1; j < genParticles.size(); j++ ) {
         auto candidate_p4 = genParticles.at(i).p4() + genParticles.at(j).p4();
         double dm = fabs(candidate_p4.mass()-m_z);
         if(dm < delta_mass_min ) {
            delta_mass_min = dm;
            p1 = genParticles.at(i);
            p2 = genParticles.at(j);
         }
      }
   }
   return std::make_pair(p1,p2);
}

std::vector<reco::GenJet> clean_jets(const std::vector<reco::GenJet> genJets, const std::vector<reco::GenParticle> leptons){
   std::vector<reco::GenJet> cleaned_jets;
   cleaned_jets.reserve(genJets.size());
   for( auto const jet : genJets ) {
      double delta_r_min = 999;
      double eta = jet.eta();
      double phi = jet.phi();
      for( auto const lep : leptons ) {
         double delta_r = pow((pow(lep.eta() - eta,2) + pow(lep.phi() - phi,2)),0.5);
         if (delta_r < delta_r_min){
            delta_r_min = delta_r;
         }
      }
      if(delta_r_min < 0.4) {
         continue;
      } else {
         cleaned_jets.push_back(jet);
      }
   }
   return cleaned_jets;
}
void print_variables(std::map<TString,double> variables) {

}
double delta_phi(reco::Candidate::LorentzVector const & p1,reco::Candidate::LorentzVector const & p2) {
   double dphi_raw = fabs(p1.phi() - p2.phi());
   if(dphi_raw > M_PI){
      return 2*M_PI - dphi_raw;
   } else {
      return dphi_raw;
   }
}
//// Parse generator history of particle and look for
bool has_v_in_history(reco::GenParticle const & part){
   size_t nmothers = part.numberOfMothers();
   if(nmothers == 0) {
      return false;
   } else {
      for( size_t im = 0; im < nmothers; im++ ){
         auto const & mother = *(part.motherRef(im));

         if(mother.pdgId() == 23 or mother.pdgId() == 24) {
            return true;
         } else if( has_v_in_history(*(part.motherRef(im))) ) {
            return true;
         }
      }
      return false;
   }
}

void GenWeightsTestAnalyzer::setup_variables(const edm::Event& iEvent) {
   using namespace edm;

   //// Initialize with dummy values
   for ( auto pair : variables ) {
      variables[pair.first] = -9999;
   }

   Handle<std::vector<reco::GenParticle>> genParticleHandle;
   iEvent.getByToken(genParticleToken_, genParticleHandle);
   const std::vector<reco::GenParticle> * genParticles = genParticleHandle.product();

   Handle<std::vector<reco::GenJet>> genJetHandle;
   iEvent.getByToken(genJetToken_, genJetHandle);
   const std::vector<reco::GenJet> * genJets = genJetHandle.product();

   edm::Handle<LHEEventProduct> lheHandle;
   iEvent.getByLabel("externalLHEProducer", lheHandle);
   const LHEEventProduct * lhe = lheHandle.product();




   variables["weight"] = lhe->originalXWGTUP();

   //~ //// Leptons
   std::vector<reco::GenParticle> leptons = select_leptons(*genParticles);
   variables["n_leptons"] = leptons.size();

   if(leptons.size() >= 2) {
      auto z_cand = find_z_candidate(leptons);
      variables["dilepton_pt"] = (z_cand.first.p4() + z_cand.second.p4()).pt();
   }
   else
      variables["dilepton_pt"] = 0;

   //// Jets
   std::vector<reco::GenJet> jets = clean_jets(select_jets(*genJets),leptons);
   int njets = jets.size();
   variables["n_jets"] = njets;
   variables["pt_jet1"]   = njets > 0 ? jets.at(0).pt() : -999;
   variables["pt_jet2"]   = njets > 1 ? jets.at(1).pt() : -999;
   variables["pt_jet3"]   = njets > 2 ? jets.at(2).pt() : -999;
   variables["pt_jet4"]   = njets > 3 ? jets.at(3).pt() : -999;

   double ht = 0;
   for(auto const & j : jets){
      ht += j.pt();
   }
   variables["ht"] = ht;

}
std::vector<double> GenWeightsTestAnalyzer::setup_weights(const edm::Event& iEvent) {
   edm::Handle<GenWeightProduct> lheWeightHandle;
   edm::Handle<GenWeightProduct> genWeightHandle;
   iEvent.getByToken(lheWeightToken_, lheWeightHandle);
   iEvent.getByToken(genWeightToken_, genWeightHandle);
   const GenWeightProduct * lheWeightProduct = lheWeightHandle.product();
   const GenWeightProduct * genWeightProduct = genWeightHandle.product();
   WeightsContainer lheWeights = lheWeightProduct->weights();
   WeightsContainer genWeights = genWeightProduct->weights();

   auto scaleWeights = scaleWeightsIndex_ >= 0 ? lheWeights.at(scaleWeightsIndex_) : std::vector<double>();
   auto pdfWeights = pdfWeightsIndex_ > 0 ? lheWeights.at(pdfWeightsIndex_) : std::vector<double>();
   std::vector<double> keepWeights;

   for(auto i : scaleWeightsOrder_){
      keepWeights.push_back(scaleWeights.at(i));
   }

   for(auto& weight : pdfWeights){
      keepWeights.push_back(weight);
   }

   if (genWeights.size() > 0) {
       for(auto& weight : genWeights.front()){
           keepWeights.push_back(weight);
       }
   }

   return keepWeights;
}


// ------------ method called for each event  ------------
void
GenWeightsTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   setup_variables(iEvent);
   tree->Fill();
   auto weights = setup_weights(iEvent);
   h_count_event->Fill(0.5);
   h_count_sumw->Fill(0.5,weights.at(4));

   for( auto pair : histograms2d ) {
      for(uint i=0; i<weights.size(); i++) {
         double value = variables.at(pair.first);
         if(value > -9998){
               pair.second->Fill(value,i,weights.at(i));
         }
      }
   }
}


// ------------ method called once each job just before starting event loop  ------------
void
GenWeightsTestAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
GenWeightsTestAnalyzer::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenWeightsTestAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void 
GenWeightsTestAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const& iLumi, edm::EventSetup const& es) {
    //edm::Handle<GenWeightInfoProduct> lheWeightsInfoHandle;
    //edm::Handle<GenRunInfoProduct> lheWeightsInfoHandle;
    //run.getByLabel("generator", lheWeightsInfoHandle);
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    iLumi.getByToken(lheWeightInfoToken_, lheWeightInfoHandle);

    // Should add a search by name function
    auto allScaleWeights = lheWeightInfoHandle->weightGroupIndicesByType(gen::WeightType::kScaleWeights);
    if (allScaleWeights.size() > 0)
        scaleWeightsIndex_ = allScaleWeights.front();

    auto scaleWeights = static_cast<const gen::ScaleWeightGroupInfo*>(
            lheWeightInfoHandle->orderedWeightGroupInfo(scaleWeightsIndex_));
    // nano ordering of mur=0.5 muf=0.5 ; [1] is mur=0.5 muf=1 ; [2] is mur=0.5 muf=2 ; [3] is mur=1 muf=0.5 ; 
    // [4] is mur=1 muf=1 ; [5] is mur=1 muf=2 ; [6] is mur=2 muf=0.5 ; [7] is mur=2 muf=1 ; [8] is mur=2 muf=2 *
    scaleWeightsOrder_.clear();
    scaleWeightsOrder_.push_back(scaleWeights->muR05muF05Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR05muF1Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR05muF2Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR1muF05Index());
    scaleWeightsOrder_.push_back(scaleWeights->centralIndex());
    scaleWeightsOrder_.push_back(scaleWeights->muR1muF2Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR2muF05Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR2muF1Index());
    scaleWeightsOrder_.push_back(scaleWeights->muR2muF2Index());

    auto pdfGroups = lheWeightInfoHandle->weightGroupsByType(gen::WeightType::kPdfWeights);
    auto ct14Set = std::find_if(pdfGroups.begin(), pdfGroups.end(), 
            [] (gen::WeightGroupInfo* group) { 
                    auto pdfGroup = dynamic_cast<gen::PdfWeightGroupInfo*>(group); 
                    return pdfGroup->containsLhapdfId(1300); 
    });
    if (ct14Set != pdfGroups.end())
        pdfWeightsIndex_ = std::distance(pdfGroups.begin(), ct14Set);
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenWeightsTestAnalyzer);

