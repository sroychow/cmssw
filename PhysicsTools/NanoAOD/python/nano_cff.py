from __future__ import print_function
import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from PhysicsTools.NanoAOD.nano_eras_cff import *
from PhysicsTools.NanoAOD.jetsAK4_CHS_cff import *
from PhysicsTools.NanoAOD.jetsAK4_Puppi_cff import *
from PhysicsTools.NanoAOD.jetsAK8_cff import *
from PhysicsTools.NanoAOD.jetMC_cff import *
from PhysicsTools.NanoAOD.muons_cff import *
from PhysicsTools.NanoAOD.taus_cff import *
from PhysicsTools.NanoAOD.electrons_cff import *
from PhysicsTools.NanoAOD.photons_cff import *
from PhysicsTools.NanoAOD.globals_cff import *
from PhysicsTools.NanoAOD.extraflags_cff import *
from PhysicsTools.NanoAOD.ttbarCategorization_cff import *
from PhysicsTools.NanoAOD.genparticles_cff import *
from PhysicsTools.NanoAOD.particlelevel_cff import *
from PhysicsTools.NanoAOD.genVertex_cff import *
from PhysicsTools.NanoAOD.genWeights_cff import *
from PhysicsTools.NanoAOD.vertices_cff import *
from PhysicsTools.NanoAOD.met_cff import *
from PhysicsTools.NanoAOD.triggerObjects_cff import *
from PhysicsTools.NanoAOD.isotracks_cff import *
from PhysicsTools.NanoAOD.protons_cff import *
from PhysicsTools.NanoAOD.NanoAODEDMEventContent_cff import *


nanoMetadata = cms.EDProducer("UniqueStringProducer",
    strings = cms.PSet(
        tag = cms.string("untagged"),
    )
)

linkedObjects = cms.EDProducer("PATObjectCrossLinker",
   jets=cms.InputTag("finalJetsPuppi"),
   muons=cms.InputTag("finalMuons"),
   electrons=cms.InputTag("finalElectrons"),
   lowPtElectrons=cms.InputTag("finalLowPtElectrons"),
   taus=cms.InputTag("finalTaus"),
   photons=cms.InputTag("finalPhotons"),
)

# Switch to AK4 CHS jets for Run-2
run2_nanoAOD_ANY.toModify(linkedObjects, jets="finalJets")

simpleCleanerTable = cms.EDProducer("NanoAODSimpleCrossCleaner",
   name=cms.string("cleanmask"),
   doc=cms.string("simple cleaning mask with priority to leptons"),
   jets=cms.InputTag("linkedObjects","jets"),
   muons=cms.InputTag("linkedObjects","muons"),
   electrons=cms.InputTag("linkedObjects","electrons"),
   lowPtElectrons=cms.InputTag("linkedObjects","lowPtElectrons"),
   taus=cms.InputTag("linkedObjects","taus"),
   photons=cms.InputTag("linkedObjects","photons"),
   jetSel=cms.string("pt>15"),
   muonSel=cms.string("track.isNonnull && isLooseMuon && isPFMuon && innerTrack.validFraction >= 0.49 && ( isGlobalMuon && globalTrack.normalizedChi2 < 3 && combinedQuality.chi2LocalPosition < 12 && combinedQuality.trkKink < 20 && segmentCompatibility >= 0.303 || segmentCompatibility >= 0.451 )"),
   electronSel=cms.string(""),
   lowPtElectronSel=cms.string(""),
   tauSel=cms.string(""),
   photonSel=cms.string(""),
   jetName=cms.string("Jet"),muonName=cms.string("Muon"),electronName=cms.string("Electron"),
   lowPtElectronName=cms.string("LowPtElectron"),
   tauName=cms.string("Tau"),photonName=cms.string("Photon")
)

btagSFdir="PhysicsTools/NanoAOD/data/btagSF/"

lhcInfoTable = cms.EDProducer("LHCInfoProducer",
    precision = cms.int32(10),
)

