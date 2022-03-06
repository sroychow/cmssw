// -*- C++ -*-
///bookLayer
// Package:    SiPixelPhase1MonitorRecHitsSoAGPU
// Class:      SiPixelPhase1MonitorRecHitsSoAGPU
//
/**\class SiPixelPhase1MonitorRecHitsSoAGPU SiPixelPhase1MonitorRecHitsSoAGPU.cc 
*/
//
// Author: Alessandro Rossi, Suvankar Roy Chowdhury
//
#include "DataFormats/Math/interface/approx_atan2.h"
#include "CUDADataFormats/Common/interface/Product.h"
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DHeterogeneous.h"
// for string manipulations
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "SiPixelPhase1MonitorRecHitsSoABase.h"

class SiPixelPhase1MonitorRecHitsSoAGPU : public SiPixelPhase1MonitorRecHitsSoABase {
public:
  explicit SiPixelPhase1MonitorRecHitsSoAGPU(const edm::ParameterSet&);
  ~SiPixelPhase1MonitorRecHitsSoAGPU() override = default;
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  edm::EDGetTokenT<cms::cuda::Product<TrackingRecHit2DGPU>> tokenHitsGPU_;
  edm::EDGetTokenT<SiPixelClusterCollectionNew> clusterToken_;
  cms::cuda::host::unique_ptr<float[]> m_store32;
  cms::cuda::host::unique_ptr<uint32_t[]> m_hitsModuleStart;
};

//
// constructors
//

SiPixelPhase1MonitorRecHitsSoAGPU::SiPixelPhase1MonitorRecHitsSoAGPU(const edm::ParameterSet& iConfig) : SiPixelPhase1MonitorRecHitsSoABase(iConfig)
{
  tokenHitsGPU_ = consumes<cms::cuda::Product<TrackingRecHit2DGPU>>(iConfig.getParameter<edm::InputTag>("pixelHitsSrc"));
  clusterToken_ = consumes<SiPixelClusterCollectionNew>(iConfig.getParameter<edm::InputTag>("pixelClusterSrc"));
}

//
// -- Analyze
//
void SiPixelPhase1MonitorRecHitsSoAGPU::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  const auto& rhsoaHandle = iEvent.getHandle(tokenHitsGPU_);
  if (!rhsoaHandle.isValid()) {
    edm::LogWarning("SiPixelPhase1MonitorRecHitsSoAGPU") << "No CUDA RecHits found \n returning!" << std::endl;
    return;
  }
  const auto& rho = iEvent.get(tokenHitsGPU_);
  cms::cuda::ScopedContextAnalyze ctx{rho};    
  const auto& rhsoa = ctx.get(rho);
  auto m_nHits = rhsoa.nHits();
  hnHits->Fill(m_nHits);
  if (m_nHits == 0)
    return;
  edm::LogWarning("SiPixelPhase1MonitorRecHitsSoAGPU") << "Total RecHits : " << m_nHits << std::endl; 
  m_store32 = rhsoa.localCoordToHostAsync(ctx.stream());
  m_hitsModuleStart = rhsoa.hitsModuleStartToHostAsync(ctx.stream());
  auto xl = m_store32.get();
  auto yl = xl + m_nHits;
  //auto xe = yl + m_nHits;//errors
  //auto ye = xe + m_nHits;

  auto const& clusters = iEvent.get(clusterToken_);

  constexpr uint32_t maxHitsInModule = gpuClustering::maxHitsInModule();
  
  for (auto DSViter = clusters.begin(); DSViter != clusters.end(); DSViter++) {
    unsigned int detid = DSViter->detId();
    DetId detIdObject(detid);
    const GeomDetUnit* gDetUnit = tkGeom_->idToDetUnit(detIdObject);
    auto gind = gDetUnit->index();
    const PixelGeomDetUnit* pixDet = dynamic_cast<const PixelGeomDetUnit*>(gDetUnit);
    assert(pixDet);
    auto fc = m_hitsModuleStart[gind];
    auto lc = m_hitsModuleStart[gind + 1];
    auto nhits = lc - fc;
    
    nhits = std::min(nhits, maxHitsInModule);
    if (0 == nhits)      continue;
    auto jnd = [&](int k) { return fc + k; };
    for (auto const& clust : *DSViter) {
      assert(clust.originalId() >= 0);
      assert(clust.originalId() < DSViter->size());
      if (clust.originalId() >= nhits)
        continue;
      auto ij = jnd(clust.originalId());
      LocalPoint lp(xl[ij], yl[ij]);
      
      GlobalPoint globalPos = gDetUnit->surface().toGlobal(lp);

      float xG=globalPos.x();//in cm
      float yG=globalPos.y();
      float zG=globalPos.z();
      float rG=globalPos.perp();
      float fphi=short2phi(globalPos.phi());
      //uint32_t charge=globalPos.charge(i);
      //int16_t sizeX=globalPos.clusterSizeX(i);
      //int16_t sizeY=globalPos.clusterSizeY(i);

      fillHistosForRecHit(detid, xG, yG, zG, rG, fphi);
    }
  }
}

void SiPixelPhase1MonitorRecHitsSoAGPU::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  SiPixelPhase1MonitorRecHitsSoABase::fillPSetDescription(desc);
  desc.add<edm::InputTag>("pixelHitsSrc", edm::InputTag("siPixelRecHitsPreSplittingCUDA"));
  desc.add<edm::InputTag>("pixelClusterSrc", edm::InputTag("siPixelClustersPreSplitting"));
  desc.add<bool>("onGPU", true);
  descriptions.addWithDefaultLabel(desc);
}
DEFINE_FWK_MODULE(SiPixelPhase1MonitorRecHitsSoAGPU);
