import FWCore.ParameterSet.Config as cms


dimu9ele9caloIdLTrackIdLdz_effmu = cms.EDAnalyzer("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_muegRef/*"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_subleadMuonPt          'Sub-Leading Muon pT turnON;            pT [GeV]; efficiency'     sflep2Pt_numerator          sflep2Pt_denominator",
        "effic_subleadMuonPt_variable 'Sub-Leading Muon pT turnON;            pT [GeV]; efficiency'     sflep2Pt_variable_numerator sflep2Pt_variable_denominator",
        "effic_subleadMuonPhi       'TriLeptonTrigger efficiency vs phi;  phi [rad]; efficiency' sflep2Phi_numerator       sflep2Phi_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_lep2pt_vs_LS 'Trailing lepton p_T efficiency vs LS; LS; Trailing lepton p_T efficiency' sflep2PtVsLS_numerator sflep2PtVsLS_denominator"
    ),
)
dimu9ele9caloIdLTrackIdLdz_effele = cms.EDAnalyzer("DQMGenericClient",
    subDirs        = cms.untracked.vstring("HLT/Higgs/TriLepton/HLT_DiMu9_Ele9_CaloIdL_TrackIdL_DZ_dimuRef/*"),
    verbose        = cms.untracked.uint32(0), # Set to 2 for all messages
    resolution     = cms.vstring(),
    efficiency     = cms.vstring(
        "effic_leadElectronPt          'Electron pT turnON;            pT [GeV]; efficiency'     cflepPt_numerator          cflepPt_denominator",
        "effic_leadElectronPt_variable 'Electron pT turnON;            pT [GeV]; efficiency'     cflepPt_variable_numerator cflepPt_variable_denominator",
        "effic_leadElectronPhi       'TriLeptonTrigger efficiency vs electron phi;  phi [rad]; efficiency' cflepPhi_numerator       cflepPhi_denominator",
    ),
    efficiencyProfile = cms.untracked.vstring(
        "effic_ElectronPt_vs_LS 'Trailing lepton p_T efficiency vs LS; LS; Trailing lepton p_T efficiency' cflepPtVsLS_numerator cflepPtVsLS_denominator"
    ),
)

higgsClient = cms.Sequence(
  dimu9ele9caloIdLTrackIdLdz_effmu + dimu9ele9caloIdLTrackIdLdz_effele
)