nanoTableTaskCommon = cms.Task(
    cms.Task(nanoMetadata), 
    jetPuppiTask, jetPuppiForMETTask, jetAK8Task,
    extraFlagsProducersTask, muonTask, tauTask, boostedTauTask,
    electronTask , lowPtElectronTask, photonTask,
    vertexTask, isoTrackTask, jetAK8LepTask,  # must be after all the leptons
    softActivityTask,
    cms.Task(linkedObjects),
    jetPuppiTablesTask, jetAK8TablesTask,
    muonTablesTask, fsrTablesTask, tauTablesTask, boostedTauTablesTask,
    electronTablesTask, lowPtElectronTablesTask, photonTablesTask,
    globalTablesTask, vertexTablesTask, metTablesTask, simpleCleanerTable, extraFlagsTableTask,
    isoTrackTablesTask,softActivityTablesTask
)

# Replace AK4 Puppi with AK4 CHS for Run-2
_nanoTableTaskCommonRun2 = nanoTableTaskCommon.copy()
_nanoTableTaskCommonRun2.replace(jetPuppiTask, jetTask)
_nanoTableTaskCommonRun2.replace(jetPuppiForMETTask, jetForMETTask)
_nanoTableTaskCommonRun2.replace(jetPuppiTablesTask, jetTablesTask)
run2_nanoAOD_ANY.toReplaceWith(nanoTableTaskCommon, _nanoTableTaskCommonRun2)

lheInfoTable = cms.EDProducer("LHETablesProducer",
    lheInfo = cms.VInputTag(cms.InputTag("externalLHEProducer"), cms.InputTag("source")),
    precision = cms.int32(14),
    storeLHEParticles = cms.bool(True),
    storeAllLHEInfo = cms.bool(False),  
)

l1bits=cms.EDProducer("L1TriggerResultsConverter", src=cms.InputTag("gtStage2Digis"), legacyL1=cms.bool(False),
                      storeUnprefireableBit=cms.bool(True), src_ext=cms.InputTag("gtStage2Digis"))
(run2_miniAOD_80XLegacy | run2_nanoAOD_94X2016 | run2_nanoAOD_94XMiniAODv1 | run2_nanoAOD_94XMiniAODv2 | run2_nanoAOD_102Xv1).toModify(l1bits, storeUnprefireableBit=False)

nanoTableTaskFS = cms.Task(
    genParticleTask, particleLevelTask, jetMCTask, muonMCTask, electronMCTask, lowPtElectronMCTask, photonMCTask,
    tauMCTask, boostedTauMCTask,
    metMCTable, ttbarCatMCProducersTask, globalTablesMCTask, cms.Task(btagWeightTable), ttbarCategoryTableTask,
    genWeightsTableTask, genVertexTablesTask, genParticleTablesTask, genProtonTablesTask, particleLevelTablesTask
)

nanoSequence = cms.Sequence(nanoSequenceCommon + nanoSequenceOnlyData + nanoSequenceOnlyFullSim)

nanoSequenceFS = cms.Sequence(genParticleSequence + genVertexTables + particleLevelSequence + nanoSequenceCommon + jetMC + muonMC + electronMC + photonMC + tauMC + metMC + ttbarCatMCProducers +  globalTablesMC + btagWeightTable + genWeightsTable + genVertexTable + genParticleTables + particleLevelTables + lheInfoTable  + ttbarCategoryTable )

(run2_nanoAOD_92X | run2_miniAOD_80XLegacy | run2_nanoAOD_94X2016 | run2_nanoAOD_94X2016 | \
    run2_nanoAOD_94XMiniAODv1 | run2_nanoAOD_94XMiniAODv2 | \
    run2_nanoAOD_102Xv1).toReplaceWith(nanoSequenceFS, nanoSequenceFS.copyAndExclude([genVertexTable, genVertexT0Table]))

# GenVertex only stored in newer MiniAOD
nanoSequenceMC = nanoSequenceFS.copy()
nanoSequenceMC.insert(nanoSequenceFS.index(nanoSequenceCommon)+1,nanoSequenceOnlyFullSim)

