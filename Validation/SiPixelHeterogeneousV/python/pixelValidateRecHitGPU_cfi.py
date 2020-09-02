import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixelrechitvalid = DQMEDAnalyzer("SiPixelValidateRecHitGPU",
    mightGet = cms.optional.untracked.vstring,
    pixelRecHitSrc = cms.InputTag("siPixelRecHitsCUDAPreSplitting"),
    src = cms.InputTag("siPixelClustersPreSplitting")
)
