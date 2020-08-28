#include <cuda_runtime.h>
#include "CUDADataFormats/Common/interface/Product.h"
#include "CUDADataFormats/Common/interface/HostProduct.h"
#include "CUDADataFormats/Track/interface/PixelTrackHeterogeneous.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
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
#include "Geometry/CommonTopologies/interface/GeomDetEnumerators.h"
#include "HeterogeneousCore/CUDACore/interface/ScopedContext.h"
#include "RecoLocalTracker/SiPixelRecHits/interface/pixelCPEforGPU.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class SiPixelValidateTrackFromSoA : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateTrackFromSoA(const edm::ParameterSet& iConfig);
  ~SiPixelValidateTrackFromSoA() override = default;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;

  edm::EDGetTokenT<PixelTrackHeterogeneous>  tracksoaToken_;
  
  std::string topFolderName_;
  MonitorElement* hnTracks;
};

SiPixelValidateTrackFromSoA::SiPixelValidateTrackFromSoA(const edm::ParameterSet& iConfig)
    : tracksoaToken_(consumes<PixelTrackHeterogeneous>(iConfig.getParameter<edm::InputTag>("src")))
{
  topFolderName_ = "SiPixelHeterogenousV/PixelTracksSoA";
}

void SiPixelValidateTrackFromSoA::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  hnTracks = ibooker.book1D("nTracksLoose", "#Tracks with quality >= loose;nTracks;#entries", 100, 0, 1000);
}

void SiPixelValidateTrackFromSoA::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  const auto&  soatracks = *(iEvent.get(tracksoaToken_)); 
  const auto *quality = soatracks.qualityData();
  uint32_t nTk = 0;
  uint32_t nTkLoose = 0;
  //soatracks.m_nTracks; this is always 0?
  //std::cout <<  "Ntracks:" << soatracks.m_nTracks << "\tmax Tracks=" << soatracks.stride() << std::endl;

  //taken from https://github.com/sroychow/cmssw/blob/pixelPhase1gpuvalid/RecoPixelVertexing/PixelTrackFitting/plugins/PixelTrackProducerFromSoA.cc
  for (int32_t it = 0; it < soatracks.stride(); ++it) {
    auto nHits = soatracks.nHits(it);
    if (nHits == 0)  break;
    nTk++;
    auto q = quality[it];
    if (q != trackQuality::loose)  continue;
    nTkLoose++;
  }
  //std::cout <<  "Ntracks:" << nTk << "\tloose Tracks=" << nTkLoose << std::endl;
  hnTracks->Fill(nTkLoose);

}

void SiPixelValidateTrackFromSoA::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("pixelTrackSoA"));
  descriptions.add("SiPixelValidateTrackFromSoA", desc);
}
DEFINE_FWK_MODULE(SiPixelValidateTrackFromSoA);
