from __future__ import print_function
import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.globals_cff import *
from PhysicsTools.NanoAOD.electrons_cff import *
from PhysicsTools.NanoAOD.genparticles_cff import *
from PhysicsTools.NanoAOD.particlelevel_cff import *
from PhysicsTools.NanoAOD.genWeights_cff import *
from PhysicsTools.NanoAOD.genVertex_cff import *
from PhysicsTools.NanoAOD.vertices_cff import *
from PhysicsTools.NanoAOD.NanoAODEDMEventContent_cff import *
from PhysicsTools.PatAlgos.slimming.miniAOD_tools import *

# for electron ID
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
from RecoEgamma.ElectronIdentification.ElectronMVAValueMapProducer_cfi import *
from RecoEgamma.ElectronIdentification.Identification.mvaElectronID_tools import EleMVA_WP, EleMVARaw_WP

egmGsfElectronIDsNano = cms.EDProducer(
        "VersionedGsfElectronIdProducer",
        physicsObjectSrc = cms.InputTag('slimmedElectronsUpdated'),
        physicsObjectIDs = cms.VPSet()
)
electronMVAValueMapProducerNano = electronMVAValueMapProducer.clone()
electronMVAValueMapProducerNano.src = cms.InputTag("gedGsfElectrons", processName=cms.InputTag.skipCurrentProcess())
#electronMVAValueMapProducerNano.src = cms.InputTag("gedGsfElectronsFrom94XTo106X")
electronMVAValueMapProducerNano.srcMiniAOD = "slimmedElectronsUpdated"

egmGsfElectronIDTaskNano = cms.Task(
        electronMVAValueMapProducerNano,
        egmGsfElectronIDsNano
)
egmGsfElectronIDSequenceNano = cms.Sequence(egmGsfElectronIDTaskNano)

electron_id_modules_WorkingPoints_nanoAOD.modules = cms.vstring(
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff',
    'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff', 
)
electron_id_modules_WorkingPoints_nanoAOD.WorkingPoints = cms.vstring(
    "egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-veto",
    "egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-loose",
    "egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-medium",
    "egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-tight",
)

from copy import deepcopy
for modname in electron_id_modules_WorkingPoints_nanoAOD.modules:
        idmod= __import__(modname, globals(), locals(), ['idName','cutFlow'])
        #print("mod name ", modname)
        for name in dir(idmod):
                item = getattr(idmod,name)
                #print("item ", item)
                if hasattr(item,'idName') and hasattr(item,'cutFlow'):
                    #item = deepcopy(item)
                    #if 'mva' in item.idName.value().lower():
                    #    # change the MVA producer name to electronMVAValueMapProducerNano 
                    #    item.cutFlow[0].mvaValueMapName = item.cutFlow[0].mvaValueMapName.value().replace("electronMVAValueMapProducer","electronMVAValueMapProducerNano")
                    #    item.cutFlow[0].mvaCategoriesMapName = item.cutFlow[0].mvaCategoriesMapName.value().replace("electronMVAValueMapProducer","electronMVAValueMapProducerNano")
                    setupVIDSelection(egmGsfElectronIDsNano, item)

electronSequence_LowPU = electronSequence.copyAndExclude([slimmedElectronsTo106X])
electronSequence_LowPU.insert(electronSequence_LowPU.index(bitmapVIDForEle), egmGsfElectronIDSequenceNano)

slimmedElectronsWithUserData.userFloats = cms.PSet(
    miniIsoChg = cms.InputTag("isoForEle:miniIsoChg"),
    miniIsoAll = cms.InputTag("isoForEle:miniIsoAll"),
    PFIsoChg = cms.InputTag("isoForEle:PFIsoChg"),
    PFIsoAll = cms.InputTag("isoForEle:PFIsoAll"),
    PFIsoAll04 = cms.InputTag("isoForEle:PFIsoAll04"),
    ptRatio = cms.InputTag("ptRatioRelForEle:ptRatio"),
    ptRel = cms.InputTag("ptRatioRelForEle:ptRel"),
    jetNDauChargedMVASel = cms.InputTag("ptRatioRelForEle:jetNDauChargedMVASel"),
    ecalTrkEnergyErrPostCorrNew = cms.InputTag("calibratedPatElectronsNano","ecalTrkEnergyErrPostCorr"),
    ecalTrkEnergyPreCorrNew     = cms.InputTag("calibratedPatElectronsNano","ecalTrkEnergyPreCorr"),
    ecalTrkEnergyPostCorrNew    = cms.InputTag("calibratedPatElectronsNano","ecalTrkEnergyPostCorr"),
    energyScaleUpNew               = cms.InputTag("calibratedPatElectronsNano","energyScaleUp"),
    energyScaleDownNew             = cms.InputTag("calibratedPatElectronsNano","energyScaleDown"),
    energySigmaUpNew               = cms.InputTag("calibratedPatElectronsNano","energySigmaUp"),
    energySigmaDownNew             = cms.InputTag("calibratedPatElectronsNano","energySigmaDown"),
    ecalEnergyPreCorrNew        = cms.InputTag("calibratedPatElectronsNano","ecalEnergyPreCorr"),
    ecalEnergyPostCorrNew       = cms.InputTag("calibratedPatElectronsNano","ecalEnergyPostCorr"),
)

