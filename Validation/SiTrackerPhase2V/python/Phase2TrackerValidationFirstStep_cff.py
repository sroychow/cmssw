import FWCore.ParameterSet.Config as cms
from Validation.SiTrackerPhase2V.Phase2TrackerValidateDigi_cff import *
from Validation.SiTrackerPhase2V.Phase2TrackerValidateRecHit_cfi import *

trackerphase2ValidationSource = cms.Sequence(pixDigiValid  + otDigiValid)
#trackerphase2ValidationSource = cms.Sequence(pixDigiValid  + otDigiValid + rechitValidIT + rechitValidOT)
