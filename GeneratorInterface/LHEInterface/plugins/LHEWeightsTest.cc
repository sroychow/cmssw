// -*- C++ -*-
//
// Package:    GenAnalysis/MergingAnalyzer
// Class:      MergingAnalyzer
//
/**\class MergingAnalyzer MergingAnalyzer.cc GenAnalysis/MergingAnalyzer/plugins/MergingAnalyzer.cc

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

class MergingAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MergingAnalyzer(const edm::ParameterSet&);
      ~MergingAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      void setup_variables(const edm::Event& iEvent);

      // ----------member data ---------------------------
      std::string tag_;
      bool isMiniaod_;
      std::vector<int> scaleWeightOrder_;

      edm::EDGetTokenT<reco::GenParticleCollection> genParticleToken_;
      edm::EDGetTokenT<reco::GenJetCollection> genJetToken_;
      edm::EDGetTokenT<LHEEventProduct> LHEToken_;
      edm::EDGetTokenT<GenEventInfoProduct> GenToken_;
      edm::Service<TFileService> fileservice;

      std::map<TString, TH2D*> histograms2d;
      std::map<TString, TH1D*> histograms1d;

      TTree * tree;
      std::map<TString, double> variables;

      TNamed * filelist;

      TH1D * h_count_event;
      TH1D * h_count_sumw;
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
MergingAnalyzer::MergingAnalyzer(const edm::ParameterSet& iConfig) :
  tag_(iConfig.getParameter<std::string>("tag")),
  isMiniaod_(iConfig.getParameter<bool>("miniaod")),
  genParticleToken_(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genParticleSrc"))),
  genJetToken_(consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genJetSrc"))),
  LHEToken_(consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("LHESrc"))),
  GenToken_(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("GenSrc")))


{
   //now do what ever initialization is needed
   usesResource("TFileService");
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

   // histograms2d["njet"] = subdirectory.make<TH2D>("njet_over_pt",     "pt threshold;njet;Events",10,-5,95,10,-0.5,9.5);

   tree = subdirectory.make<TTree>("events","events");
   for(auto const entry : histograms1d){
      tree->Branch(entry.first, &variables[entry.first],(entry.first+"/D").Data());
   }
   filelist = subdirectory.make<TNamed>(tag_,tag_);
}


MergingAnalyzer::~MergingAnalyzer()
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
reco::Candidate const * find_first_mother_with_different_id( reco::Candidate const * daughter ){
   reco::Candidate const * mother = daughter->mother(0);
   if(abs(mother->pdgId()) != abs(daughter->pdgId())) {
      return mother;
   }else{
      return find_first_mother_with_different_id(mother);
   }
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
std::vector<reco::GenJet> clean_v_jets(const std::vector<reco::GenJet> & genJets){
   std::vector<reco::GenJet> cleaned;
   for( auto const & j : genJets ) {
      bool found = false;
      for( auto const & c: j.getGenConstituents() ) {
         found |= has_v_in_history(*c);
         if(found) break;
      }
      if(not found){
         cleaned.push_back(j);
      }
   }

   return cleaned;
}


void MergingAnalyzer::setup_variables(const edm::Event& iEvent) {
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
   //// Jets
   std::vector<reco::GenJet> jets = clean_v_jets(clean_jets(select_jets(*genJets),leptons));
   int njets = jets.size();
   variables["n_jets"] = njets;
   if(njets>0){
      variables["pt_jet1"]   = jets.at(0).pt() ;
   }
   if(njets>1){
      variables["pt_jet2"]   = jets.at(1).pt() ;
   }
   if(njets>2){
      variables["pt_jet3"]   = jets.at(2).pt() ;
   }
   if(njets>3){
      variables["pt_jet4"]   = jets.at(3).pt() ;
   }


   // for (unsigned int i = 0; i<=10; i++){

   //    float threshold = i*10;
   //    int njet = 0;
   //    for ( auto const & j : jets ) {
   //       if(j.pt() > threshold){
   //          njet++;
   //       }
   //    }
   //    histograms2d["njet"]->Fill(threshold,njet,variables["weight"]);
   // }

   double ht = 0;
   for(auto const & j : jets){
      ht += j.pt();
   }
   variables["ht"] = ht;

}
std::vector<double> setup_weights(const edm::Event& iEvent) {
   edm::Handle<LHEEventProduct> lheHandle;
   iEvent.getByLabel("testWeights", lheHandle);
   const LHEWeightProduct * lheWeights = lheHandle.product();
   std::vector<float> weights = lheWeights.weights();

   for(auto i : scaleWeightOrder_){
      weights.push_back(weights[i]);
   }
   return weights;
}


// ------------ method called for each event  ------------
void
MergingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   setup_variables(iEvent);
   tree->Fill();
   auto weights = setup_weights(iEvent);
   h_count_event->Fill(0.5);
   h_count_sumw->Fill(0.5,weights.at(0));

   for( auto pair : histograms2d ) {
      for(uint i=0; i<weights.size(); i++) {
         // cout << pair.first <<
         double value = variables.at(pair.first);
         if(value > -9998){
               pair.second->Fill(value,i,weights.at(i));
         }
      }
   }
}


// ------------ method called once each job just before starting event loop  ------------
void
MergingAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
MergingAnalyzer::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MergingAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void 
LHEWeightProductProducer::beginRunProduce(edm::Run& run, edm::EventSetup const& es) {
    edm::Handle<LHEWeightInfoProduct> lheWeightInfoHandle;
    // get by token gives an error (the same one that's been in the ExternalLHEProducer for ages)
    run.getByLabel("testWeights", lheWeightInfoHandle);

    // Should add a search by name function
    gen::ScaleWeightGroup scaleWeightInfo;
    for (const auto& group : lheWeightInfoHandle->allWeightGroupsInfo()) {
        if (std::find("scale", group.name()) != std::string::npos) {
            scaleWeightInfo = group;
            break;
        }
    }
    
    // nano ordering of mur=0.5 muf=0.5 ; [1] is mur=0.5 muf=1 ; [2] is mur=0.5 muf=2 ; [3] is mur=1 muf=0.5 ; 
    // [4] is mur=1 muf=1 ; [5] is mur=1 muf=2 ; [6] is mur=2 muf=0.5 ; [7] is mur=2 muf=1 ; [8] is mur=2 muf=2 *
    scaleWeightOrder_ = { group.muR05muF05Index(),
        group.muR05muF1Index(),
        group.muR05muF2Index(),
        group.muR1muF05Index(),
        group.centralIndex(),
        group.muR1muF2Index(),
        group.muR2muF05Index(),
        group.muR2muF1Index(),
        group.muR2muF2Index(),
    }

}

//define this as a plug-in
DEFINE_FWK_MODULE(MergingAnalyzer);