slimmedElectronsWithUserData.userIntFromBools = cms.PSet(
        cutbasedID_Fall17_V1_veto   = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V1-veto"),
        cutbasedID_Fall17_V1_loose  = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V1-loose"),
        cutbasedID_Fall17_V1_medium = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V1-medium"),
        cutbasedID_Fall17_V1_tight  = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V1-tight"),
        cutbasedID_Fall17_V2_veto   = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-veto"),
        cutbasedID_Fall17_V2_loose  = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-loose"),
        cutbasedID_Fall17_V2_medium = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-medium"),
        cutbasedID_Fall17_V2_tight  = cms.InputTag("egmGsfElectronIDsNano:cutBasedElectronID-Fall17-94X-V2-tight"),
        cutbasedID_HEEP             = cms.InputTag("egmGsfElectronIDsNano:heepElectronID-HEEPV70"),
)

linkedElectrons = cms.EDFilter("PATElectronSelector",
    src = finalElectrons.src,
    cut = finalElectrons.cut,
)

electronTable.src = "linkedElectrons"
slimmedElectronsUpdated.src = cms.InputTag("slimmedElectrons")

bitmapVIDForEle.WorkingPoints = electron_id_modules_WorkingPoints_nanoAOD.WorkingPoints
bitmapVIDForEleHEEP.WorkingPoints = cms.vstring("egmGsfElectronIDsNano:heepElectronID-HEEPV70")

## SC candidates
superClusterMerger =  cms.EDProducer("EgammaSuperClusterMerger",
    src = cms.VInputTag(cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALBarrel"),
                        cms.InputTag("particleFlowSuperClusterECAL:particleFlowSuperClusterECALEndcapWithPreshower"),
#                        cms.InputTag("particleFlowEGamma"),
                        ),
)


superClusterCands = cms.EDProducer("ConcreteEcalCandidateProducer",
                                   src = cms.InputTag("superClusterMerger"),
                                   particleType = cms.int32(11),
                                   )

goodSuperClusters = cms.EDFilter("RecoEcalCandidateRefSelector",
                                 src = cms.InputTag("superClusterCands"),
                                 cut = cms.string('abs(eta)<2.5 &&  et>5.0'),
                                 filter = cms.bool(True)
                                 )


recoEcalCandidateHelper = cms.EDProducer("RecoEcalCandidateVariableHelper",
                                         probes = cms.InputTag("superClusterCands"),
                                         countTracks = cms.bool( False ),
                                         trkIsoPtMin = cms.double( 0.5 ),
                                         trkIsoStripEndcap = cms.double( 0.03 ),
                                         trackProducer = cms.InputTag( "generalTracks" ),
                                         trkIsoStripBarrel = cms.double( 0.03 ),
                                         trkIsoConeSize = cms.double( 0.4 ),
                                         trkIsoVetoConeSize = cms.double( 0.06 ),
                                         trkIsoRSpan = cms.double( 999999.0 ),
                                         trkIsoZSpan = cms.double( 999999. )
                                         )

electronSCTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
    src = cms.InputTag("goodSuperClusters"),
    cut = cms.string(""),
    name= cms.string("ElectronSC"),
    doc = cms.string("superclusters associated to electrons"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the electrons
    variables = cms.PSet(CandVars,
    ),
)
sc_sequenceAOD = cms.Sequence(
        superClusterMerger      +
        superClusterCands       +
        goodSuperClusters       +
        electronSCTable
)

from PhysicsTools.NanoAOD.jets_cff import jetCorrFactorsNano,updatedJets
electronTPSequence_LowPU = cms.Sequence(jetCorrFactorsNano + updatedJets + electronSequence_LowPU + linkedElectrons + electronTable + sc_sequenceAOD)
electronTPSequenceMC_LowPU = cms.Sequence(jetCorrFactorsNano + updatedJets + electronSequence_LowPU + linkedElectrons + electronTable + mergedGenParticles + genParticles2HepMC + electronMC + sc_sequenceAOD)