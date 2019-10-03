// -*- C++ -*-
//
// Package:    TestWeightInfo/TestAnalyzer
// Class:      TestAnalyzer
//
/**\class TestAnalyzer TestAnalyzer.cc TestWeightInfo/TestAnalyzer/plugins/TestAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Kenneth David Long
//         Created:  Wed, 18 Sep 2019 13:44:58 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
 #include "FWCore/Utilities/interface/InputTag.h"
 #include "DataFormats/TrackReco/interface/Track.h"
 #include "DataFormats/TrackReco/interface/TrackFwd.h"
 #include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
 #include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
 #include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


using reco::TrackCollection;

class TestAnalyzer : public edm::one::EDAnalyzer<edm::one::WatchRuns>  {
   public:
      explicit TestAnalyzer(const edm::ParameterSet&);
      ~TestAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      edm::EDGetTokenT<GenWeightInfoProduct> lheWeightInfoToken_;
      virtual void beginJob() override;
      void beginRun(edm::Run const& iRun, edm::EventSetup const&) override;
      void endRun(edm::Run const& iRun, edm::EventSetup const&) override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
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
TestAnalyzer::TestAnalyzer(const edm::ParameterSet& iConfig) :
    lheWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InRun>(edm::InputTag("externalLHEProducer")))
{
   //now do what ever initialization is needed

}


TestAnalyzer::~TestAnalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    std::cerr << "Event" << std::endl;
}

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif


// ------------ method called once each job just before starting event loop  ------------
void
TestAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
TestAnalyzer::endJob()
{
}

void TestAnalyzer::endRun(edm::Run const& iRun, edm::EventSetup const&) {}

void TestAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const&) 
{   
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    iRun.getByToken( lheWeightInfoToken_, lheWeightInfoHandle );

    const GenWeightInfoProduct* lheProd = lheWeightInfoHandle.product();
    edm::OwnVector<gen::WeightGroupInfo> groups = lheProd->allWeightGroupsInfo();
    for (const auto& group : groups) {
        std::cout << "Type of the weight is " << group.weightType() << " name is " << group.name() << std::endl;
        if (group.weightType() == 1) {
            //gen::ScaleWeightGroupInfo* scaleInfo = group.clone();//static_cast<gen::ScaleWeightGroupInfo>(group);
            const gen::ScaleWeightGroupInfo* scaleInfo = dynamic_cast<const gen::ScaleWeightGroupInfo*>(&group);
            std::cout << "muR05muF1 " << scaleInfo->muR05muF1Index() << std::endl;
            std::cout << "muR05muF05 " << scaleInfo->muR05muF05Index() << std::endl;
        }
    }
}
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TestAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TestAnalyzer);
