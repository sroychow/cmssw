import FWCore.ParameterSet.Config as cms
#import the validation modules
##modules that import SoA directly from GPU
from Validation.SiPixelHeterogeneousV.pixelValidateRecHitGPU_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateVerticesGPU_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateDigiGPU_cfi import *

##modules that import intermediate SoA from CPU
from Validation.SiPixelHeterogeneousV.pixelValidateVerticesFromSoA_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateDigiFromSoA_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateTrackFromSoA_cfi import *

#Add the modules in the Sequence
#the following sequence needs to be added to the standard validation sequence##TODO
pixelHeterogeneousValidationSource = cms.Sequence(sipixeldigivalid
                                                  *sipixelrechitvalid
                                                  *pixelverticesvalid)

pixelHeterogeneousValidationFromsoaSource = cms.Sequence(sipixeldigifromsoavalid
                                                        *sipixelrechitvalid
                                                        *sipixeltrackfromsoavalid
                                                        *pixelverticesfromsoavalid)