# modify extraFlagsTable to store ecalBadCalibFilter decision which is re-run with updated bad crystal list for 2017 and 2018 samples
for modifier in run2_nanoAOD_94XMiniAODv1, run2_nanoAOD_94XMiniAODv2, run2_nanoAOD_102Xv1:
    modifier.toModify(extraFlagsTable, variables= cms.PSet())
    modifier.toModify(extraFlagsTable, variables = dict(Flag_ecalBadCalibFilterV2 = ExtVar(cms.InputTag("ecalBadCalibFilterNanoTagger"), bool, doc = "Bad ECAL calib flag (updated xtal list)")))

# modifier which adds new tauIDs (currently only deepTauId2017v2p1 is being added)
import RecoTauTag.RecoTau.tools.runTauIdMVA as tauIdConfig
def nanoAOD_addTauIds(process):
    updatedTauName = "slimmedTausUpdated"
    tauIdEmbedder = tauIdConfig.TauIDEmbedder(process, debug = False, updatedTauName = updatedTauName,
            toKeep = [ "deepTau2017v2p1" ])
    tauIdEmbedder.runTauID()
    process.patTauMVAIDsSeq.insert(process.patTauMVAIDsSeq.index(getattr(process, updatedTauName)),
                                   process.rerunMvaIsolationSequence)
    return process

def nanoAOD_addBoostedTauIds(process, idsToRun=[]):
    if idsToRun: #no-empty list of tauIDs to run
        updatedBoostedTauName = "slimmedTausBoostedNewID"
        boostedTauIdEmbedder = tauIdConfig.TauIDEmbedder(process, debug = False,
                                                         originalTauName = "slimmedTausBoosted",
                                                         updatedTauName = updatedBoostedTauName,
                                                         postfix = "Boosted",
                                                         toKeep = idsToRun)
        boostedTauIdEmbedder.runTauID()
        _boostedTauTask = process.rerunMvaIsolationTaskBoosted.copy()
        _boostedTauTask.add(getattr(process, updatedBoostedTauName))
        _boostedTauTask.add(process.finalBoostedTaus)
        process.finalBoostedTaus.src = updatedBoostedTauName
        #remember to adjust the selection and tables with added IDs

        process.boostedTauTask = _boostedTauTask.copy()

    return process

