// -*- C++ -*-
///bookLayer
// Package:    SiPixelPhase1MonitorRecHitsSoACPU
// Class:      SiPixelPhase1MonitorRecHitsSoACPU
//
/**\class SiPixelPhase1MonitorRecHitsSoACPU SiPixelPhase1MonitorRecHitsSoACPU.cc 
*/
//
// Author: Alessandro Rossi, Suvankar Roy Chowdhury
//
#include "DataFormats/Math/interface/approx_atan2.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DHeterogeneous.h"

#include "SiPixelPhase1MonitorRecHitsSoABase.h"

class SiPixelPhase1MonitorRecHitsSoACPU : public SiPixelPhase1MonitorRecHitsSoABase {
public:
  explicit SiPixelPhase1MonitorRecHitsSoACPU(const edm::ParameterSet&);
  ~SiPixelPhase1MonitorRecHitsSoACPU() override = default;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::EDGetTokenT<TrackingRecHit2DCPU> tokenSoAHitsCPU_;
};

//
// constructors
//

SiPixelPhase1MonitorRecHitsSoACPU::SiPixelPhase1MonitorRecHitsSoACPU(const edm::ParameterSet& iConfig)
    : SiPixelPhase1MonitorRecHitsSoABase(iConfig) {
  tokenSoAHitsCPU_ = consumes<TrackingRecHit2DCPU>(iConfig.getParameter<edm::InputTag>("pixelHitsSrc"));
}

//
// -- Analyze
//
void SiPixelPhase1MonitorRecHitsSoACPU::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  const auto& rhsoaHandle = iEvent.getHandle(tokenSoAHitsCPU_);
  if (!rhsoaHandle.isValid())
    return;
  auto const& rhsoa = iEvent.get(tokenSoAHitsCPU_);  //*((rhsoaHandle.product())->get());
  const TrackingRecHit2DSOAView* soa2d = rhsoa.view();

  uint32_t nHits_ = soa2d->nHits();
  hnHits->Fill(nHits_);
  auto detIds = tkGeom_->detUnitIds();
  for (uint32_t i = 0; i < nHits_; i++) {
    DetId id = detIds[soa2d->detectorIndex(i)];
    float xG = soa2d->xGlobal(i);
    float yG = soa2d->yGlobal(i);
    float zG = soa2d->zGlobal(i);
    float rG = soa2d->rGlobal(i);
    float fphi = short2phi(soa2d->iphi(i));
    uint32_t charge = soa2d->charge(i);
    int16_t sizeX = std::ceil(float(std::abs(soa2d->clusterSizeX(i)))/8.);
    int16_t sizeY = std::ceil(float(std::abs(soa2d->clusterSizeY(i)))/8.);
    fillHistosForRecHit(id, xG, yG, zG, rG, fphi, charge, sizeX, sizeY);
  }
}

void SiPixelPhase1MonitorRecHitsSoACPU::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // monitorpixelRecHitsSoA
  edm::ParameterSetDescription desc;
  SiPixelPhase1MonitorRecHitsSoABase::fillPSetDescription(desc);
  desc.add<edm::InputTag>("pixelHitsSrc", edm::InputTag("siPixelRecHitsPreSplitting@cpu"));
  desc.add<bool>("onGPU", false);
  descriptions.addWithDefaultLabel(desc);
}
DEFINE_FWK_MODULE(SiPixelPhase1MonitorRecHitsSoACPU);
