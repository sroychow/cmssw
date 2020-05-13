#include <cuda_runtime.h>
#include "CUDADataFormats/Common/interface/Product.h"
#include "CUDADataFormats/Common/interface/HostProduct.h"
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DCUDA.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
#include "RecoLocalTracker/SiPixelRecHits/interface/pixelCPEforGPU.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class SiPixelValidateRecHitGPU : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateRecHitGPU(const edm::ParameterSet& iConfig);
  ~SiPixelValidateRecHitGPU() override = default;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  edm::EDGetTokenT<cms::cuda::Product<TrackingRecHit2DCUDA>> tokenHit_;  // CUDA hits
  edm::EDGetTokenT<SiPixelClusterCollectionNew> clusterToken_;

  uint32_t m_nHits;
  cms::cuda::host::unique_ptr<float[]> m_store32;
  cms::cuda::host::unique_ptr<uint32_t[]> m_hitsModuleStart;
  
  std::string topFolderName_;
  MonitorElement* nHits;
  MonitorElement* hrecHit_Barrel_XY;
  MonitorElement* hrecHit_Endcap_plus_XY;
  MonitorElement* hrecHit_Endcap_minus_XY;
  MonitorElement* hrecHit_Barrel_RZ;
  MonitorElement* hrecHit_Endcap_plus_RZ;
  MonitorElement* hrecHit_Endcap_minus_RZ;

};

SiPixelValidateRecHitGPU::SiPixelValidateRecHitGPU(const edm::ParameterSet& iConfig)
    : tokenHit_(consumes<cms::cuda::Product<TrackingRecHit2DCUDA>>(iConfig.getParameter<edm::InputTag>("pixelRecHitSrc"))),
      clusterToken_(consumes<SiPixelClusterCollectionNew>(iConfig.getParameter<edm::InputTag>("src")))
{
  topFolderName_ = "SiPixelHeterogeneousV/PixelRecHitGPU";
}

void SiPixelValidateRecHitGPU::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  nHits = ibooker.book1D("nrecHitsFromGPU", "Number of rechits from GPU", 100, 0, 10000);
  hrecHit_Barrel_XY = ibooker.book2D( "RecHit_Barrel_XY", "Barrel y vs. x", 960, -120, 120, 960, -120, 120 );
  hrecHit_Endcap_plus_XY = ibooker.book2D( "RecHit_Endcap_plus_XY", "RecHit Forward Endcap y vs. x", 960, -120, 120, 960, -120, 120 );
  hrecHit_Endcap_minus_XY = ibooker.book2D( "RecHit_Endcap_minus_XY", "RecHit Forward Endcap y vs. x", 960, -120, 120, 960, -120, 120 );

  hrecHit_Barrel_RZ = ibooker.book2D( "RecHit_Barrel_RZ", "RecHit #rho vs. z", 900, -300, 300, 480, 0, 120 );
  hrecHit_Endcap_plus_RZ = ibooker.book2D( "RecHit_Endcap_plus_RZ", "RecHit #rho vs. z", 900, -300, 300, 480, 0, 120 );
  hrecHit_Endcap_minus_RZ = ibooker.book2D( "RecHit_Endcap_minus_RZ", "RecHit #rho vs. z", 900, -300, 300, 480, 0, 120 );

}

void SiPixelValidateRecHitGPU::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("pixelRecHitSrc", edm::InputTag("siPixelRecHitsCUDAPreSplitting"));
  desc.add<edm::InputTag>("src", edm::InputTag("siPixelClustersPreSplitting"));
  descriptions.add("SiPixelValidateRecHitGPU", desc);
}

void SiPixelValidateRecHitGPU::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  cms::cuda::Product<TrackingRecHit2DCUDA> const& inputDataWrapped = iEvent.get(tokenHit_);
  cms::cuda::ScopedContextAnalyze ctx{inputDataWrapped};
  auto const& inputData = ctx.get(inputDataWrapped);
  m_nHits = inputData.nHits();
  if (m_nHits == 0)
    return;

  m_store32 = inputData.localCoordToHostAsync(ctx.stream());
  m_hitsModuleStart = inputData.hitsModuleStartToHostAsync(ctx.stream());
  nHits->Fill(m_nHits);

  auto xl = m_store32.get();
  auto yl = xl + m_nHits;
  //auto xe = yl + m_nHits;//errors
  //auto ye = xe + m_nHits;

  //get TkGeometry record
  edm::ESHandle<TrackerGeometry> geom;
  es.get<TrackerDigiGeometryRecord>().get(geom);
  geom = geom.product();

  edm::Handle<SiPixelClusterCollectionNew> hclusters;
  iEvent.getByToken(clusterToken_, hclusters);

  auto const& input = *hclusters;

  constexpr uint32_t MaxHitsInModule = gpuClustering::MaxHitsInModule;
  
  for (auto DSViter = input.begin(); DSViter != input.end(); DSViter++) {
    unsigned int detid = DSViter->detId();
    DetId detIdObject(detid);
    const GeomDetUnit* gDetUnit = geom->idToDetUnit(detIdObject);
    auto gind = gDetUnit->index();
    const PixelGeomDetUnit* pixDet = dynamic_cast<const PixelGeomDetUnit*>(gDetUnit);
    assert(pixDet);
    
    auto fc = m_hitsModuleStart[gind];
    auto lc = m_hitsModuleStart[gind + 1];
    auto nhits = lc - fc;
    
    nhits = std::min(nhits, MaxHitsInModule);
    
    if (0 == nhits)
      continue;
    auto jnd = [&](int k) { return fc + k; };
    for (auto const& clust : *DSViter) {
      assert(clust.originalId() >= 0);
      assert(clust.originalId() < DSViter->size());
      if (clust.originalId() >= nhits)
        continue;
      auto ij = jnd(clust.originalId());
      if (ij >= TrackingRecHit2DSOAView::maxHits())
	continue;  // overflow...
      LocalPoint lp(xl[ij], yl[ij]);
      
      GlobalPoint rechitPos = gDetUnit->surface().toGlobal(lp);
      
      if (gDetUnit->type().isBarrel()) {
	hrecHit_Barrel_XY->Fill( rechitPos.x(), rechitPos.y() );
	hrecHit_Barrel_RZ->Fill( rechitPos.z(), rechitPos.perp());
      } else if (gDetUnit->type().isEndcap()) {
	if(rechitPos.z() > 0) {
	  hrecHit_Endcap_plus_XY->Fill( rechitPos.x(), rechitPos.y() );
	  hrecHit_Endcap_plus_RZ->Fill( rechitPos.z(), rechitPos.perp());
	} else {
	  hrecHit_Endcap_minus_XY->Fill( rechitPos.x(), rechitPos.y() );
	  hrecHit_Endcap_minus_RZ->Fill( rechitPos.z(), rechitPos.perp());
	}
      }
    }
  }
}
DEFINE_FWK_MODULE(SiPixelValidateRecHitGPU);
