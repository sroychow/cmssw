#ifndef RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPTT6COMMONMODENOISESUBTRACTION_H
#define RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPTT6COMMONMODENOISESUBTRACTION_H
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripCommonModeNoiseSubtractor.h"
#include "CondFormats/DataRecord/interface/SiStripNoisesRcd.h"
#include "CalibTracker/Records/interface/SiStripQualityRcd.h"

#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
class SiStripNoises;
class SiStripQuality;

class TT6CMNSubtractor : public SiStripCommonModeNoiseSubtractor {
  friend class SiStripRawProcessingFactory;

public:
  void init(const edm::EventSetup& es) override;
  void subtract(uint32_t detId, uint16_t firstAPV, std::vector<int16_t>& digis) override;
  void subtract(uint32_t detId, uint16_t firstAPV, std::vector<float>& digis) override;

private:
  template <typename T>
  void subtract_(uint32_t detId, uint16_t firstAPV, std::vector<T>& digis);

  TT6CMNSubtractor(double in, edm::ConsumesCollector&& iC)
      : cut_to_avoid_signal_(in),
        noiseToken_(iC.esConsumes<SiStripNoises, SiStripNoisesRcd>()),
        qualityToken_(iC.esConsumes<SiStripQuality, SiStripQualityRcd>()) {}
  double cut_to_avoid_signal_;
  edm::ESGetToken<SiStripNoises, SiStripNoisesRcd> noiseToken_;
  edm::ESGetToken<SiStripQuality, SiStripQualityRcd> qualityToken_;
  const SiStripNoises* noiseHandle;
  const SiStripQuality* qualityHandle;
  edm::ESWatcher<SiStripNoisesRcd> noiseWatcher_;
  edm::ESWatcher<SiStripQualityRcd> qualityWatcher_;
};
#endif
