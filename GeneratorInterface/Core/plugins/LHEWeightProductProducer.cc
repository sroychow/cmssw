#include <cstdio>
#include <memory>
#include <vector>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "GeneratorInterface/LHEInterface/interface/LHERunInfo.h"
#include "GeneratorInterface/LHEInterface/interface/LHEEvent.h"
#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

class LHEWeightProductProducer : public edm::one::EDProducer<edm::BeginRunProducer,
                                                        edm::EndRunProducer> {
public:
  explicit LHEWeightProductProducer(const edm::ParameterSet& iConfig);
  ~LHEWeightProductProducer() override;
  
private:
  gen::LHEWeightHelper weightHelper_;
  edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;
  edm::EDGetTokenT<LHEEventProduct> lheEventToken_;

  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginRunProduce(edm::Run& run, edm::EventSetup const& es) override;
  void endRunProduce(edm::Run&, edm::EventSetup const&) override;

};

//
// constructors and destructor
//
LHEWeightProductProducer::LHEWeightProductProducer(const edm::ParameterSet& iConfig) :
    lheRunInfoToken_(consumes<LHERunInfoProduct, edm::InRun>(edm::InputTag("externalLHEProducer"))),
        //iConfig.getUntrackedParameter<edm::InputTag>("lheSource", edm::InputTag("externalLHEProducer")))),
    lheEventToken_(consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer")))
        //iConfig.getUntrackedParameter<edm::InputTag>("lheSource", edm::InputTag("externalLHEProducer"))))
{
  produces<GenWeightProduct>();
  produces<GenWeightInfoProduct, edm::Transition::BeginRun>();
}


LHEWeightProductProducer::~LHEWeightProductProducer()
{
}


// ------------ method called to produce the data  ------------
void
LHEWeightProductProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<LHEEventProduct> lheEventInfo;
  iEvent.getByToken(lheEventToken_, lheEventInfo);
  // Read weights from LHEEventProduct
  auto weightProduct = weightHelper_.weightProduct(lheEventInfo->weights());
  iEvent.put(std::move(weightProduct));
}

// ------------ method called when starting to processes a run  ------------
void 
LHEWeightProductProducer::beginRunProduce(edm::Run& run, edm::EventSetup const& es) {
    edm::Handle<LHERunInfoProduct> lheRunInfoHandle;
    //run.getByToken(lheRunInfoToken_, lheRunInfoHandle);
    // get by token gives an error (the same one that's been in the ExternalLHEProducer for ages)
    run.getByLabel("externalLHEProducer", lheRunInfoHandle);

    typedef std::vector<LHERunInfoProduct::Header>::const_iterator header_cit;
    LHERunInfoProduct::Header headerWeightInfo;
    for (header_cit iter=lheRunInfoHandle->headers_begin(); iter!=lheRunInfoHandle->headers_end(); iter++) { 
    //for (header_cit iter=lheRunInfoProduct.headers_begin(); iter!=lheRunInfoProduct.headers_end(); iter++) { 
        if (iter->tag() == "initrwgt")
            headerWeightInfo = *iter;
    }

	weightHelper_.parseWeightGroupsFromHeader(headerWeightInfo.lines());
    auto weightInfoProduct = std::make_unique<GenWeightInfoProduct>();
    for (auto& weightGroup : weightHelper_.weightGroups()) {
        weightInfoProduct->addWeightGroupInfo(weightGroup.clone());
    }
    run.put(std::move(weightInfoProduct));
}


// ------------ method called when ending the processing of a run  ------------
void 
LHEWeightProductProducer::endRunProduce(edm::Run& run, edm::EventSetup const& es) {
}

DEFINE_FWK_MODULE(LHEWeightProductProducer);

