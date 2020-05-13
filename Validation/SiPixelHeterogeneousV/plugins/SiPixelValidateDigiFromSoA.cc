#include <cuda_runtime.h>
#include "CUDADataFormats/Common/interface/Product.h"
#include "CUDADataFormats/Common/interface/HostProduct.h"
#include "CUDADataFormats/SiPixelDigi/interface/SiPixelDigisCUDA.h"
#include "DataFormats/SiPixelDigi/interface/SiPixelDigisSoA.h"
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

class SiPixelValidateDigiFromSoA : public DQMEDAnalyzer {

public:
  explicit SiPixelValidateDigiFromSoA(const edm::ParameterSet& iConfig);
  ~SiPixelValidateDigiFromSoA() override = default;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
private:
  void analyze(const edm::Event& iEvent, edm::EventSetup const& iSetup) override;
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;

  edm::EDGetTokenT<SiPixelDigisSoA>  digisoaToken_;
  
  std::string topFolderName_;
  MonitorElement* hnDigis;
  MonitorElement* hadc;
};

SiPixelValidateDigiFromSoA::SiPixelValidateDigiFromSoA(const edm::ParameterSet& iConfig)
    : digisoaToken_(consumes<SiPixelDigisSoA>(iConfig.getParameter<edm::InputTag>("src")))
{
  topFolderName_ = "SiPixelHeterogeneousV/PixelDigisSoA";
}

void SiPixelValidateDigiFromSoA::bookHistograms(DQMStore::IBooker& ibooker, edm::Run const& run , edm::EventSetup const& es) {
  ibooker.cd();
  ibooker.setCurrentFolder(topFolderName_);
  hnDigis = ibooker.book1D("nDigis", ";nDigis;#entries", 1000, 0, 1000000);
  hadc = ibooker.book1D("adc", "Digi ADC values", 500, 0, 50000);
}

void SiPixelValidateDigiFromSoA::analyze(const edm::Event& iEvent, edm::EventSetup const& es) {
  const auto&  soadigis = iEvent.get(digisoaToken_);
  const uint32_t nDigis = soadigis.size();

  hnDigis->Fill(nDigis);

  const std::vector<uint32_t>& pdigiVec = soadigis.pdigiVector();
  const std::vector<uint32_t>& rawIdVec = soadigis.rawIdArrVector();
  const std::vector<uint16_t>& adcVec   = soadigis.adcVector();
  const std::vector<int32_t>&  clus     = soadigis.clusVector();

  //get TkGeometry record
  edm::ESHandle<TrackerGeometry> geom;
  es.get<TrackerDigiGeometryRecord>().get(geom);
  geom = geom.product();

  for(uint32_t idigi = 0; idigi < nDigis; idigi++) {
    DetId detId(rawIdVec[idigi]);
    if(detId.null())  continue; 
    if(detId.det() != DetId::Detector::Tracker)   continue;
    auto adc_idigi = adcVec[idigi];
    hadc->Fill(adc_idigi);
  }

}

void SiPixelValidateDigiFromSoA::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("siPixelDigisSoA"));
  descriptions.add("SiPixelValidateDigiFromSoA", desc);
}
DEFINE_FWK_MODULE(SiPixelValidateDigiFromSoA);
