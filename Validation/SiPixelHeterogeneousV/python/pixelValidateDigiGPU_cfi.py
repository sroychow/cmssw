import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixeldigivalid = DQMEDAnalyzer("SiPixelValidateDigiGPU",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelClustersCUDAPreSplitting")
)
