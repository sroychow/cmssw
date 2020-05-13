import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixeltrackfromsoavalid = DQMEDAnalyzer("SiPixelValidateTrackFromSoA",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("pixelTrackSoA")
)