def nanoAOD_addDeepMET(process, addDeepMETProducer, ResponseTune_Graph):
    if addDeepMETProducer:
        # produce DeepMET on the fly if it is not in MiniAOD
        print("add DeepMET Producers")
        process.load('RecoMET.METPUSubtraction.deepMETProducer_cfi')
        process.deepMETsResolutionTune = process.deepMETProducer.clone()
        process.deepMETsResponseTune = process.deepMETProducer.clone()
        #process.deepMETsResponseTune.graph_path = 'RecoMET/METPUSubtraction/data/deepmet/deepmet_resp_v1_2018.pb'
        process.deepMETsResponseTune.graph_path = ResponseTune_Graph.value()
    process.metTables += process.deepMetTables
    return process

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD
def nanoAOD_recalibrateMETs(process,isData):
    # add DeepMETs
    nanoAOD_DeepMET_switch = cms.PSet(
        nanoAOD_addDeepMET_switch = cms.untracked.bool(True), # decide if DeeMET should be included in Nano
        nanoAOD_produceDeepMET_switch = cms.untracked.bool(False), # decide if DeepMET should be computed on the fly
        ResponseTune_Graph = cms.untracked.string('RecoMET/METPUSubtraction/data/deepmet/deepmet_resp_v1_2018.pb')
    )
    for modifier in run2_miniAOD_80XLegacy, run2_nanoAOD_94X2016, run2_nanoAOD_94XMiniAODv1, run2_nanoAOD_94XMiniAODv2, run2_nanoAOD_102Xv1, run2_nanoAOD_106Xv1:
        # compute DeepMETs in these eras (before 111X)
        modifier.toModify(nanoAOD_DeepMET_switch, nanoAOD_produceDeepMET_switch =  cms.untracked.bool(True))
    for modifier in run2_miniAOD_80XLegacy, run2_nanoAOD_94X2016:
        modifier.toModify(nanoAOD_DeepMET_switch, ResponseTune_Graph=cms.untracked.string("RecoMET/METPUSubtraction/data/deepmet/deepmet_resp_v1_2016.pb"))
    if nanoAOD_DeepMET_switch.nanoAOD_addDeepMET_switch:
        process = nanoAOD_addDeepMET(process,
                                     addDeepMETProducer=nanoAOD_DeepMET_switch.nanoAOD_produceDeepMET_switch,
                                     ResponseTune_Graph=nanoAOD_DeepMET_switch.ResponseTune_Graph)

    # if included in Nano, and not computed in the fly, then it should be extracted from minAOD
    extractDeepMETs = nanoAOD_DeepMET_switch.nanoAOD_addDeepMET_switch and not nanoAOD_DeepMET_switch.nanoAOD_produceDeepMET_switch

    runMetCorAndUncFromMiniAOD(process,isData=isData, extractDeepMETs=extractDeepMETs)
    process.nanoSequenceCommon.insert(process.nanoSequenceCommon.index(process.jetSequence),cms.Sequence(process.fullPatMetSequence))
    process.basicJetsForMetForT1METNano = process.basicJetsForMet.clone(
        src = process.updatedJetsWithUserData.src,
        skipEM = False,
        type1JetPtThreshold = 0.0,
        calcMuonSubtrRawPtAsValueMap = cms.bool(True),
    )
    process.jetSequence.insert(process.jetSequence.index(process.updatedJetsWithUserData),cms.Sequence(process.basicJetsForMetForT1METNano))
    process.updatedJetsWithUserData.userFloats.muonSubtrRawPt = cms.InputTag("basicJetsForMetForT1METNano:MuonSubtrRawPt")
    process.corrT1METJetTable.src = process.finalJets.src
    process.corrT1METJetTable.cut = "pt<15 && abs(eta)<9.9"
    for table in process.jetTable, process.corrT1METJetTable:
        table.variables.muonSubtrFactor = Var("1-userFloat('muonSubtrRawPt')/(pt()*jecFactor('Uncorrected'))",float,doc="1-(muon-subtracted raw pt)/(raw pt)",precision=6)
    process.metTables += process.corrT1METJetTable
