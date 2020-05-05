import FWCore.ParameterSet.Config as cms
#import the validation modules
from Validation.SiPixelHeterogeneousV.pixelValidateRecHitGPU_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateVerticesGPU_cfi import *
from Validation.SiPixelHeterogeneousV.pixelValidateDigiGPU_cfi import *


#Add the modules in the Sequence
#the following sequence needs to be added to the standard validation sequence
pixelHeterogeneousValidationSource = cms.Sequence(sipixeldigivalid*sipixelrechitvalid*pixelverticesvalid)
