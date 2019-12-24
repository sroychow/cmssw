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

class LHEWeightProductProducer : public edm::one::EDProducer<edm::BeginLuminosityBlockProducer,
                                                                edm::one::WatchRuns> {
public:
  explicit LHEWeightProductProducer(const edm::ParameterSet& iConfig);
  ~LHEWeightProductProducer() override;
  
private:
  gen::LHEWeightHelper weightHelper_;
  std::string lheLabel_;
  edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;
  edm::EDGetTokenT<LHEEventProduct> lheEventToken_;

  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginLuminosityBlockProduce(edm::LuminosityBlock& lumi, edm::EventSetup const& es) override;
  void beginRun(edm::Run const& run, edm::EventSetup const& es) override;
  void endRun(edm::Run const& run, edm::EventSetup const& es) override;

};

//
// constructors and destructor
//
LHEWeightProductProducer::LHEWeightProductProducer(const edm::ParameterSet& iConfig) :
    lheLabel_(iConfig.getUntrackedParameter<std::string>("lheSourceLabel")),
    lheRunInfoToken_(consumes<LHERunInfoProduct, edm::InRun>(
        iConfig.getUntrackedParameter<edm::InputTag>("lheSource", edm::InputTag("externalLHEProducer")))),
    lheEventToken_(consumes<LHEEventProduct>(
        iConfig.getUntrackedParameter<edm::InputTag>("lheSource", edm::InputTag("externalLHEProducer"))))
{
  produces<GenWeightProduct>();
  produces<GenWeightInfoProduct, edm::Transition::BeginLuminosityBlock>();
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
LHEWeightProductProducer::beginRun(edm::Run const& run, edm::EventSetup const& es) {
    edm::Handle<LHERunInfoProduct> lheRunInfoHandle;
    //run.getByToken(lheRunInfoToken_, lheRunInfoHandle);
    // get by token gives an error (the same one that's been in the ExternalLHEProducer for ages)
    run.getByLabel(lheLabel_, lheRunInfoHandle);

    typedef std::vector<LHERunInfoProduct::Header>::const_iterator header_cit;
    LHERunInfoProduct::Header headerWeightInfo;
    for (header_cit iter=lheRunInfoHandle->headers_begin(); iter!=lheRunInfoHandle->headers_end(); iter++) { 
        if (iter->tag() == "initrwgt")
            headerWeightInfo = *iter;
    }

	weightHelper_.parseWeightGroupsFromHeader(headerWeightInfo.lines());
}

void
LHEWeightProductProducer::endRun(edm::Run const& run, edm::EventSetup const& es) { }

void
LHEWeightProductProducer::beginLuminosityBlockProduce(edm::LuminosityBlock& lumi, edm::EventSetup const& es) {
    auto weightInfoProduct = std::make_unique<GenWeightInfoProduct>();
    for (auto& weightGroup : weightHelper_.weightGroups()) {
        weightInfoProduct->addWeightGroupInfo(weightGroup.clone());
    }
    lumi.put(std::move(weightInfoProduct));
}

DEFINE_FWK_MODULE(LHEWeightProductProducer);