#    makePuppiesFromMiniAOD(process,True) # call this before in the global customizer otherwise it would reset photon IDs in VID
    nanoAOD_PuppiV15_switch = cms.PSet(
            recoMetFromPFCs = cms.untracked.bool(False),
            reclusterJets = cms.untracked.bool(False),
            )
    run2_nanoAOD_106Xv1.toModify(nanoAOD_PuppiV15_switch,recoMetFromPFCs=True,reclusterJets=True)
    if nanoAOD_PuppiV15_switch.reclusterJets:
        from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
        from PhysicsTools.PatAlgos.tools.helpers import getPatAlgosToolsTask, addToProcessAndTask
        task = getPatAlgosToolsTask(process)
        addToProcessAndTask('ak4PuppiJets', ak4PFJets.clone (src = 'puppi', doAreaFastjet = True, jetPtMin = 10.), process, task)
        from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection
        addJetCollection(process,
            labelName = 'Puppi',
            jetSource = cms.InputTag('ak4PuppiJets'),
            algo = 'AK', rParam=0.4,
            genJetCollection=cms.InputTag('slimmedGenJets'),
            jetCorrections = ('AK4PFPuppi', ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual'], 'None'),
            pfCandidates = cms.InputTag('packedPFCandidates'),
            pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
            svSource = cms.InputTag('slimmedSecondaryVertices'),
            muSource =cms.InputTag( 'slimmedMuons'),
            elSource = cms.InputTag('slimmedElectrons'),
            genParticles= cms.InputTag('prunedGenParticles'),
            getJetMCFlavour= False
        )

        process.patJetsPuppi.addGenPartonMatch = cms.bool(False)
        process.patJetsPuppi.addGenJetMatch = cms.bool(False)
        print("nanoAOD_PuppiV15_switch.reclusterJets is true")

    runMetCorAndUncFromMiniAOD(process,isData=isData,metType="Puppi",postfix="Puppi",jetFlavor="AK4PFPuppi", recoMetFromPFCs=bool(nanoAOD_PuppiV15_switch.recoMetFromPFCs), reclusterJets=bool(nanoAOD_PuppiV15_switch.reclusterJets))
    process.nanoSequenceCommon.insert(2,cms.Sequence(process.puppiMETSequence+process.fullPatMetSequencePuppi))

    return process

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
def nanoAOD_activateVID(process):
    switchOnVIDElectronIdProducer(process,DataFormat.MiniAOD)
    for modname in electron_id_modules_WorkingPoints_nanoAOD.modules:
        setupAllVIDIdsInModule(process,modname,setupVIDElectronSelection)
    process.electronSequence.insert(process.electronSequence.index(process.bitmapVIDForEle),process.egmGsfElectronIDSequence)
    for modifier in run2_egamma_2017,run2_egamma_2018,run2_miniAOD_80XLegacy,run2_nanoAOD_94XMiniAODv1,run2_nanoAOD_94XMiniAODv2,run2_nanoAOD_94X2016,run2_nanoAOD_102Xv1,run2_nanoAOD_106Xv1:
        modifier.toModify(process.electronMVAValueMapProducer, src = "slimmedElectronsUpdated")
        modifier.toModify(process.egmGsfElectronIDs, physicsObjectSrc = "slimmedElectronsUpdated")


    switchOnVIDPhotonIdProducer(process,DataFormat.MiniAOD) # do not call this to avoid resetting photon IDs in VID, if called before inside makePuppiesFromMiniAOD
    for modname in photon_id_modules_WorkingPoints_nanoAOD.modules:
        setupAllVIDIdsInModule(process,modname,setupVIDPhotonSelection)
    process.photonSequence.insert(process.photonSequence.index(bitmapVIDForPho),process.egmPhotonIDSequence)
    for modifier in run2_miniAOD_80XLegacy,run2_nanoAOD_94XMiniAODv1,run2_nanoAOD_94XMiniAODv2,run2_nanoAOD_94X2016,run2_nanoAOD_102Xv1:
        modifier.toModify(process.photonMVAValueMapProducer, src = "slimmedPhotonsTo106X")
        modifier.toModify(process.egmPhotonIDs, physicsObjectSrc = "slimmedPhotonsTo106X")
    return process

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
def nanoAOD_runMETfixEE2017(process,isData):
    runMetCorAndUncFromMiniAOD(process,isData=isData,
                               fixEE2017 = True,
                               fixEE2017Params = {'userawPt': True, 'ptThreshold':50.0, 'minEtaThreshold':2.65, 'maxEtaThreshold': 3.139},
                               postfix = "FixEE2017")
    process.nanoSequenceCommon.insert(process.nanoSequenceCommon.index(jetSequence),process.fullPatMetSequenceFixEE2017)

def nanoAOD_customizeCommon(process):
    makePuppiesFromMiniAOD(process,True)
    process.puppiNoLep.useExistingWeights = True
    process.puppi.useExistingWeights = True
    run2_nanoAOD_106Xv1.toModify(process.puppiNoLep, useExistingWeights = False)
    run2_nanoAOD_106Xv1.toModify(process.puppi, useExistingWeights = False)
    process = nanoAOD_activateVID(process)
    # This function is defined in jetsAK4_CHS_cff.py
    process = nanoAOD_addDeepInfoAK4CHS(process,
        addDeepBTag=nanoAOD_addDeepInfoAK4CHS_switch.nanoAOD_addDeepBTag_switch,
        addDeepFlavour=nanoAOD_addDeepInfoAK4CHS_switch.nanoAOD_addDeepFlavourTag_switch
    )

    # This function is defined in jetsAK8_cff.py
    process = nanoAOD_addDeepInfoAK8(process,
        addDeepBTag=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addDeepBTag_switch,
        addDeepBoostedJet=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addDeepBoostedJet_switch,
        addDeepDoubleX=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addDeepDoubleX_switch,
        addDeepDoubleXV2=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addDeepDoubleXV2_switch,
        addParticleNet=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addParticleNet_switch,
        addParticleNetMass=nanoAOD_addDeepInfoAK8_switch.nanoAOD_addParticleNetMass_switch,
        jecPayload=nanoAOD_addDeepInfoAK8_switch.jecPayload
    )

    nanoAOD_tau_switch = cms.PSet(
        idsToAdd = cms.vstring()
    )
    (run2_nanoAOD_94XMiniAODv1 | run2_nanoAOD_94X2016 | run2_nanoAOD_94XMiniAODv2 | run2_nanoAOD_102Xv1 | run2_nanoAOD_106Xv1).toModify(nanoAOD_tau_switch, idsToAdd = ["deepTau2017v2p1"])
    (run2_nanoAOD_94XMiniAODv1 | run2_nanoAOD_94X2016 | run2_nanoAOD_94XMiniAODv2 | run2_nanoAOD_102Xv1 | run2_nanoAOD_106Xv1).toModify(process, lambda p : nanoAOD_addTauIds(p, nanoAOD_tau_switch.idsToAdd.value()))
    nanoAOD_boostedTau_switch = cms.PSet(
        idsToAdd = cms.vstring()
    )
    run2_nanoAOD_106Xv2.toModify(nanoAOD_boostedTau_switch, idsToAdd = ["2017v2", "dR0p32017v2", "newDM2017v2","againstEle2018"])
    run2_nanoAOD_106Xv2.toModify(process, lambda p : nanoAOD_addBoostedTauIds(p, nanoAOD_boostedTau_switch.idsToAdd.value()))
    return process

def nanoAOD_customizeData(process):
    process = nanoAOD_customizeCommon(process)
    for modifier in run2_miniAOD_80XLegacy,run2_nanoAOD_94X2016,run2_nanoAOD_94XMiniAODv1,run2_nanoAOD_94XMiniAODv2,run2_nanoAOD_102Xv1,run2_nanoAOD_106Xv1:
        modifier.toModify(process, lambda p: nanoAOD_recalibrateMETs(p,isData=True))
    for modifier in run2_nanoAOD_94XMiniAODv1, run2_nanoAOD_94XMiniAODv2:
        modifier.toModify(process, lambda p: nanoAOD_runMETfixEE2017(p,isData=True))
    return process

def nanoAOD_customizeMC(process):
    process = nanoAOD_customizeCommon(process)
    process = nanoAOD_recalibrateMETs(process,isData=False)
    for modifier in run2_nanoAOD_94XMiniAODv1, run2_nanoAOD_94XMiniAODv2:
        modifier.toModify(process, lambda p: nanoAOD_runMETfixEE2017(p,isData=False))
    return process

###increasing the precision of selected GenParticles.
def nanoWmassGenCustomize(process):
    pdgSelection="?(abs(pdgId) == 11|| abs(pdgId)==13 || abs(pdgId)==15 ||abs(pdgId)== 12 || abs(pdgId)== 14 || abs(pdgId)== 16|| abs(pdgId)== 24|| pdgId== 23)"
    # Keep precision same as default RECO for selected particles
    ptPrecision="{}?{}:{}".format(pdgSelection, CandVars.pt.precision.value(),genParticleTable.variables.pt.precision.value())
    process.genParticleTable.variables.pt.precision=cms.string(ptPrecision)
    phiPrecision="{} ? {} : {}".format(pdgSelection, CandVars.phi.precision.value(), genParticleTable.variables.phi.precision.value())
    process.genParticleTable.variables.phi.precision=cms.string(phiPrecision)
    etaPrecision="{} ? {} : {}".format(pdgSelection, CandVars.eta.precision.value(), genParticleTable.variables.eta.precision.value())
    process.genParticleTable.variables.eta.precision=cms.string(etaPrecision)
    return process

# lowPtElectrons do not exsit for old nano campaigns (i.e. before v9)
_modifiers = ( run2_miniAOD_80XLegacy |
               run2_nanoAOD_94XMiniAODv1 |
               run2_nanoAOD_94XMiniAODv2 |
               run2_nanoAOD_94X2016 |
               run2_nanoAOD_102Xv1 |
               run2_nanoAOD_106Xv1 )
_modifiers.toModify(linkedObjects,lowPtElectrons="")
_modifiers.toModify(simpleCleanerTable,lowPtElectrons="")
