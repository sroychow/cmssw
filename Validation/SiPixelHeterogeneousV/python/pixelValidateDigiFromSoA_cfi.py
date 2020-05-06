import FWCore.ParameterSet.Config as cms

from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer

sipixeldigifromsoavalid = DQMEDAnalyzer("SiPixelValidateDigiFromSoA",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelDigisSoA")
)
