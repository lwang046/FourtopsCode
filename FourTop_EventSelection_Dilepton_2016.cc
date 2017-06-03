////////////////////////////////////////////////////////////////////////////////////
////         Analysis code for search for Four Top Production.                  ////
////////////////////////////////////////////////////////////////////////////////////

// ttbar @ NLO 13 TeV:
// all-had ->679 * .46 = 312.34
// semi-lep ->679 *.45 = 305.55
// di-lep-> 679* .09 = 61.11

// ttbar @ NNLO 8 TeV:
// all-had -> 245.8 * .46 = 113.068
// semi-lep-> 245.8 * .45 = 110.61
// di-lep ->  245.8 * .09 = 22.122

#define _USE_MATH_DEFINES
#include "TStyle.h"
#include "TPaveText.h"
#include "TTree.h"
#include "TNtuple.h"
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <ctime>
#include <cmath>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include "TRandom3.h"
#include "TRandom.h"
#include "TProfile.h"
#include <iostream>
#include <map>
#include <cstdlib>

#include "TopTreeProducer/interface/TRootRun.h"
#include "TopTreeProducer/interface/TRootEvent.h"
#include "TopTreeAnalysisBase/Selection/interface/Run2Selection.h"
#include "TopTreeAnalysisBase/Content/interface/AnalysisEnvironment.h"
#include "TopTreeAnalysisBase/Content/interface/Dataset.h"
#include "TopTreeAnalysisBase/Tools/interface/JetTools.h"
#include "TopTreeAnalysisBase/Tools/interface/BTagWeightTools.h"
#include "TopTreeAnalysisBase/Tools/interface/BTagCalibrationStandalone.h"
#include "TopTreeAnalysisBase/Tools/interface/JetCombiner.h"
//	#include "TopTreeAnalysisBase/Tools/interface/MVATrainer.h"
//	#include "TopTreeAnalysisBase/Tools/interface/MVAComputer.h"
#include "TopTreeAnalysisBase/Tools/interface/TopologyWorker.h"
//#include "TopTreeAnalysisBase/Tools/interface/PlottingTools.h"
//#include "TopTreeAnalysisBase/Tools/interface/MultiSamplePlot.h"
#include "TopTreeAnalysisBase/Tools/interface/Trigger.h"
#include "TopTreeAnalysisBase/Tools/interface/TTreeLoader.h"
#include "TopTreeAnalysisBase/Tools/interface/LeptonTools.h"
#include "TopTreeAnalysisBase/Tools/interface/AnalysisEnvironmentLoader.h"
#include "TopTreeAnalysisBase/Reconstruction/interface/JetCorrectorParameters.h"
#include "TopTreeAnalysisBase/Reconstruction/interface/JetCorrectionUncertainty.h"
#include "TopTreeAnalysisBase/Reconstruction/interface/MakeBinning.h"
#include "TopTreeAnalysisBase/Reconstruction/interface/MEzCalculator.h"
#include "TopTreeAnalysisBase/Reconstruction/interface/TTreeObservables.h"
#include "TopTreeAnalysisBase/MCInformation/interface/LumiReWeighting.h"
#include "TopTreeAnalysisBase/MCInformation/interface/JetPartonMatching.h"

using namespace std;
using namespace TopTree;
using namespace reweight;

bool split_ttbar = false;
float topness;

pair<float, vector<unsigned int> > MVAvals1;
pair<float, vector<unsigned int> > MVAvals2;
pair<float, vector<unsigned int> > MVAvals2ndPass;
pair<float, vector<unsigned int> > MVAvals3rdPass;

int nMVASuccesses = 0;
int nMatchedEvents = 0;

map<string, TH1F*> histo1D;
map<string, TH2F*> histo2D;
map<string, TProfile*> histoProfile;

struct HighestCVSBtag {
    bool operator()(TRootJet* j1, TRootJet* j2) const
    {
        return j1->btag_combinedInclusiveSecondaryVertexV2BJetTags() > j2->btag_combinedInclusiveSecondaryVertexV2BJetTags();
    }
};

// To cout the Px, Py, Pz, E and Pt of objects
int Factorial(int N);
float Sphericity(vector<TLorentzVector> parts);
float Centrality(vector<TLorentzVector> parts);
float ElectronRelIso(TRootElectron* el, float rho);
float MuonRelIso(TRootMuon* mu);
float PythiaTune(int jets);

int main(int argc, char* argv[])
{

    // Checking Passed Arguments to ensure proper execution of MACRO
    if(argc < 12) {
        std::cerr << "TOO FEW INPUTs FROM XMLFILE.  CHECK XML INPUT FROM SCRIPT.  " << argc << " ARGUMENTS HAVE BEEN PASSED." << std::endl;
        return 1;
    }
    // Placing arguments in properly typed variables for Dataset creation
    const string dName = argv[1];
    const string dTitle = argv[2];
    const int color = strtol(argv[4], NULL, 10);
    const int ls = strtol(argv[5], NULL, 10);
    const int lw = strtol(argv[6], NULL, 10);
    const float normf = strtod(argv[7], NULL);
    const float EqLumi = strtod(argv[8], NULL);
    const float xSect = strtod(argv[9], NULL);
    const float PreselEff = strtod(argv[10], NULL);
    vector<string> vecfileNames;
    for(int args = 11; args < argc; args++) {
        vecfileNames.push_back(argv[args]);
    }
    cout << "---Dataset accepted from command line---" << endl;
    cout << "Dataset Name: " << dName << endl;
    cout << "Dataset Title: " << dTitle << endl;
    cout << "Dataset color: " << color << endl;
    cout << "Dataset ls: " << ls << endl;
    cout << "Dataset lw: " << lw << endl;
    cout << "Dataset normf: " << normf << endl;
    cout << "Dataset EqLumi: " << EqLumi << endl;
    cout << "Dataset xSect: " << xSect << endl;
    for(int files = 0; files < vecfileNames.size(); files++) {
        cout << "Dataset File Names: " << vecfileNames[files] << endl;
    }
    cout << "----------------------------------------------------------------------" << endl;

    int passed = 0;
    int negWeights = 0;
    float weightCount = 0.0;
    int eventCount = 0, trigCount = 0;
    clock_t start = clock();
    string postfix = "_Run2_TopTree_Study";
    int doJESShift = 0; // 0: off 1: minus 2: plus
    cout << "doJESShift: " << doJESShift << endl;
    int doJERShift = 0; // 0: off (except nominal scalefactor for jer) 1: minus 2: plus
    cout << "doJERShift: " << doJERShift << endl;
    int dobTagEffShift = 0; // 0: off (except nominal scalefactor for btag eff) 1: minus 2: plus
    cout << "dobTagEffShift: " << dobTagEffShift << endl;
    int domisTagEffShift = 0; // 0: off (except nominal scalefactor for mistag eff) 1: minus 2: plus
    cout << "domisTagEffShift: " << domisTagEffShift << endl;
    int mvaNegWeight = 0; // 0: Using MVA trained without neg weights 1: Using MVA trained with Neg Weights
    cout << "mvaNegWeight: " << mvaNegWeight << endl;
    if(doJESShift == 1)
        postfix = postfix + "_JESMinus";
    if(doJESShift == 2)
        postfix = postfix + "_JESPlus";
    if(doJERShift == 1)
        postfix = postfix + "_JERMinus";
    if(doJERShift == 2)
        postfix = postfix + "_JERPlus";
    if(dobTagEffShift == -1)
        postfix = postfix + "_bTagMinus";
    if(dobTagEffShift == 1)
        postfix = postfix + "_bTagPlus";
    if(domisTagEffShift == -1)
        postfix = postfix + "_misTagMinus";
    if(domisTagEffShift == 1)
        postfix = postfix + "_misTagPlus";
    if(mvaNegWeight == 1)
        postfix = postfix + "_MVANegWeight";

    cout << "*************************************************************" << endl;
    cout << " Beginning of the program for the FourTop search ! " << endl;
    cout << "*************************************************************" << endl;


    ///////////////////////////////////////
    //      Configuration                //
    ///////////////////////////////////////

    string channelpostfix = "";
    string MVAmethod = "BDT";  // MVAmethod to be used to get the good jet combi calculation (not for training! this is chosen in the jetcombiner class)
    float Luminosity; // pb^-1 

    bool debug = false;
    bool dilepton = true;
    bool Muon = false;
    bool Electron = false;
    bool HadTopOn = true;
    bool EventBDTOn = true;
    bool TrainMVA = false; // If false, the previously trained MVA will be used to calculate stuff
    bool bTagReweight = false;
    bool bTagCSVReweight = true;
    bool bLeptonSF = true;
    bool applyJER = false;
    bool applyJEC = true;
    bool JERNom = false;
    bool JERUp = false;
    bool JERDown = false;
    bool JESUp = false;
    bool JESDown = false;
    bool fillingbTagHistos = false;
    bool verbose = false;

    if(dName.find("MuEl") != std::string::npos) {
        Muon = true;
        Electron = true;
    } else if(dName.find("MuMu") != std::string::npos) {
        Muon = true;
        Electron = false;
    } else if(dName.find("ElEl") != std::string::npos) {
        Muon = false;
        Electron = true;
    } else
        cout << "Boolean setting by name failed" << endl;

    if(Muon && Electron && dilepton) {
        cout << " --> Using the Muon-Electron channel..." << endl;
        channelpostfix = "_MuEl";
    } else if(Muon && !Electron && dilepton) {
        cout << " --> Using the Muon-Muon channel..." << endl;
        channelpostfix = "_MuMu";
    } else if(!Muon && Electron && dilepton) {
        cout << " --> Using the Electron-Electron channel..." << endl;
        channelpostfix = "_ElEl";
    } else {
        cerr << "Correct Di-lepton Channel not selected." << endl;
        exit(1);
    }

    //////////////////////////////////
    //  Set up AnalysisEnvironment  //
    //////////////////////////////////

    AnalysisEnvironment anaEnv;
    cout << "Creating environment ..." << endl;
    anaEnv.PrimaryVertexCollection = "PrimaryVertex";
    anaEnv.JetCollection = "PFJets_slimmedJets";
    anaEnv.FatJetCollection = "FatJets_slimmedJetsAK8";
    anaEnv.METCollection = "PFMET_slimmedMETs";
    anaEnv.MuonCollection = "Muons_slimmedMuons";
    anaEnv.ElectronCollection = "Electrons_selectedElectrons";
    anaEnv.GenJetCollection = "GenJets_slimmedGenJets";
    //anaEnv.TrackMETCollection = "";
    //anaEnv.GenEventCollection = "GenEvent";
    anaEnv.NPGenEventCollection = "NPGenEvent";
    anaEnv.MCParticlesCollection = "MCParticles";
    anaEnv.loadFatJetCollection = true;
    anaEnv.loadGenJetCollection = true;
    //anaEnv.loadGenEventCollection = false;
    anaEnv.loadNPGenEventCollection = false;
    anaEnv.loadMCParticles = true;
    //anaEnv.loadTrackMETCollection = false;
    anaEnv.JetType = 2;
    anaEnv.METType = 2;
    //anaEnv.Verbose;

    ////////////////////////////////
    //  Load datasets             //
    ////////////////////////////////

    TTreeLoader treeLoader;
    vector<Dataset*> datasets;
    cout << "Creating Dataset ..." << endl;
    Dataset* theDataset = new Dataset(dName, dTitle, true, color, ls, lw, normf, xSect, vecfileNames);
    theDataset->SetEquivalentLuminosity(EqLumi * normf);
    datasets.push_back(theDataset);
    string dataSetName = theDataset->Name();

    bool isData = false;
    if(dataSetName.find("Data") != string::npos || dataSetName.find("data") != string::npos || dataSetName.find("DATA") != string::npos) {
        isData = true;
    }
    
    if(dataSetName.find("Data_Run2016B") != string::npos) Luminosity = 5743.725979478;
    else if(dataSetName.find("Data_Run2016C") != string::npos) Luminosity = 2573.399420069;
    else if(dataSetName.find("Data_Run2016D") != string::npos) Luminosity = 4248.383597366;
    else if(dataSetName.find("Data_Run2016E") != string::npos) Luminosity = 4009.132398404;
    else if(dataSetName.find("Data_Run2016F") != string::npos) Luminosity = 3101.618412335;
    else if(dataSetName.find("Data_Run2016G") != string::npos) Luminosity = 7540.487735974;
    else if(dataSetName.find("Data_Run2016H") != string::npos) Luminosity = 8390.540446658+215.149415251;
    else Luminosity = 35822.4374055;

    /////////////////////////////
    //   Initialize Trigger    //
    /////////////////////////////

    Trigger* trigger_mumu = new Trigger(1, 0, 0, 1,0,0,0);  // mu , el, single, double, tri, met, jet
    Trigger* trigger_muel = new Trigger(1, 1, 0, 1,0,0,0);
    Trigger* trigger_elel = new Trigger(0, 1, 0, 1,0,0,0);
    Trigger* trigger_mu = new Trigger(1, 0, 1, 0,0,0,0);

    ////////////////////////////////
    // Setting Up Scaling Objects //
    ////////////////////////////////

    //////////////////////////////////////////////////////
    //     bTag calibration reader and weight tools     //
    //////////////////////////////////////////////////////
    cout << "Loading btag calibration files ..." << endl;
    BTagCalibration * bTagCalib;   
    BTagCalibrationReader * bTagReader;
    BTagCalibrationReader * bTagReaderUp;
    BTagCalibrationReader * bTagReaderDown;
    BTagCalibrationReader * reader_csvv2; //for csv reshaping 
    BTagCalibrationReader * reader_JESUp;
    BTagCalibrationReader * reader_JESDown;
    BTagCalibrationReader * reader_LFUp;
    BTagCalibrationReader * reader_LFDown;
    BTagCalibrationReader * reader_HFUp;
    BTagCalibrationReader * reader_HFDown;
    BTagCalibrationReader * reader_HFStats1Up;
    BTagCalibrationReader * reader_HFStats1Down;
    BTagCalibrationReader * reader_HFStats2Up;
    BTagCalibrationReader * reader_HFStats2Down;
    BTagCalibrationReader * reader_LFStats1Up;
    BTagCalibrationReader * reader_LFStats1Down;
    BTagCalibrationReader * reader_LFStats2Up;
    BTagCalibrationReader * reader_LFStats2Down;
    BTagCalibrationReader * reader_CFErr1Up;
    BTagCalibrationReader * reader_CFErr1Down;
    BTagCalibrationReader * reader_CFErr2Up;
    BTagCalibrationReader * reader_CFErr2Down;

    BTagWeightTools* btwt;
    BTagWeightTools* btwtUp;
    BTagWeightTools* btwtDown;

    if(bTagReweight && !isData) {
        // Btag documentation : http://mon.iihe.ac.be/~smoortga/TopTrees/BTagSF/BTaggingSF_inTopTrees.pdf //v2 or _v2
        bTagCalib = new BTagCalibration("CSVv2", "../TopTreeAnalysisBase/Calibrations/BTagging/CSVv2Moriond17_2017_1_26_BtoH.csv");
        bTagReader = new BTagCalibrationReader(bTagCalib, BTagEntry::OP_MEDIUM, "mujets", "central");  // mujets
        bTagReaderUp = new BTagCalibrationReader(bTagCalib, BTagEntry::OP_MEDIUM, "mujets", "up");     // mujets
        bTagReaderDown = new BTagCalibrationReader(bTagCalib, BTagEntry::OP_MEDIUM, "mujets", "down"); // mujets
        if(fillingbTagHistos) {
            btwt = new BTagWeightTools(bTagReader, "bTagWeightHistosPtEta_" + dataSetName + ".root", false, 20, 670, 2.4);
            btwtUp = new BTagWeightTools(bTagReader, "bTagWeightHistosPtEta_" + dataSetName + "_Up.root", false, 20, 670, 2.4);
            btwtDown = new BTagWeightTools(bTagReader, "bTagWeightHistosPtEta_" + dataSetName + "_Down.root", false, 20, 670, 2.4);
        } else {
            btwt = new BTagWeightTools(bTagReader, "bTagWeightHistosPtEta_TTJetsPowheg.root", false, 20, 670, 2.4);
            btwtUp = new BTagWeightTools(bTagReaderUp, "bTagWeightHistosPtEta_TTJetsPowheg.root", false, 20, 670, 2.4);
            btwtDown = new BTagWeightTools(bTagReaderDown, "bTagWeightHistosPtEta_TTJetsPowheg.root", false, 20, 670, 2.4);
        }
    }

    if(bTagCSVReweight && !isData){
        BTagCalibration calib_csvv2("csvv2", "../TopTreeAnalysisBase/Calibrations/BTagging/CSVv2Moriond17_2017_1_26_BtoH.csv");
        reader_csvv2 = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "central");
        reader_JESUp = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_jes");
        reader_JESDown = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_jes");
        reader_LFUp = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_lf");
        reader_LFDown = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_lf");
        reader_HFUp = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_hf");
        reader_HFDown = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_hf");
        reader_HFStats1Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_hfstats1");
        reader_HFStats1Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_hfstats1");
        reader_HFStats2Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_hfstats2");
        reader_HFStats2Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_hfstats2");
        reader_LFStats1Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_lfstats1");
        reader_LFStats1Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_lfstats1");
        reader_LFStats2Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_lfstats2");
        reader_LFStats2Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_lfstats2"); 
        reader_CFErr1Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_cferr1");
        reader_CFErr1Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_cferr1");
        reader_CFErr2Up = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "up_cferr2");
        reader_CFErr2Down = new BTagCalibrationReader(&calib_csvv2, BTagEntry::OP_RESHAPING, "iterativefit", "down_cferr2");
    }

    /////////////////////////////////////////////////
    //                   Lepton SF                 //
    /////////////////////////////////////////////////
    cout << "Loading leptonsf files ..." << endl;
    MuonSFWeight* muonSFWeightID_GH;
    MuonSFWeight* muonSFWeightID_BCDEF;
    MuonSFWeight* muonSFWeightIso_GH;
    MuonSFWeight* muonSFWeightIso_BCDEF;
    MuonSFWeight* muonSFWeightTrig_BCDEF;
    MuonSFWeight* muonSFWeightTrig_GH;

    ElectronSFWeight* electronSFWeight;
    ElectronSFWeight* electronSFWeightReco;
    if(bLeptonSF) {
        if(Muon) {
            muonSFWeightID_GH = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/20170413/IDEfficienciesAndSF_GH.root",
                                              "MC_NUM_LooseID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio", true, false, false);
            muonSFWeightIso_GH = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/20170413/IsoEfficienciesAndSF_GH.root",
                                              "LooseISO_LooseID_pt_eta/abseta_pt_ratio", true, false, false);
            muonSFWeightID_BCDEF = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/20170413/IDEfficienciesAndSF_BCDEF.root",
                                              "MC_NUM_LooseID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio", true, false, false);
            muonSFWeightIso_BCDEF = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/20170413/IsoEfficienciesAndSF_BCDEF.root",
                                              "LooseISO_LooseID_pt_eta/abseta_pt_ratio", true, false, false);
            muonSFWeightTrig_BCDEF = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/SingleMuonTrigger_EfficienciesAndSF_RunsBCDEF.root",
                                              "IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio", true, false, false);
            muonSFWeightTrig_GH = new MuonSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/MuonSF/SingleMuonTrigger_EfficienciesAndSF_RunsGH.root",
                                              "IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio", true, false, false);
        }
        if(Electron) {
            electronSFWeight = new ElectronSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/ElectronSF/20170413/egammaEffi.txt_EGM2D_IDcutbLoose_20170413.root",
                    "EGamma_SF2D", true, false, false);
            electronSFWeightReco = new ElectronSFWeight("../TopTreeAnalysisBase/Calibrations/LeptonSF/ElectronSF/20170413/egammaEffi.txt_EGM2D_reco_20170413.root",
                    "EGamma_SF2D", true, false, false, true);
        }
    }

    /////////////////////////////////////////////////
    //               Pu reweighting                //
    /////////////////////////////////////////////////
    cout << "Loading lumiweights files..." << endl;
    LumiReWeighting LumiWeights;
    LumiReWeighting LumiWeights_up;
    LumiReWeighting LumiWeights_down;

    LumiWeights = LumiReWeighting(
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/MCPileup_Summer16.root",
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/pileup_2016Data80X_Run271036-284044Cert__Full2016DataSet.root",
        "pileup", "pileup");
    LumiWeights_up = LumiReWeighting(
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/MCPileup_Summer16.root",
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/pileup_2016Data80X_Run271036-284044Cert__Full2016DataSet_sysPlus.root",
        "pileup", "pileup");
    LumiWeights_down = LumiReWeighting(
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/MCPileup_Summer16.root",
        "../TopTreeAnalysisBase/Calibrations/PileUpReweighting/pileup_2016Data80X_Run271036-284044Cert__Full2016DataSet_sysMinus.root",
        "pileup", "pileup");

    //////////////////////////////////////////////////
    //               JEC factors                    //
    //////////////////////////////////////////////////
    cout << "Loading JEC files..." << endl;
    vector<JetCorrectorParameters> vCorrParam;
    string pathCalJEC = "../TopTreeAnalysisBase/Calibrations/JECFiles/";
    JetCorrectionUncertainty *jecUnc;

    if(dataSetName.find("Data_Run2016B")!=string::npos || dataSetName.find("Data_Run2016C")!=string::npos || dataSetName.find("Data_Run2016D")!=string::npos) {
        JetCorrectorParameters *L1JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt");
        vCorrParam.push_back(*L1JetCorPar);
        JetCorrectorParameters *L2JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK4PFchs.txt");
        vCorrParam.push_back(*L2JetCorPar);
        JetCorrectorParameters *L3JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt");
        vCorrParam.push_back(*L3JetCorPar);
        JetCorrectorParameters *L2L3ResJetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt");
        vCorrParam.push_back(*L2L3ResJetCorPar);
        jecUnc = new JetCorrectionUncertainty(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_Uncertainty_AK4PFchs.txt");
    } else if(dataSetName.find("Data_Run2016E")!=string::npos || dataSetName.find("Data_Run2016F")!=string::npos) {
        JetCorrectorParameters *L1JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt");
        vCorrParam.push_back(*L1JetCorPar);
        JetCorrectorParameters *L2JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK4PFchs.txt");
        vCorrParam.push_back(*L2JetCorPar);
        JetCorrectorParameters *L3JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt");
        vCorrParam.push_back(*L3JetCorPar);
        JetCorrectorParameters *L2L3ResJetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt");
        vCorrParam.push_back(*L2L3ResJetCorPar);
        jecUnc = new JetCorrectionUncertainty(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_Uncertainty_AK4PFchs.txt");
    } else if(dataSetName.find("Data_Run2016G")!=string::npos) {
        JetCorrectorParameters *L1JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt");
        vCorrParam.push_back(*L1JetCorPar);
        JetCorrectorParameters *L2JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK4PFchs.txt");
        vCorrParam.push_back(*L2JetCorPar);
        JetCorrectorParameters *L3JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt");
        vCorrParam.push_back(*L3JetCorPar);
        JetCorrectorParameters *L2L3ResJetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt");
        vCorrParam.push_back(*L2L3ResJetCorPar);
        jecUnc = new JetCorrectionUncertainty(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_Uncertainty_AK4PFchs.txt");
    } else if(dataSetName.find("Data_Run2016H")!=string::npos) {
        JetCorrectorParameters *L1JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt");
        vCorrParam.push_back(*L1JetCorPar);
        JetCorrectorParameters *L2JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK4PFchs.txt");
        vCorrParam.push_back(*L2JetCorPar);
        JetCorrectorParameters *L3JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt");
        vCorrParam.push_back(*L3JetCorPar);
        JetCorrectorParameters *L2L3ResJetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt");
        vCorrParam.push_back(*L2L3ResJetCorPar);
        jecUnc = new JetCorrectionUncertainty(pathCalJEC+"/Summer16_23Sep2016V4_DATA/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_Uncertainty_AK4PFchs.txt");
    } else {
        JetCorrectorParameters *L1JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt");
        vCorrParam.push_back(*L1JetCorPar);
        JetCorrectorParameters *L2JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt");
        vCorrParam.push_back(*L2JetCorPar);
        JetCorrectorParameters *L3JetCorPar = new JetCorrectorParameters(pathCalJEC+"/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt");
        vCorrParam.push_back(*L3JetCorPar);
        jecUnc = new JetCorrectionUncertainty(pathCalJEC+"/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_Uncertainty_AK4PFchs.txt");
    }


    JetTools* jetTools = new JetTools(vCorrParam, jecUnc, true);



    JetCombiner* jetCombiner = new JetCombiner(TrainMVA, Luminosity, datasets, MVAmethod, false, "", "_13TeV");
    cout << "Instantiated jet combiner ..." << endl;

    /////////////////////////////////
    //  Loop over Datasets
    /////////////////////////////////

    cout << "Find sample with equivalent lumi ..." << theDataset->EquivalentLumi() << endl;
    cout << "Rescale to an integrated luminosity of ..." << Luminosity << " pb^-1" << endl;


    // vector of objects
    cout << "Variable declaration ..." << endl;
    vector<TRootVertex*> vertex;
    vector<TRootMuon*> init_muons;
    vector<TRootElectron*> init_electrons;
    vector<TRootJet*> init_jets;
    vector<TRootJet*> init_fatjets;
    vector<TRootMET*> mets;

    // Global variable
    TRootEvent* event = 0;


    // Plots
    string pathPNG = "OUTPUT/FourTop" + postfix + channelpostfix;
    mkdir(pathPNG.c_str(), 0777);
    cout << "Making directory :" << pathPNG << endl;


    /////////////////////////////////
    // Loop on datasets
    /////////////////////////////////

    cout << "Looping over datasets ... " << datasets.size() << " datasets !" << endl;

    for(unsigned int d = 0; d < datasets.size(); d++) {
        cout << "Loading Dataset" << endl;
        treeLoader.LoadDataset(datasets[d], anaEnv); // open files and load dataset
        string previousFilename = "";
        int iFile = -1;
        bool nlo = false;
        dataSetName = datasets[d]->Name();

        //////////////////////////////////////////////
        // Steering based on DataSet Name	    //
        //////////////////////////////////////////////

        if(dataSetName.find("JERDown") != string::npos) {
            JERDown = true;
        } else if(dataSetName.find("JERUp") != string::npos) {
            JERUp = true;
        } else {
            JERNom = true;
        }
        if(dataSetName.find("JESDown") != string::npos) {
            JESDown = true;
        } else if(dataSetName.find("JESUp") != string::npos) {
            JESUp = true;
        }
        if(dataSetName.find("NLO") != std::string::npos)
            nlo = true;
        else
            nlo = false;
        if(nlo)
            cout << "NLO Dataset!" << endl;
        else
            cout << "LO Dataset!" << endl;

        ///////////////////////
        // booking triggers  //
        ///////////////////////

        trigger_mumu->bookTriggers(isData,dataSetName);
        trigger_muel->bookTriggers(isData,dataSetName);
        trigger_elel->bookTriggers(isData,dataSetName);
        trigger_mu->bookTriggers(isData,dataSetName);


        //////////////////////////////////////////////
        // Setup Date string and nTuple for output  //
        //////////////////////////////////////////////

        time_t t = time(0); // get time now
        struct tm* now = localtime(&t);
        int year = now->tm_year + 1900;
        int month = now->tm_mon + 1;
        int day = now->tm_mday;
        int hour = now->tm_hour;
        int min = now->tm_min;
        int sec = now->tm_sec;
        string year_str;
        string month_str;
        string day_str;
        string hour_str;
        string min_str;
        string sec_str;

        ostringstream convert; // stream used for the conversion
        convert << year;       // insert the textual representation of 'Number' in the characters in the stream
        year_str = convert.str();
        convert.str("");
        convert.clear();
        convert << month; // insert the textual representation of 'Number' in the characters in the stream
        month_str = convert.str();
        convert.str("");
        convert.clear();
        convert << day; // insert the textual representation of 'Number' in the characters in the stream
        day_str = convert.str();
        convert.str("");
        convert.clear();
        convert << hour; // insert the textual representation of 'Number' in the characters in the stream
        hour_str = convert.str();
        convert.str("");
        convert.clear();
        convert << min; // insert the textual representation of 'Number' in the characters in the stream
        min_str = convert.str();
        convert.str("");
        convert.clear();
        convert << day; // insert the textual representation of 'Number' in the characters in the stream
        sec_str = convert.str();
        convert.str("");
        convert.clear();

        string date_str = day_str + "_" + month_str + "_" + year_str;
        cout << "DATE STRING   " << date_str << endl;
        string channel_dir = "Craneens" + channelpostfix;
        string date_dir = channel_dir + "/Craneens" + date_str + "/";
        string cutflow_dir = channel_dir + "/Cutflow" + date_str + "/";
        int mkdirstatus = mkdir(channel_dir.c_str(), 0777);
        mkdirstatus = mkdir(date_dir.c_str(), 0777);
        mkdirstatus = mkdir(cutflow_dir.c_str(), 0777);

        string Ntupname = "Craneens" + channelpostfix + "/Craneens" + date_str + "/" + dataSetName + postfix + ".root";
        string Cuttupname = "Craneens" + channelpostfix + "/Cutflow" + date_str + "/" + dataSetName + ".root";
        string Ntuptitle = "Craneen_" + channelpostfix;
        string cutTuptitle = "Craneen_" + channelpostfix + "_CutFlow";

        TFile* tupfile = new TFile(Ntupname.c_str(), "RECREATE");
        TNtuple* tup = new TNtuple(Ntuptitle.c_str(), Ntuptitle.c_str(),
            "SFlepton_BCDEF:nJets:nFatJets:nWTags:nTopTags:nLtags:nMtags:nTtags:1stJetPt:2ndJetPt:3rdJetPt:4thJetPt:5thJetPt:6thJetPt:7thJetPt:8thJetPt:MET:HT:"
            "LeadingLepPt:LeadingLepEta:SubLeadingLepPt:dRLep:AbsSumCharge:LepFlavor:nLep:LeadingBJetPt:dRbb:"
            "HT2M:HTb:HTH:HTRat:topness:EventSph:EventCen:DiLepSph:DiLepCen:TopDiLepSph:TopDiLepCen:fnjetW:ttbar_flav:"
            "SFlepton_GH:SFbtagCSV:SFbtag_light:SFbtag_lightUp:SFbtag_lightDown:SFbtag_heavy:SFbtag_heavyUp:SFbtag_heavyDown:SFPU:SFPU_up:SFPU_down:"
            "btagWeightCSVLFUp:btagWeightCSVLFDown:btagWeightCSVHFUp:btagWeightCSVHFDown:btagWeightCSVHFStats1Up:btagWeightCSVHFStats1Down:"
            "btagWeightCSVHFStats2Up:btagWeightCSVHFStats2Down:btagWeightCSVLFStats1Up:btagWeightCSVLFStats1Down:"
            "btagWeightCSVLFStats2Up:btagWeightCSVLFStats2Down:btagWeightCSVCFErr1Up:btagWeightCSVCFErr1Down:btagWeightCSVCFErr2Up:btagWeightCSVCFErr2Down:"
            "SFalphaTune:SFtopPt:SFISR:ScaleFactor:PU:NormFactor:Luminosity:GenWeight:weight1:weight2:weight3:weight4:weight5:weight6:weight7:weight8:"
            "diLepMass:RunNumber:LumiBlock:EventNumber:TriJetMass:DiJetMass:LeadingLepPhi:SubLeadingLepPhi:1stJetPhi:hdampup:hdampdown:LeadingLepIso:SubLeadingLepIso:"
            "SFleptrig_BCDEF:SFleptrig_GH");
        TFile* cuttupfile = new TFile(Cuttupname.c_str(), "RECREATE");
        TNtuple* cuttup = new TNtuple(cutTuptitle.c_str(), cutTuptitle.c_str(),
            "initevent:trigger:isGoodPV:Lep1:Lep2:nJets2:nJets3:nJets4:nTags:HT");


        //////////////////////////////////////////////////
        // Loop on events
        //////////////////////////////////////////////////

        cout << "First try to read into dataset::potential tech. bugz::" << endl;
        unsigned int ending = datasets[d]->NofEvtsToRunOver();
        cout << "Number of total events in dataset = " << ending << endl;

        int previousRun = -1;
        int start = 0;
        int event_start = 0; 
        int event_end = ending;
        float BDTScore, MHT, MHTSig, STJet, muoneta, muonpt, electronpt, bjetpt, EventMass, EventMassX, SumJetMass,
            SumJetMassX, H, HX, HTHi, HTRat, HT, HTX, HTH, HTXHX, sumpx_X, sumpy_X, sumpz_X, sume_X, sumpx, sumpy,
            sumpz, sume, jetpt, PTBalTopEventX, PTBalTopSumJetX, PTBalTopMuMet, dRLep, dRbb, lepFlavor;

        if(debug) event_end = 10000; //artifical ending for debug
        cout << "Looping over events ..." << endl;
        cout << "Will run over " << (event_end - event_start) << " events..." << endl;
        cout << "Starting event ==== " << event_start << endl;
        if(event_end < event_start) {
            cout << "Starting event larger than number of events.  Exiting." << endl;
            return 1;
        }

        TRootRun* runInfos = new TRootRun();
        datasets[d]->runTree()->SetBranchStatus("runInfos*", 1);
        datasets[d]->runTree()->SetBranchAddress("runInfos", &runInfos);

        TopologyWorker* topologyW = new TopologyWorker(false);


        //////////////////////////////////////
        // Begin Event Loop
        //////////////////////////////////////

        for(unsigned int ievt = event_start; ievt < event_end; ievt++) {

            // define object containers
            vector<TRootElectron *> selectedElectrons, selectedLooseElectrons, selectedMediumElectrons, selectedTightElectrons, selectedExtraElectrons;
            vector<TRootElectron *> selectedLooseXElectrons, selectedMediumXElectrons, selectedPosTightElectrons, selectedNegTightElectrons, selectedNTightElectrons;
            vector<TRootMuon *> selectedMuons, selectedPosMuons, selectedNegMuons, selectedExtraMuons;
            vector<TRootPFJet *> selectedJets, selectedUncleanedJets, selectedLooseJets, MVASelJets1;
            vector<TRootSubstructureJet*> selectedFatJets;
            selectedElectrons.reserve(10);
            selectedMuons.reserve(10);

            BDTScore = -99999.0, MHT = 0., MHTSig = 0., muoneta = 0., muonpt = 0., electronpt = 0., bjetpt = 0.,
            STJet = 0., EventMass = 0., EventMassX = 0., SumJetMass = 0., SumJetMassX = 0., HTHi = 0., HTRat = 0;
            H = 0., HX = 0., HT = 0., HTX = 0., HTH = 0., HTXHX = 0., sumpx_X = 0., sumpy_X = 0., sumpz_X = 0.,
            sume_X = 0., sumpx = 0., sumpy = 0., sumpz = 0., sume = 0., jetpt = 0., PTBalTopEventX = 0.,
            PTBalTopSumJetX = 0., dRLep = 0, dRbb = 0, lepFlavor = 0;

            float centralWeight = 1, scaleUp = 1, scaleDown = 1, weight1 = 1, weight2 = 1, weight3 = 1, weight4 = 1, weight5 = 1, weight6 = 1, weight7 = 1, weight8 = 1, weight_hdamp_up = 1, weight_hdamp_dw = 1;

            if(debug) cout << "Event loop " << ievt << endl;
            if(ievt % 10000 == 0) {
                cout << "Processing the " << ievt << "th event, time = " << ((double)clock() - start) / CLOCKS_PER_SEC << " (" << 100 * (ievt - start) / (ending - start) << "%)" << flush << "\r" << endl;
            }
            float scaleFactor = 1.; // scale factor for the event
            float normfactor = 1.0;

            event = treeLoader.LoadEvent(ievt, vertex, init_muons, init_electrons, init_jets, init_fatjets, mets, debug); // load event

            float nvertices = vertex.size();
            datasets[d]->eventTree()->LoadTree(ievt);
            string currentFilename = datasets[d]->eventTree()->GetFile()->GetName();
            if(previousFilename != currentFilename) {
                previousFilename = currentFilename;
                iFile++;
                cout << "File changed!!! => " << currentFilename << endl;
            }
            int rBytes = datasets[d]->runTree()->GetEntry(iFile);
            int currentRun = event->runId();
            int currentEvent = event->eventId();
            int LumiBlock = event->lumiBlockId();

            /*if(isData)
            {
                if(!event->getHBHENoiseFilter() 
                    || !event->getHBHENoiseIsoFilter() 
                    || !event->getEEBadScFilter() 
                    || !event->getglobalTightHalo2016Filter()
                    || !event->getEcalDeadCellTriggerPrimitiveFilter() 
                    || !event->getPVFilter() 
                    || !event->getBadChCandFilter() 
                    || !event->getBadPFMuonFilter())
                continue;
            }
            else if(!isData)
            {
                if(!event->getHBHENoiseFilter()
                    || !event->getHBHENoiseIsoFilter()
                    || !event->getglobalTightHalo2016Filter()
                    || !event->getEcalDeadCellTriggerPrimitiveFilter()
                    || !event->getPVFilter()
                    || !event->getBadChCandFilter()
                    || !event->getBadPFMuonFilter())
                continue;
            }*/

            eventCount++;

            if(!isData){
                if(event->getWeight(1)!= -9999){
                    centralWeight = (event->getWeight(1))/(abs(event->originalXWGTUP()));  
                    weight1 = (event->getWeight(2))/(abs(event->originalXWGTUP()));                
                    weight2 = (event->getWeight(3))/(abs(event->originalXWGTUP()));                
                    weight3 = (event->getWeight(4))/(abs(event->originalXWGTUP()));                
                    weight4 = (event->getWeight(5))/(abs(event->originalXWGTUP()));                
                    weight5 = (event->getWeight(6))/(abs(event->originalXWGTUP()));                
                    weight6 = (event->getWeight(7))/(abs(event->originalXWGTUP()));                
                    weight7 = (event->getWeight(8))/(abs(event->originalXWGTUP()));                
                    weight8 = (event->getWeight(9))/(abs(event->originalXWGTUP()));                    
                } else if (event->getWeight(1001)!= -9999){
                    centralWeight = (event->getWeight(1001))/(abs(event->originalXWGTUP()));  
                    weight1 = (event->getWeight(1002))/(abs(event->originalXWGTUP()));                
                    weight2 = (event->getWeight(1003))/(abs(event->originalXWGTUP()));                
                    weight3 = (event->getWeight(1004))/(abs(event->originalXWGTUP()));                
                    weight4 = (event->getWeight(1005))/(abs(event->originalXWGTUP()));                
                    weight5 = (event->getWeight(1006))/(abs(event->originalXWGTUP()));                
                    weight6 = (event->getWeight(1007))/(abs(event->originalXWGTUP()));                
                    weight7 = (event->getWeight(1008))/(abs(event->originalXWGTUP()));                
                    weight8 = (event->getWeight(1009))/(abs(event->originalXWGTUP()));                    
                }
		if (event->getWeight(1001)!= -9999) {
			weight_hdamp_up = event->getWeight(5019)/fabs(event->originalXWGTUP());
			weight_hdamp_dw = event->getWeight(5010)/fabs(event->originalXWGTUP());
		}
            }

            float negweight = 0.0;
            if(nlo) {
                if(centralWeight < 0.0) {
                    negWeights++;
                    negweight =-1.0;
                } else {
                    negweight = 1.0;
                }
            }


            float rho = event->fixedGridRhoFastjetAll();
            if(debug) cout << "Rho: " << rho << endl;


            //////////////////////
            // Loading Gen jets //
            //////////////////////

            float numOfbb = 0;
            float numOfcc = 0;
            float numOfll = 0;
            float ttbar_flav = -1;

            vector<TRootGenJet*> genjets;
            if(!isData) {
                genjets = treeLoader.LoadGenJet(ievt, false);   // loading GenJets as I need them for JER
                if(debug) cout << "Nof genjets " << genjets.size() << endl;
            }
            if(dataSetName.find("TTJets") != string::npos || dataSetName.find("TTScale") != string::npos) {
                // treeLoader.LoadMCEvent(ievt, 0, mcParticles_flav, false);
                for(int gj = 0; gj < genjets.size(); gj++) {
                    if(genjets[gj]->BHadron().Et() > 0) {
                        if(genjets[gj]->Pt() > 20 && abs(genjets[gj]->Eta() < 2.5))
                            numOfbb++;
                    }
                    if(genjets[gj]->CHadron().Et() > 0) {
                        if(genjets[gj]->Pt() > 20 && abs(genjets[gj]->Eta() < 2.5))
                            numOfcc++;
                    }
                }
            }

            if(numOfbb > 2) {
                ttbar_flav = 2;
            } else if(numOfcc > 0) {
                ttbar_flav = 1;
            } else {
                ttbar_flav = 0;
            }

            ///////////////////////////////////////////
            //  Trigger
            ///////////////////////////////////////////

            bool trigged = false;
            bool trigged_mumu = false;
            bool trigged_muel = false;
            bool trigged_elel = false;
            bool trigged_mu = false;


            if(Muon && !Electron){
                trigger_mumu->checkAvail(currentRun, datasets, d, &treeLoader, event, verbose);
                trigged_mumu = trigger_mumu->checkIfFired();
                trigger_mu->checkAvail(currentRun, datasets, d, &treeLoader, event, verbose);
                trigged_mu = trigger_mu->checkIfFired();
            } else if(Muon && Electron){
                trigger_muel->checkAvail(currentRun, datasets, d, &treeLoader, event, verbose);
                trigged_muel = trigger_muel->checkIfFired();
                trigger_mu->checkAvail(currentRun, datasets, d, &treeLoader, event, verbose);
                trigged_mu = trigger_mu->checkIfFired();
            } else if(!Muon && Electron){
                trigger_elel->checkAvail(currentRun, datasets, d, &treeLoader, event, verbose);
                trigged_elel = trigger_elel->checkIfFired();
            }

            bool mmdataset = dName.find("DoubleM")!=string::npos;
            bool medataset = dName.find("MuonEG")!=string::npos;
            bool eedataset = dName.find("DoubleE")!=string::npos;
            bool mdataset = dName.find("SingleM")!=string::npos;

            bool MM = trigged_mumu;
            bool ME = trigged_muel;
            bool EE = trigged_elel;
            bool M = trigged_mu;


            if(isData){
                if(Muon && !Electron){
                    if(mmdataset) trigged = MM;
                    else if(mdataset) trigged = M && !MM;
                    else {cerr << "Error" << endl; exit(1);}
                }
                else if(Muon && Electron){
                    if(medataset) trigged = ME;
                    else if(mdataset) trigged = M && !ME;
                    else {cerr << "Error" << endl; exit(1);}
                }
                else if(!Muon && Electron){
                    if(eedataset) trigged = EE;
                    else {cerr << "Error" << endl; exit(1);}
                }
                else {cerr << "Error" << endl; exit(1);}
            } else {
                if(Muon && !Electron) trigged = MM || M; 
                else if(Muon && Electron) trigged = ME || M;
                else if(!Muon && Electron) trigged = EE;
                else {cerr << "Error" << endl; exit(1);}
            }
 
            if(debug) cout << "triggered? Y/N?  " << trigged << endl;
 

            //////////////////////////////////////
            ///  Jet Energy Scale Corrections  ///
            //////////////////////////////////////

            if(applyJER && !isData) {
                if(JERNom) {
                    jetTools->correctJetJER(init_jets, genjets, mets[0], "nominal", false);
                } else if(JERDown)
                    jetTools->correctJetJER(init_jets, genjets, mets[0], "minus", false);
                else if(JERUp)
                    jetTools->correctJetJER(init_jets, genjets, mets[0], "plus", false);
            }

            if(JESDown)
                jetTools->correctJetJESUnc(init_jets, "minus", 1);
            else if(JESUp)
                jetTools->correctJetJESUnc(init_jets, "plus", 1);

            if(applyJEC) {
                jetTools->correctJets(init_jets, event->fixedGridRhoFastjetAll(), isData);
            }

            /////////////////////////////////////
            // Init Jet Plots for ISR reweight //
            /////////////////////////////////////

            int nInitJets = 0;
            float ISRsf = 1.0;
            for(int iJet = 0; iJet < init_jets.size(); iJet++) {
                if(init_jets[iJet]->genParticleIndex() == -1) {
                    nInitJets++;
                }
            }


            //////////////////////////////////
            //      Event selection         //
            //////////////////////////////////

            // Declare selection instance
            Run2Selection selection(init_jets, init_fatjets, init_muons, init_electrons, mets, rho);

            // Define object selection cuts
            if(Muon && Electron && dilepton) {
                selectedMuons = selection.GetSelectedMuons(20, 2.4, 0.15, "Loose", "Summer16");
                selectedElectrons = selection.GetSelectedElectrons(25, 2.4, "Loose", "Spring16_80X", true, true);
            } if(Muon && !Electron && dilepton) {
                selectedMuons = selection.GetSelectedMuons(20, 2.4, 0.15, "Loose", "Summer16");
                selectedElectrons = selection.GetSelectedElectrons(25, 2.4, "Loose", "Spring16_80X", true, true);
            } if(!Muon && Electron && dilepton) {
                selectedMuons = selection.GetSelectedMuons(20, 2.4, 0.15, "Loose", "Summer16");
                selectedElectrons = selection.GetSelectedElectrons(25, 2.4, "Loose", "Spring16_80X", true, true);
            }
            selectedUncleanedJets = selection.GetSelectedJets(30, 2.4, true, "Loose");
            selectedLooseJets = selection.GetSelectedJets();
            selectedFatJets = selection.GetSelectedFatJets();
            selectedJets.clear();

            for(int unj = 0; unj < selectedUncleanedJets.size(); unj++) {
                bool isClose = false;
                for(int e = 0; e < selectedElectrons.size(); e++) {
                    if(selectedElectrons[e]->DeltaR(*selectedUncleanedJets[unj]) < 0.3)
                        isClose = true;
                }
                for(int mu = 0; mu < selectedMuons.size(); mu++) {
                    if(selectedMuons[mu]->DeltaR(*selectedUncleanedJets[unj]) < 0.3)
                        isClose = true;
                }
                if(!isClose)
                    selectedJets.push_back(selectedUncleanedJets[unj]);
            }

            vector<TRootJet*> selectedLBJets;
            vector<TRootJet*> selectedMBJets;
            vector<TRootJet*> selectedTBJets;
            vector<TRootPFJet*> selectedLightJets;
            vector<TRootPFJet*> selectedHeavyJets;

            for(int selj = 0; selj < selectedJets.size(); selj++) {
                if(abs(selectedJets[selj]->hadronFlavour()) >= 4)
                    selectedHeavyJets.push_back(selectedJets[selj]);
                else
                    selectedLightJets.push_back(selectedJets[selj]);
            }

            int nMu = 0, nEl = 0, nLooseIsoMu = 0, nLep = 0, nLooseEl = 0, nMedEl = 0, nTightEl = 0;
            nMu = selectedMuons.size();     // Number of Muons in Event
            nEl = selectedElectrons.size(); // Number of Loose Electrons in Event

            vector<TLorentzVector> selectedMuonsTLV_JC, selectedElectronsTLV_JC, selectedLooseIsoMuonsTLV;
            vector<TLorentzVector> mcParticlesTLV, selectedJetsTLV, mcMuonsTLV, mcPartonsTLV;
            vector<TRootMCParticle*> mcParticlesMatching_;
            vector<int> mcMuonIndex, mcPartonIndex;
            JetPartonMatching muonMatching, jetMatching;

            ////////////////////////////////////
            // Preselection Lepton Operations //
            ////////////////////////////////////

            float diLepMass = 0, diMuMass = 0;
            bool ZVeto = false, sameCharge = false;
            float ZMass = 91, ZMassWindow = 15;
            int cj1 = 0, cj2 = 0, lidx1 = 0, lidx2 = 0;
            TLorentzVector lep1, lep2, diLep;
            vector<pair<float, pair<TRootLepton*, TRootLepton*> > > LeptonPairs;

            for(int selmu = 0; selmu < selectedMuons.size(); selmu++) {
                selectedMuonsTLV_JC.push_back(*selectedMuons[selmu]);
            }

            //sort(selectedPosMuons.begin(), selectedPosMuons.end(), HighestPt());
            //sort(selectedNegMuons.begin(), selectedNegMuons.end(), HighestPt());

            for(int selel = 0; selel < selectedElectrons.size(); selel++) {
                selectedElectronsTLV_JC.push_back(*selectedElectrons[selel]);
            }
            for(Int_t seljet = 0; seljet < selectedJets.size(); seljet++) {
                selectedJetsTLV.push_back(*selectedJets[seljet]);
            }

            if(nMu == 2 && nEl == 0 && Muon && !Electron) // Di-Muon Selection
            {
                lep1 = selectedMuonsTLV_JC[0];
                lep2 = selectedMuonsTLV_JC[1];
                if(selectedMuons[0]->charge() == selectedMuons[1]->charge())
                    sameCharge = true;
                nLep = nMu;
            } else if(nEl == 2 && nMu == 0 && Electron && !Muon) // Di-Electron Selection criteria
            {
                lep1 = selectedElectronsTLV_JC[0];
                lep2 = selectedElectronsTLV_JC[1];
                if(selectedElectrons[0]->charge() == selectedElectrons[1]->charge())
                    sameCharge = true;
                nLep = nEl;
            } else if(nEl == 1 && nMu == 1 && Electron && Muon) // Muon-Electron Selection
            {
                lep1 = selectedMuonsTLV_JC[0];
                lep2 = selectedElectronsTLV_JC[0];
                if(selectedMuons[0]->charge() == selectedElectrons[0]->charge())
                    sameCharge = true;
                nLep = nMu + nEl;
            }

            if(!sameCharge && nLep==2) {
                diLep = lep1 + lep2;
                diLepMass = diLep.M();
                for(Int_t seljet = 0; seljet < selectedJetsTLV.size(); seljet++) {
                    if(lep1.DeltaR(selectedJetsTLV[seljet]) < lep1.DeltaR(selectedJetsTLV[cj1]))
                        cj1 = seljet;
                    if(lep2.DeltaR(selectedJetsTLV[seljet]) < lep2.DeltaR(selectedJetsTLV[cj2]))
                        cj2 = seljet;
                }
                if(selectedJetsTLV.size() > 0) {
                    if(lep1.DeltaPhi(selectedJetsTLV[cj1]) < 0.05) {
                        float HOverE = (selectedJets[cj1]->chargedHadronEnergyFraction() + selectedJets[cj1]->neutralHadronEnergyFraction()) /
                                       (selectedJets[cj1]->chargedEmEnergyFraction() + selectedJets[cj1]->neutralEmEnergyFraction());
                    }
                    if(lep2.DeltaPhi(selectedJetsTLV[cj2]) < 0.05) {
                        float HOverE = (selectedJets[cj2]->chargedHadronEnergyFraction() +selectedJets[cj2]->neutralHadronEnergyFraction()) /
                                       (selectedJets[cj2]->chargedEmEnergyFraction() + selectedJets[cj2]->neutralEmEnergyFraction());
                    }
              }
            }

            float temp_HT = 0., HTb = 0.;
            int jet_flavor;
            float eff = 1;
            double JetPt, JetEta;

            for(Int_t seljet = 0; seljet < selectedJets.size(); seljet++) {
                jet_flavor = selectedJets[seljet]->hadronFlavour();
                JetPt = selectedJets[seljet]->Pt();
                JetEta = selectedJets[seljet]->Eta();
                temp_HT += JetPt;
                if(selectedJets[seljet]->btag_combinedInclusiveSecondaryVertexV2BJetTags() > 0.5426) {
                    selectedLBJets.push_back(selectedJets[seljet]);
                    if(selectedJets[seljet]->btag_combinedInclusiveSecondaryVertexV2BJetTags() > 0.8484) {
                        selectedMBJets.push_back(selectedJets[seljet]);
                        if(selectedJets[seljet]->btag_combinedInclusiveSecondaryVertexV2BJetTags() > 0.9535) {
                            selectedTBJets.push_back(selectedJets[seljet]);
                        }
                    }
                //} else {
                  //  selectedLightJets.push_back(selectedJets[seljet]);
                }
            }

            float nJets = selectedJets.size();
            float nMtags = selectedMBJets.size();
            float nLtags = selectedLBJets.size();
            float nTtags = selectedTBJets.size();
            float nFatJets = selectedFatJets.size();

            if(debug)
                cout << " applying baseline event selection for cut table..." << endl;
            bool isGoodPV = selection.isPVSelected(vertex, 4, 24., 2);
            if(debug)
                cout << "PrimaryVertexBit: " << isGoodPV << " TriggerBit: " << trigged << endl;

            int cfTrigger = 0, cfPV = 0, cfLep1 = 0, cfLep2 = 0, cfJets2 = 0, cfJets3 = 0, cfJets4 = 0, cfTags = 0, cfHT = 0, cfAll = 1;
            if(Muon && Electron && dilepton) 
            {
                if(trigged) {
                    cfTrigger = 1;
                    if(isGoodPV) {
                        cfPV = 1;
                        if(nMu == 1) {
                            cfLep1 = 1;
                            if(nEl == 1) {
                                cfLep2 = 1;
                                if(nJets >= 2) {
                                    cfJets2 = 1;
                                    if(nJets >= 3) {
                                        cfJets3 = 1;
                                        if(nJets >= 4) {
                                            cfJets4 = 1;
                                            if(nMtags >= 2) {
                                                cfTags = 1;
                                                if(temp_HT >= 500) {
                                                    cfHT = 1;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(Muon && !Electron && dilepton) 
            {
                if(!sameCharge && (diLepMass < 20 || (diLepMass > (ZMass - ZMassWindow) && diLepMass < (ZMass + ZMassWindow))))
                    ZVeto = true;
                if(trigged) {
                    cfTrigger = 1;
                    if(isGoodPV) {
                        cfPV = 1;
                        if(nMu == 2 && nEl == 0 && !sameCharge) {
                            cfLep1 = 1;
                            if(!ZVeto) {
                                cfLep2 = 1;
                                if(nJets >= 2) {
                                    cfJets2 = 1;
                                    if(nJets >= 3) {
                                        cfJets3 = 1;
                                        if(nJets >= 4) {
                                            cfJets4 = 1;
                                            if(nMtags >= 2) {
                                                cfTags = 1;
                                                if(temp_HT >= 500) {
                                                    cfHT = 1;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(!Muon && Electron && dilepton) 
            {
                if(!sameCharge && (diLepMass < 20 || (diLepMass > (ZMass - ZMassWindow) && diLepMass < (ZMass + ZMassWindow))))
                    ZVeto = true;
                if(trigged) {
                    cfTrigger = 1;
                    if(isGoodPV) {
                        cfPV = 1;
                        if(nEl == 2 && nMu == 0  && !sameCharge) {
                            cfLep1 = 1;
                            if(!ZVeto) {
                                cfLep2 = 1;
                                if(nJets >= 2) {
                                    cfJets2 = 1;
                                    if(nJets >= 3) {
                                        cfJets3 = 1;
                                        if(nJets >= 4) {
                                            cfJets4 = 1;
                                            if(nMtags >= 2) {
                                                cfTags = 1;
                                                if(temp_HT >= 500) {
                                                    cfHT = 1;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            cuttup->Fill(cfAll, cfTrigger, cfPV, cfLep1, cfLep2, cfJets2, cfJets3, cfJets4, cfTags, cfHT);

            /////////////////////////////////
            // Applying baseline selection //
            /////////////////////////////////

            if(!trigged) continue;
            trigCount++;
            if(!isGoodPV) continue;

            if(debug)
                cout << " applying baseline event selection... nMu = " << nMu << " nEl = " << nEl << " ZVeto: " << ZVeto << " sameCharge: " << sameCharge << endl;

            if(Muon && Electron && dilepton) {
                if(!(nMu == 1 && nEl == 1 && !sameCharge))
                    continue; 
                lepFlavor = 2;
            } else if(Muon && !Electron && dilepton) {
                if(!(nMu == 2 && nEl == 0 && !ZVeto && !sameCharge))
                    continue; 
                lepFlavor = 1;
            } else if(!Muon && Electron && dilepton) {
                if(!(nMu == 0 && nEl == 2 && !ZVeto && !sameCharge))
                    continue; 
                lepFlavor = 3;
            } else {
                cerr << "Correct Channel not selected." << endl;
                exit(1);
            }

            if(debug)
                cout << "HT: " << temp_HT << " nJets: " << nJets << " nMTags: " << nMtags << endl;
            vector<TLorentzVector*> selectedLeptonTLV_JC;

            if(dilepton && Muon && Electron) {
                if(!(nJets >= 2 && nMtags >= 2)) continue;
                if(!(nJets >= 4)) continue;
                if(!(temp_HT >= 500)) continue;
                selectedLeptonTLV_JC.push_back(selectedMuons[0]);
            } else if(dilepton && Muon && !Electron) {
                if(!(nJets >= 2 && nMtags >= 2)) continue;
                if(!(nJets >= 4)) continue;
                if(!(temp_HT >= 500)) continue;
                selectedLeptonTLV_JC.push_back(selectedMuons[0]);
            } else if(dilepton && !Muon && Electron) {
                if(!(nJets >= 2 && nMtags >= 2)) continue;
                if(!(nJets >= 4)) continue;
                if(!(temp_HT >= 500)) continue;
                selectedLeptonTLV_JC.push_back(selectedElectrons[0]);
            }
            if(debug)
                cout << "Selection Passed." << endl;
            passed++;



            /////////////////////////////////////////////////
            //               Pu reweighting                //
            /////////////////////////////////////////////////

            float lumiWeight, lumiWeight_up, lumiWeight_down;
            if(isData) {
                lumiWeight = 1;
                lumiWeight_up = 1;
                lumiWeight_down = 1;
            } else {
                // lumiWeight = LumiWeights.ITweight( vertex.size() );
                lumiWeight = LumiWeights.ITweight((int)event->nTruePU());
                lumiWeight_up = LumiWeights_up.ITweight((int)event->nTruePU());
                lumiWeight_down = LumiWeights_down.ITweight((int)event->nTruePU());
            }
            //if(lumiWeight < 0.2) cout<<"PU:  "<<(int)event->nTruePU()<<"    LUMI WEIGHT:    "<<lumiWeight<<"!"<<endl;

            /////////////////////////////////////////////////
            //                    bTag SF                  //
            /////////////////////////////////////////////////

            float bTagEff(1);
            float bTagEffUp(1);
            float bTagEffDown(1);
            if(fillingbTagHistos) {
                if(bTagReweight && !isData) {
                    // get btag weight info
                    for(int jetbtag = 0; jetbtag < selectedJets.size(); jetbtag++) {
                        float jetpt = selectedJets[jetbtag]->Pt();
                        float jeteta = selectedJets[jetbtag]->Eta();
                        float jetdisc = selectedJets[jetbtag]->btag_combinedInclusiveSecondaryVertexV2BJetTags();
                        BTagEntry::JetFlavor jflav;
                        int jetpartonflav = std::abs(selectedJets[jetbtag]->hadronFlavour());
                        if(debug)
                            cout << "parton flavour: " << jetpartonflav << "  jet eta: " << jeteta << " jet pt: " << jetpt << "  jet disc: " << jetdisc << endl;
                        if(jetpartonflav == 5) {
                            jflav = BTagEntry::FLAV_B;
                        } else if(jetpartonflav == 4) {
                            jflav = BTagEntry::FLAV_C;
                        } else {
                            jflav = BTagEntry::FLAV_UDSG;
                        }
                        bTagEff = bTagReader->eval(jflav, jeteta, jetpt, jetdisc);
                        bTagEffUp = bTagReaderUp->eval(jflav, jeteta, jetpt, jetdisc);
                        bTagEffDown = bTagReaderDown->eval(jflav, jeteta, jetpt, jetdisc);

                        if(debug) cout << "btag efficiency = " << bTagEff << endl;
                    }
                    btwt->FillMCEfficiencyHistos(selectedJets);
                    btwtUp->FillMCEfficiencyHistos(selectedJets);
                    btwtDown->FillMCEfficiencyHistos(selectedJets);
                }
            }

            if (debug) cout<<"get MC Event Weight for btag"<<endl;
            float btagWeight = 1;
            float btagWeightCSV = 1;
            float btagWeightCSVLFUp = 1;
            float btagWeightCSVLFDown = 1;
            float btagWeightCSVHFUp = 1;
            float btagWeightCSVHFDown = 1;
            float btagWeightCSVHFStats1Up = 1;
            float btagWeightCSVHFStats1Down = 1;
            float btagWeightCSVHFStats2Up = 1;
            float btagWeightCSVHFStats2Down = 1;            
            float btagWeightCSVLFStats1Up = 1;
            float btagWeightCSVLFStats1Down = 1;
            float btagWeightCSVLFStats2Up = 1;
            float btagWeightCSVLFStats2Down = 1; 
            float btagWeightCSVCFErr1Up = 1;
            float btagWeightCSVCFErr1Down = 1;
            float btagWeightCSVCFErr2Up = 1;
            float btagWeightCSVCFErr2Down = 1; 

            float btagWeight_light=1;
            float btagWeight_lightUp = 1;
            float btagWeight_lightDown = 1;
            float btagWeight_heavy=1;
            float btagWeight_heavyUp = 1;
            float btagWeight_heavyDown = 1;            
            if(bTagReweight && !isData){
                if(!fillingbTagHistos){
                    btagWeight_light =  btwt->getMCEventWeight(selectedLightJets, false);
                    btagWeight_lightUp =  btwtUp->getMCEventWeight(selectedLightJets, false);
                    btagWeight_lightDown =  btwtDown->getMCEventWeight(selectedLightJets, false);
                    btagWeight_heavy =  btwt->getMCEventWeight(selectedHeavyJets, false);
                    btagWeight_heavyUp =  btwtUp->getMCEventWeight(selectedHeavyJets, false);
                    btagWeight_heavyDown =  btwtDown->getMCEventWeight(selectedHeavyJets, false);                    
                }
                
                if(debug) cout<<"btag weight "<<btagWeight_light<<"  btag weight Up "<<btagWeight_lightUp<<"   btag weight Down "<<btagWeight_lightDown<<endl;
            }

            //csv discriminator reweighting
            if(bTagCSVReweight && !isData){
            //get btag weight info
                for(int jetbtag = 0; jetbtag<selectedJets.size(); jetbtag++){
                    float bTagEff_LFUp, bTagEff_LFDown, bTagEff_HFUp, bTagEff_HFDown, bTagEff_HFStats1Up, bTagEff_HFStats1Down, bTagEff_HFStats2Up,
                          bTagEff_HFStats2Down, bTagEff_LFStats1Up, bTagEff_LFStats1Down, bTagEff_LFStats2Up, bTagEff_LFStats2Down, bTagEff_CFErr1Up, 
                          bTagEff_CFErr1Down, bTagEff_CFErr2Up, bTagEff_CFErr2Down;
                    float jetpt = selectedJets[jetbtag]->Pt();
                    float jeteta = selectedJets[jetbtag]->Eta();
                    float jetdisc = selectedJets[jetbtag]->btag_combinedInclusiveSecondaryVertexV2BJetTags();
                    bool isBFlav = false;
                    bool isLFlav = false;
                    bool isCFlav = false;
                    if(jetdisc<0.0) jetdisc = -0.05;
                    if(jetdisc>1.0) jetdisc = 1.0;
                    BTagEntry::JetFlavor jflav;
                    int jethadronflav = std::abs(selectedJets[jetbtag]->hadronFlavour());
                    if(debug) cout<<"hadron flavour: "<<jethadronflav<<"  jet eta: "<<jeteta<<" jet pt: "<<jetpt<<"  jet disc: "<<jetdisc<<endl;
                    if(jethadronflav == 5){
                        jflav = BTagEntry::FLAV_B;
                        isBFlav =true;
                    }
                    else if(jethadronflav == 4){
                        jflav = BTagEntry::FLAV_C;
                        isCFlav=true;
                    }
                    else{
                        jflav = BTagEntry::FLAV_UDSG;
                        isLFlav = true;
                    }
                    if( dataSetName.find("JESUp") != string::npos && !isCFlav ){
                        bTagEff = reader_JESUp->eval(jflav, jeteta, jetpt, jetdisc);
                    } else if( dataSetName.find("JESDown") != string::npos && !isCFlav ){
                        bTagEff = reader_JESDown->eval(jflav, jeteta, jetpt, jetdisc);
                    } else {
                        bTagEff = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    }
                    if( isBFlav ) bTagEff_LFUp = reader_LFUp->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isBFlav ) bTagEff_LFDown = reader_LFDown->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isBFlav ) bTagEff_HFStats1Up = reader_HFStats1Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isBFlav ) bTagEff_HFStats1Down = reader_HFStats1Down->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isBFlav ) bTagEff_HFStats2Up = reader_HFStats2Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isBFlav ) bTagEff_HFStats2Down = reader_HFStats2Down->eval(jflav, jeteta, jetpt, jetdisc);

                    if( isLFlav ) bTagEff_HFUp = reader_HFUp->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isLFlav ) bTagEff_HFDown = reader_HFDown->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isLFlav ) bTagEff_LFStats1Up = reader_LFStats1Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isLFlav ) bTagEff_LFStats1Down = reader_LFStats1Down->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isLFlav ) bTagEff_LFStats2Up = reader_LFStats2Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isLFlav ) bTagEff_LFStats2Down = reader_LFStats2Down->eval(jflav, jeteta, jetpt, jetdisc);

                    if( isCFlav ) bTagEff_CFErr1Up = reader_CFErr1Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isCFlav ) bTagEff_CFErr1Down = reader_CFErr1Down->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isCFlav ) bTagEff_CFErr2Up = reader_CFErr2Up->eval(jflav, jeteta, jetpt, jetdisc);
                    if( isCFlav ) bTagEff_CFErr2Down = reader_CFErr2Down->eval(jflav, jeteta, jetpt, jetdisc);
                    
                    //If jet is not the appropriate flavor for that systematic, use the nominal reader so that all weights will be on the same jet multiplicity footing.
                    if( !isBFlav ) bTagEff_LFUp = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isBFlav ) bTagEff_LFDown = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isBFlav ) bTagEff_HFStats1Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isBFlav ) bTagEff_HFStats1Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isBFlav ) bTagEff_HFStats2Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isBFlav ) bTagEff_HFStats2Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);

                    if( !isLFlav ) bTagEff_HFUp = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isLFlav ) bTagEff_HFDown = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isLFlav ) bTagEff_LFStats1Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isLFlav ) bTagEff_LFStats1Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isLFlav ) bTagEff_LFStats2Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isLFlav ) bTagEff_LFStats2Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);

                    if( !isCFlav ) bTagEff_CFErr1Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isCFlav ) bTagEff_CFErr1Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isCFlav ) bTagEff_CFErr2Up = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    if( !isCFlav ) bTagEff_CFErr2Down = reader_csvv2->eval(jflav, jeteta, jetpt, jetdisc);
                    
                    btagWeightCSVLFUp *= bTagEff_LFUp;
                    btagWeightCSVLFDown *= bTagEff_LFDown;
                    btagWeightCSVHFUp *= bTagEff_HFUp;
                    btagWeightCSVHFDown *= bTagEff_HFDown;
                    btagWeightCSVHFStats1Up *= bTagEff_HFStats1Up;
                    btagWeightCSVHFStats1Down *= bTagEff_HFStats1Down;
                    btagWeightCSVHFStats2Up *= bTagEff_HFStats2Up;
                    btagWeightCSVHFStats2Down *= bTagEff_HFStats2Down;            
                    btagWeightCSVLFStats1Up *= bTagEff_LFStats1Up;
                    btagWeightCSVLFStats1Down *= bTagEff_LFStats1Down;
                    btagWeightCSVLFStats2Up *= bTagEff_LFStats2Up;
                    btagWeightCSVLFStats2Down *= bTagEff_LFStats2Down; 
                    btagWeightCSVCFErr1Up *= bTagEff_CFErr1Up;
                    btagWeightCSVCFErr1Down *= bTagEff_CFErr1Down;
                    btagWeightCSVCFErr2Up *= bTagEff_CFErr2Up;
                    btagWeightCSVCFErr2Down *= bTagEff_CFErr2Down; 

                    btagWeightCSV*=bTagEff;
             
                    if(debug) cout<<"btag efficiency = "<<bTagEff<<endl;       
                }      


            }

            ///////////////////////////
            //  Lepton Scale Factors //
            ///////////////////////////

            float fleptonSF1 = 1, fleptonSF2 = 1;
            float fleptonSF_GH = 1, fleptonSF_BCDEF = 1;
            float sfleptrig_GH = 1, sfleptrig_BCDEF = 1;
            if(bLeptonSF && !isData) {
                if(Muon && !Electron && nMu == 2 && nEl == 0) {
                    fleptonSF1 = muonSFWeightID_GH->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0) * muonSFWeightIso_GH->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
                    fleptonSF2 = muonSFWeightID_GH->at(selectedMuons[1]->Eta(), selectedMuons[1]->Pt(), 0) * muonSFWeightIso_GH->at(selectedMuons[1]->Eta(), selectedMuons[1]->Pt(), 0);
                    fleptonSF_GH = fleptonSF1 * fleptonSF2;
                    fleptonSF1 = muonSFWeightID_BCDEF->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0) * muonSFWeightIso_BCDEF->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
                    fleptonSF2 = muonSFWeightID_BCDEF->at(selectedMuons[1]->Eta(), selectedMuons[1]->Pt(), 0) * muonSFWeightIso_BCDEF->at(selectedMuons[1]->Eta(), selectedMuons[1]->Pt(), 0);
                    fleptonSF_BCDEF = fleptonSF1 * fleptonSF2;
                } else if(nEl == 2 && nMu == 0 && Electron && !Muon)
                {
                    fleptonSF1 = electronSFWeight->at(selectedElectrons[0]->Eta(), selectedElectrons[0]->Pt(), 0) * electronSFWeightReco->at(selectedElectrons[0]->Eta(), selectedElectrons[0]->Pt(), 0);
                    fleptonSF2 = electronSFWeight->at(selectedElectrons[1]->Eta(), selectedElectrons[1]->Pt(), 0) * electronSFWeightReco->at(selectedElectrons[1]->Eta(), selectedElectrons[1]->Pt(), 0);
                    fleptonSF_GH = fleptonSF1 * fleptonSF2;
                    fleptonSF_BCDEF = fleptonSF1 * fleptonSF2;
                } else if(Electron && Muon && nEl == 1 && nMu == 1) {
                    fleptonSF1 = electronSFWeight->at(selectedElectrons[0]->Eta(), selectedElectrons[0]->Pt(), 0) * electronSFWeightReco->at(selectedElectrons[0]->Eta(), selectedElectrons[0]->Pt(), 0);
                    fleptonSF2 = muonSFWeightID_GH->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0) * muonSFWeightIso_GH->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
                    fleptonSF_GH = fleptonSF1 * fleptonSF2;
                    fleptonSF2 = muonSFWeightID_BCDEF->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0) * muonSFWeightIso_BCDEF->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
                    fleptonSF_BCDEF = fleptonSF1 * fleptonSF2;
                }
            }
            //fleptonSF = fleptonSF1 * fleptonSF2;
            //if(debug) cout << "lepton1 SF:  " << fleptonSF1 << "  lepton2 SF:  " << fleptonSF2 << endl;
            if(Muon && !Electron && MM && !isData){
                if( abs( selectedMuons[0]->Eta() )<0.9 ){
                    if( abs( selectedMuons[1]->Eta()<0.9 ) ) {sfleptrig_BCDEF = 0.948179;sfleptrig_GH = (7540.49*0.976139 + 8605.69*0.968985)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<1.2 ) ) {sfleptrig_BCDEF = 0.960657;sfleptrig_GH = (7540.49*0.980864 + 8605.69*0.979902)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.1 ) ) {sfleptrig_BCDEF = 0.968998;sfleptrig_GH = (7540.49*0.990586 + 8605.69*0.986834)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.4 ) ) {sfleptrig_BCDEF = 0.960431;sfleptrig_GH = (7540.49*0.985026 + 8605.69*0.978097)/(7540.49+8605.69);}
                    else {cerr << "Error" << endl; exit(1);}
                } else if( abs( selectedMuons[0]->Eta() )<1.2 ) {
                    if( abs( selectedMuons[1]->Eta()<0.9 ) ) {sfleptrig_BCDEF = 0.955935;sfleptrig_GH = (7540.49*0.982464 + 8605.69*0.983213)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<1.2 ) ) {sfleptrig_BCDEF = 0.964931;sfleptrig_GH = (7540.49*1.016780 + 8605.69*0.999540)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.1 ) ) {sfleptrig_BCDEF = 0.982130;sfleptrig_GH = (7540.49*0.991973 + 8605.69*0.991456)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.4 ) ) {sfleptrig_BCDEF = 0.951884;sfleptrig_GH = (7540.49*1.004301 + 8605.69*0.999805)/(7540.49+8605.69);}
                    else {cerr << "Error" << endl; exit(1);}
                } else if( abs( selectedMuons[0]->Eta() )<2.1 ) {
                    if( abs( selectedMuons[1]->Eta()<0.9 ) ) {sfleptrig_BCDEF = 0.970120;sfleptrig_GH = (7540.49*0.988191 + 8605.69*0.983700)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<1.2 ) ) {sfleptrig_BCDEF = 0.979653;sfleptrig_GH = (7540.49*0.992394 + 8605.69*0.982809)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.1 ) ) {sfleptrig_BCDEF = 0.991621;sfleptrig_GH = (7540.49*0.994308 + 8605.69*0.976385)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.4 ) ) {sfleptrig_BCDEF = 0.983483;sfleptrig_GH = (7540.49*0.989969 + 8605.69*0.983709)/(7540.49+8605.69);}
                    else {cerr << "Error" << endl; exit(1);}
                } else if( abs( selectedMuons[0]->Eta() )<2.4 ) {
                    if( abs( selectedMuons[1]->Eta()<0.9 ) ) {sfleptrig_BCDEF = 0.970599;sfleptrig_GH = (7540.49*0.990155 + 8605.69*0.979211)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<1.2 ) ) {sfleptrig_BCDEF = 0.960382;sfleptrig_GH = (7540.49*1.005147 + 8605.69*0.992346)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.1 ) ) {sfleptrig_BCDEF = 0.981527;sfleptrig_GH = (7540.49*1.000300 + 8605.69*0.992799)/(7540.49+8605.69);}
                    else if( abs( selectedMuons[1]->Eta()<2.4 ) ) {sfleptrig_BCDEF = 0.968428;sfleptrig_GH = (7540.49*0.972295 + 8605.69*0.974504)/(7540.49+8605.69);}
                    else {cerr << "Error" << endl; exit(1);}
                } else {cerr << "Error" << endl; exit(1);}
            } else if(Muon && M && !MM && !ME && !isData){
                sfleptrig_BCDEF = muonSFWeightTrig_BCDEF->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
                sfleptrig_GH = muonSFWeightTrig_GH->at(selectedMuons[0]->Eta(), selectedMuons[0]->Pt(), 0);
            }


            float fTriggerSF = 1.0;
            if(Electron && Muon)
                fTriggerSF *= 0.971;
            else if(!Electron && Muon)
                fTriggerSF *= 0.931;
            else if(Electron && !Muon)
                fTriggerSF *= 0.958;

            ///////////////////////
            // Getting Gen Event //
            ///////////////////////

            if(!isData){
                vector<TRootMCParticle*> mcParticles;
                vector<TRootMCParticle*> mcTops;
                mcParticlesMatching_.clear();
                mcParticlesTLV.clear();
                //selectedJetsTLV.clear();
                mcParticles.clear();
                mcTops.clear();

                treeLoader.LoadMCEvent(ievt, 0, mcParticlesMatching_, false);
                if(debug)
                    cout << "size   " << mcParticlesMatching_.size() << endl;
            }

            /////////////////////////////////////////////////
            //           heavy flav  reweighting           //
            /////////////////////////////////////////////////
            
            //vector<TRootMCParticle*> mcParticles_flav;
            /*
            float numOfbb = 0;
            float numOfcc = 0;
            float numOfll = 0;
            float ttbar_flav = -1;
            vector<TRootMCParticle*> mcParticles_flav;
            TRootGenEvent* genEvt_flav = 0;
            if(dataSetName.find("TTJets") != string::npos || dataSetName.find("TTScale") != string::npos) {
                genEvt_flav = treeLoader.LoadGenEvent(ievt,false);
                treeLoader.LoadMCEvent(ievt, 0, mcParticles_flav, false);
                for(int gj = 0; gj < genjets.size(); gj++) {
                    if(genjets[gj]->BHadron().Et() > 0) {
                        if(genjets[gj]->Pt() > 20 && abs(genjets[gj]->Eta() < 2.5))
                            numOfbb++;
                   }
                  if(genjets[gj]->CHadron().Et() > 0) {
                        if(genjets[gj]->Pt() > 20 && abs(genjets[gj]->Eta() < 2.5))
                            numOfcc++;
                   }
                }
            }

            if(numOfbb > 2) {
                ttbar_flav = 2;
            } else if(numOfcc > 0) {
                ttbar_flav = 1;
            } else {
                ttbar_flav = 0;
            }
            cout << "ttbar_flav: " << ttbar_flav << endl;
            cin.get();            */
            

            ///////////////////////
            //  Top PT Reweight  //
            ///////////////////////

            float fTopPtsf = 1, fAntitopPtsf = 1, fTopPtReWeightsf = 1;
            int nTops = 0, nAntiTops = 0;
            if(dataSetName.find("TTJets") != string::npos || dataSetName.find("TTDilept") != string::npos || dataSetName.find("TT") != string::npos) {
                for(int part = 0; part < mcParticlesMatching_.size(); part++) {
                    if(mcParticlesMatching_[part]->type() == 6 && mcParticlesMatching_[part]->status() == 22) {
                        if(mcParticlesMatching_[part]->Pt() < 400)
                            fTopPtsf = exp(0.0615 - (0.0005 * mcParticlesMatching_[part]->Pt()));
                        else
                            fTopPtsf = exp(0.0615 - (0.0005 * 400));
                        nTops++;
                    }
                    else if(mcParticlesMatching_[part]->type() == -6 && mcParticlesMatching_[part]->status() == 22) {
                        if(mcParticlesMatching_[part]->Pt() < 400)
                            fAntitopPtsf = exp(0.0615 - (0.0005 * mcParticlesMatching_[part]->Pt()));
                        else
                            fAntitopPtsf = exp(0.0615 - (0.0005 * 400));
                        nAntiTops++; 
                    }
                }
                fTopPtReWeightsf = sqrt(fTopPtsf * fAntitopPtsf);
            }


            //////////////////////////////
            //  Behrends Scaling Factor //
            //////////////////////////////

            float fbehrendsSF = 1.0;

            //////////////////////////////
            //  AlphaS Scaling Factor //
            //////////////////////////////

            float alphaTune = 1, falphaTuneSF = 1.0;


            /////////////////////////////
            // Aggregating ScaleFactor //
            /////////////////////////////

            scaleFactor *= centralWeight;
            scaleFactor *= lumiWeight;
            scaleFactor *= fleptonSF_GH;
            scaleFactor *= (btagWeight_light * btagWeight_heavy);
            scaleFactor *= falphaTuneSF;
            scaleFactor *= fTopPtReWeightsf;

            weightCount += scaleFactor;


            //////////////////////
            //   W/Top tagging  //
            //////////////////////

            float nTopTags = 0;
            float nWTags = 0;
            for(int fatjet = 0; fatjet < nFatJets; fatjet++) {
                float tau1 = selectedFatJets[fatjet]->Tau1();
                float tau2 = selectedFatJets[fatjet]->Tau2();
                float prunedmass = selectedFatJets[fatjet]->PrunedMass();
                float nsubjets = selectedFatJets[fatjet]->CmsTopTagNsubjets();
                float minmass = selectedFatJets[fatjet]->CmsTopTagMinMass();
                float topmass = selectedFatJets[fatjet]->CmsTopTagMass();
                // W-tagging
                if((tau2 / tau1) > 0.6 && prunedmass > 50.0) {
                    nWTags++;                    // cout <<"W-TAG!"<<endl;
                }
                // Top-tagging
                if(nsubjets > 2 && minmass > 50.0 && topmass > 150.0) {
                    nTopTags++;                    // cout <<"TOP-TAG!"<<endl;
                }
            }


            sort(selectedJets.begin(), selectedJets.end(), HighestCVSBtag());


            //////////////////////////////////////
            // MVA Hadronic Top Reconstructions //
            //////////////////////////////////////

            float TriJetMass, DiJetMass;
            TLorentzVector Wh, Bh, Th;
            int wj1;
            int wj2;
            int bj1;

            if(nJets >= 4) {
                jetCombiner->ProcessEvent_SingleHadTop(datasets[d], mcParticlesMatching_, selectedJets, selectedLeptonTLV_JC[0], scaleFactor);
                if(!TrainMVA) {
                    MVAvals1 = jetCombiner->getMVAValue(MVAmethod, 1); // 1 means the highest MVA value
                    topness = MVAvals1.first;
                    for(Int_t seljet1 = 0; seljet1 < selectedJets.size(); seljet1++) {
                        if(seljet1 == MVAvals1.second[0] || seljet1 == MVAvals1.second[1] || seljet1 == MVAvals1.second[2]) {
                            MVASelJets1.push_back(selectedJets[seljet1]);
                        }
                    }

                    // check data-mc agreement of kin. reco. variables.
                    float mindeltaR = 100.;
                    float mindeltaR_temp = 100.;
                    // define the jets from W as the jet pair with smallest deltaR
                    for(int m = 0; m < MVASelJets1.size(); m++) {
                        for(int n = 0; n < MVASelJets1.size(); n++) {
                            if(n == m) continue;
                            TLorentzVector lj1 = *MVASelJets1[m];
                            TLorentzVector lj2 = *MVASelJets1[n];
                            mindeltaR_temp = lj1.DeltaR(lj2);
                            if(mindeltaR_temp < mindeltaR) {
                                mindeltaR = mindeltaR_temp;
                                wj1 = m;
                                wj2 = n;
                            }
                        }
                    }
                    // find the index of the jet not chosen as a W-jet
                    for(unsigned int p = 0; p < MVASelJets1.size(); p++) {
                        if(p != wj1 && p != wj2)
                            bj1 = p;
                    }

                    if(debug)
                        cout << "Processing event with jetcombiner : 3 " << endl;

                    // now that putative b and W jets are chosen, calculate the six kin. variables.
                    Wh = *MVASelJets1[wj1] + *MVASelJets1[wj2];
                    Bh = *MVASelJets1[bj1];
                    Th = Wh + Bh;

                    TriJetMass = Th.M();

                    DiJetMass = Wh.M();
                    // DeltaR
                    float AngleThWh = fabs(Th.DeltaPhi(Wh));
                    float AngleThBh = fabs(Th.DeltaPhi(Bh));

                    float btag = MVASelJets1[bj1]->btag_combinedInclusiveSecondaryVertexV2BJetTags();

                    double PtRat = ((*MVASelJets1[0] + *MVASelJets1[1] + *MVASelJets1[2]).Pt()) / (MVASelJets1[0]->Pt() + MVASelJets1[1]->Pt() + MVASelJets1[2]->Pt());
                    double diLepThdR = fabs(Th.DeltaR(diLep));
                    double diLepThdPhi = fabs(Th.DeltaPhi(diLep));

                    if(debug)
                        cout << "Processing event with jetcombiner : 4 " << endl;

                    if(debug)
                        cout << "Processing event with jetcombiner : 8 " << endl;
                }
            }

            if(nEl >= 1) {
                float epRat = selectedElectrons[0]->E() / selectedElectrons[0]->P();
                if(debug) cout << "Electron E/P Ratio : " << epRat << endl;
            }

            dRLep = lep1.DeltaR(lep2);

            HT = 0;
            float HT1M2L = 0, H1M2L = 0, HTbjets = 0, HT2M = 0, H2M = 0, dRbb = 0;

            for(Int_t seljet1 = 0; seljet1 < selectedJets.size(); seljet1++) {
                if(nMtags >= 2 && seljet1 >= 2) {
                    jetpt = selectedJets[seljet1]->Pt();
                    HT2M = HT2M + jetpt;
                    H2M = H2M + selectedJets[seljet1]->P();
                }
                if(selectedJets[seljet1]->btag_combinedInclusiveSecondaryVertexV2BJetTags() > 0.8484) {
                    HTb += selectedJets[seljet1]->Pt();
                }
                // Event-level variables
                jetpt = selectedJets[seljet1]->Pt();
                HT = HT + jetpt;
                H = H + selectedJets[seljet1]->P();
                /*jettup->Fill(scaleFactor, normfactor, Luminosity, selectedJets[seljet1]->neutralHadronEnergyFraction(),
                             selectedJets[seljet1]->neutralEmEnergyFraction(), selectedJets[seljet1]->nConstituents(),
                             selectedJets[seljet1]->chargedHadronEnergyFraction(), selectedJets[seljet1]->chargedMultiplicity(),
                             selectedJets[seljet1]->chargedEmEnergyFraction());*/
            }
            dRbb = fabs(selectedJets[0]->DeltaR(*selectedJets[1]));
            HTRat = (selectedJets[0]->Pt() + selectedJets[1]->Pt()) / HT;
            HTH = HT / H;

            //histo2D["HTLepSep"]->Fill(HT, lep1.DeltaR(lep2));
            sort(selectedJets.begin(), selectedJets.end(), HighestPt()); // order Jets wrt Pt for tuple output

            float reliso1, reliso2;
            if(dilepton && Muon && Electron) {
                muonpt = selectedMuons[0]->Pt();
                muoneta = selectedMuons[0]->Eta();
                electronpt = selectedElectrons[0]->Pt();
                reliso1 = selectedMuons[0]->relPfIso(4,0.5);
                reliso2 = ElectronRelIso(selectedElectrons[0],rho);
            }
            if(dilepton && Muon && !Electron) {
                muonpt = selectedMuons[0]->Pt();
                muoneta = selectedMuons[0]->Eta();
                reliso1 = selectedMuons[0]->relPfIso(4,0.5);
                reliso2 = selectedMuons[1]->relPfIso(4,0.5);
            }
            if(dilepton && !Muon && Electron) {
                muonpt = selectedElectrons[0]->Pt();
                muoneta = selectedElectrons[0]->Eta();
                reliso1 = ElectronRelIso(selectedElectrons[0],rho);
                reliso2 = ElectronRelIso(selectedElectrons[1],rho);
            }

            if(nMtags >= 1) {
                bjetpt = selectedMBJets[0]->Pt();
            }

            ////////////////////
            // Topology Plots //
            ////////////////////
            float tSph = 0, dSph = 0, tdSph = 0, tCen = 0, dCen = 0, tdCen = 0;

            topologyW->setPartList(selectedJetsTLV, selectedJetsTLV);
            float fSphericity = topologyW->get_sphericity();
            float fOblateness = topologyW->oblateness();
            float fAplanarity = topologyW->get_aplanarity();
            float fh10 = topologyW->get_h10();
            float fh20 = topologyW->get_h20();
            float fh30 = topologyW->get_h30();
            float fh40 = topologyW->get_h40();
            float fh50 = topologyW->get_h50();
            float fh60 = topologyW->get_h60();
            float fht = topologyW->get_ht();
            float fht3 = topologyW->get_ht3();
            float fet0 = topologyW->get_et0();
            float fsqrts = topologyW->get_sqrts();
            float fnjetW = topologyW->get_njetW();
            float fet56 = topologyW->get_et56();
            float fcentrality = topologyW->get_centrality();

            vector<TLorentzVector> selectedParticlesTLV, diLepSystemTLV, topDiLepSystemTLV;
            // collection Total Event TLVs
            selectedParticlesTLV.insert(selectedParticlesTLV.end(), selectedElectronsTLV_JC.begin(), selectedElectronsTLV_JC.end());
            selectedParticlesTLV.insert(selectedParticlesTLV.end(), selectedMuonsTLV_JC.begin(), selectedMuonsTLV_JC.end());
            selectedParticlesTLV.insert(selectedParticlesTLV.end(), selectedJetsTLV.begin(), selectedJetsTLV.end());
            selectedParticlesTLV.push_back(*mets[0]);
            // collecting diLep TLVs
            diLepSystemTLV.push_back(lep1);
            diLepSystemTLV.push_back(lep2);
            diLepSystemTLV.push_back(*mets[0]);
            // collecting topDiLep TLVs
            topDiLepSystemTLV.insert(topDiLepSystemTLV.end(), diLepSystemTLV.begin(), diLepSystemTLV.end());
            if(!TrainMVA) {
                if(nJets >= 4) {
                    topDiLepSystemTLV.push_back(*MVASelJets1[wj1]);
                    topDiLepSystemTLV.push_back(*MVASelJets1[wj2]);
                    topDiLepSystemTLV.push_back(*MVASelJets1[bj1]);
                }
                tSph = Sphericity(selectedParticlesTLV), tCen = Centrality(selectedParticlesTLV);
                dSph = Sphericity(diLepSystemTLV), dCen = Centrality(diLepSystemTLV);
                tdSph = Sphericity(topDiLepSystemTLV), tdCen = Centrality(topDiLepSystemTLV);
            }



            if((isData || dataSetName.find("DY") != string::npos) && Muon && Electron) {
                cout << "Data Event Passed Selection.  Run: " << event->runId()
                     << " LumiSection: " << event->lumiBlockId() << " Event: " << event->eventId() << " HT: " << HT
                     << " nMTags: " << nMtags << " nJets: " << nJets << endl;
                cout << "Muon Eta: " << selectedMuons[0]->Eta() << " Muon Pt: " << selectedMuons[0]->Pt()
                     << " Electron Eta: " << selectedElectrons[0]->Eta() << " Electron Pt: " << selectedElectrons[0]->Pt() << endl;
            }
            if((isData || dataSetName.find("DY") != string::npos) && Muon && !Electron) {
                cout << "Data Event Passed Selection.  Run: " << event->runId()
                     << " LumiSection: " << event->lumiBlockId() << " Event: " << event->eventId() << " HT: " << HT
                     << " nMTags: " << nMtags << " nJets: " << nJets << endl;
                cout << "Muon1 Eta: " << selectedMuons[0]->Eta() << " Muon1 Pt: " << selectedMuons[0]->Pt()
                     << " Muon2 Eta: " << selectedMuons[1]->Eta() << " Muon2 Pt: " << selectedMuons[1]->Pt() << endl;
            }
            if((isData || dataSetName.find("DY") != string::npos) && !Muon && Electron) {
                cout << "Data Event Passed Selection.  Run: " << event->runId()
                     << " LumiSection: " << event->lumiBlockId() << " Event: " << event->eventId() << " HT: " << HT
                     << " nMTags: " << nMtags << " nJets: " << nJets << endl;
                cout << "Electron1 Eta: " << selectedElectrons[0]->Eta() << " Electron1 Pt: " << selectedElectrons[0]->Pt()
                     << " Electron2 Eta: " << selectedElectrons[1]->Eta() << " Electron2 Pt: " << selectedElectrons[1]->Pt() << endl;
                //for(int nj=0; nj< selectedJets.size(); nj++) cout << "Jet Pt: " << selectedJets[nj]->Pt() << endl;
                //cout << "trigged: " << trigged << "goodPV: " << isGoodPV << "nEl: " << nEl << "nMu: " << nMu << "dilepMass" << diLepMass << "charge: " << sameCharge << "ZVeto: " << ZVeto << endl;
            }

            ////////////////////
            // Filling nTuple //
            //////////////////// 
            float vals[98] = { fleptonSF_BCDEF, nJets, nFatJets, nWTags, nTopTags, nLtags, nMtags, nTtags,
                (float)(nJets > 0 ? selectedJets[0]->Pt() : -9999), (float)(nJets > 1 ? selectedJets[1]->Pt() : -9999),
                (float)(nJets > 2 ? selectedJets[2]->Pt() : -9999), (float)(nJets > 3 ? selectedJets[3]->Pt() : -9999),
                (float)(nJets > 4 ? selectedJets[4]->Pt() : -9999), (float)(nJets > 5 ? selectedJets[5]->Pt() : -9999),
                (float)(nJets > 6 ? selectedJets[6]->Pt() : -9999), (float)(nJets > 7 ? selectedJets[7]->Pt() : -9999),
                (float)mets[0]->Et(), HT, 0, 0, 0, dRLep, (float)(sameCharge ? 2 : 0), lepFlavor, (float)nLep, bjetpt,
                dRbb, HT2M, HTb, HTH, HTRat, topness, tSph, tCen, dSph, dCen, tdSph, tdCen, fnjetW, ttbar_flav,
                fleptonSF_GH, btagWeightCSV, btagWeight_light, btagWeight_lightUp, btagWeight_lightDown, btagWeight_heavy, btagWeight_heavyUp,
                btagWeight_heavyDown, lumiWeight, lumiWeight_up, lumiWeight_down, btagWeightCSVLFUp, btagWeightCSVLFDown, btagWeightCSVHFUp,
                btagWeightCSVHFDown ,btagWeightCSVHFStats1Up, btagWeightCSVHFStats1Down, btagWeightCSVHFStats2Up, btagWeightCSVHFStats2Down,
                btagWeightCSVLFStats1Up, btagWeightCSVLFStats1Down, btagWeightCSVLFStats2Up, btagWeightCSVLFStats2Down, btagWeightCSVCFErr1Up,
                btagWeightCSVCFErr1Down, btagWeightCSVCFErr2Up, btagWeightCSVCFErr2Down, falphaTuneSF, fTopPtReWeightsf, ISRsf,
                scaleFactor, nvertices, normfactor, Luminosity, centralWeight, weight1, weight2, weight3, weight4, weight5, weight6, weight7, weight8,
                diLepMass, (float) currentRun, (float) LumiBlock, (float) currentEvent, TriJetMass, DiJetMass, 0, 0, (float) selectedJets[0]->Phi(),
                weight_hdamp_up, weight_hdamp_dw, reliso1, reliso2, sfleptrig_BCDEF, sfleptrig_GH};

            if(Muon && Electron) {
                vals[18] = muonpt;
                vals[19] = muoneta;
                vals[20] = selectedElectrons[0]->Pt();
                vals[89] = selectedMuons[0]->Phi();
                vals[90] = selectedElectrons[0]->Phi();
            }
            if(Muon && !Electron) {
                vals[18] = muonpt;
                vals[19] = muoneta;
                vals[20] = selectedMuons[1]->Pt();
                vals[89] = selectedMuons[0]->Phi();
                vals[90] = selectedMuons[1]->Phi();
            }
            if(!Muon && Electron) {
                vals[18] = muonpt;
                vals[19] = muoneta;
                vals[20] = selectedElectrons[1]->Pt();
                vals[89] = selectedElectrons[0]->Phi();
                vals[90] = selectedElectrons[1]->Phi();
            }

            tup->Fill(vals);

        } // End Loop on Events

        tupfile->cd();
        tup->Write();
        tupfile->Close();
        cuttupfile->cd();
        cuttup->Write();
        cuttupfile->Close();



        cout << "n events passing selection  = " << passed << endl;
        cout << "n events passing trigger    = " << trigCount << endl;
        cout << "n events passing filter     = " << eventCount << endl;
        cout << "trigger eff. = " << (float) trigCount/(float) eventCount << endl;
        cout << "n events with negative weights = " << negWeights << endl;
        cout << "Weight Count = " << weightCount << endl;

        treeLoader.UnLoadDataset(); // important: free memory
    } // End Loop on Datasets

    cout << "Writing outputs to the files ..." << endl;

    if(bTagReweight){
        delete btwt;
        delete btwtUp;
        delete btwtDown;
    }

    //////////////////////
    //   MVA Training   //
    //////////////////////

    if(TrainMVA){
        TFile* foutmva = new TFile("foutMVA.root", "RECREATE");
        jetCombiner->Write(foutmva, true, pathPNG.c_str());
        delete foutmva;
    }

    cout << "It took us " << ((double)clock() - start) / CLOCKS_PER_SEC << "s to run the program" << endl;
    cout << "********************************************" << endl;
    cout << "           End of the program !!            " << endl;
    cout << "********************************************" << endl;

    return 0;
}

int Factorial(int N = 1)
{
    int fact = 1;
    for(int i = 1; i <= N; i++)
        fact = fact * i; // OR fact *= i;
    return fact;
}

float Sphericity(vector<TLorentzVector> parts)
{
    if(parts.size() > 0) {
        double spTensor[3 * 3] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. };
        int counter = 0;
        float tensorNorm = 0, y1 = 0, y2 = 0, y3 = 0;

        for(int tenx = 0; tenx < 3; tenx++) {
            for(int teny = 0; teny < 3; teny++) {
                for(int selpart = 0; selpart < parts.size(); selpart++) {

                    spTensor[counter] += ((parts[selpart][tenx]) * (parts[selpart][teny]));
                    //                    if((tenx == 0 && teny == 2) || (tenx == 2 && teny == 1))
                    //                    {
                    //                    cout << "nan debug term " << counter+1 << ": " <<
                    //                    (parts[selpart][tenx])*(parts[selpart][teny]) << endl;
                    //                    cout << "Tensor Building Term " << counter+1 << ": " << spTensor[counter] <<
                    //                    endl;
                    //                    }
                    if(tenx == 0 && teny == 0) {
                        tensorNorm += parts[selpart].Vect().Mag2();
                    }
                }
                if((tenx == 0 && teny == 2) || (tenx == 2 && teny == 1)) {
                    //                    cout << "Tensor term pre-norm " << counter+1 << ": " << spTensor[counter] <<
                    //                    endl;
                }
                spTensor[counter] /= tensorNorm;
                //                cout << "Tensor Term " << counter+1 << ": " << spTensor[counter] << endl;
                counter++;
            }
        }
        TMatrixDSym m(3, spTensor);
        // m.Print();
        TMatrixDSymEigen me(m);
        TVectorD eigenval = me.GetEigenValues();
        vector<float> eigenVals;
        eigenVals.push_back(eigenval[0]);
        eigenVals.push_back(eigenval[1]);
        eigenVals.push_back(eigenval[2]);
        sort(eigenVals.begin(), eigenVals.end());
        // cout << "EigenVals: "<< eigenVals[0] << ", " << eigenVals[1] << ", " << eigenVals[2] << ", " << endl;
        float sp = 3.0 * (eigenVals[0] + eigenVals[1]) / 2.0;
        // cout << "Sphericity: " << sp << endl;
        return sp;
    } else {
        return 0;
    }
}
float Centrality(vector<TLorentzVector> parts)
{
    float E = 0, ET = 0;
    for(int selpart = 0; selpart < parts.size(); selpart++) {
        E += parts[selpart].E();
        ET += parts[selpart].Et();
    }
    return ET / E;
}

float ElectronRelIso(TRootElectron* el, float rho)
{
    double EffectiveArea = 0.;
    // Updated to Spring 2015 EA from
    // https://github.com/cms-sw/cmssw/blob/CMSSW_7_4_14/RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt#L8
    if(fabs(el->superClusterEta()) >= 0.0 && fabs(el->superClusterEta()) < 1.0)
        EffectiveArea = 0.1752;
    if(fabs(el->superClusterEta()) >= 1.0 && fabs(el->superClusterEta()) < 1.479)
        EffectiveArea = 0.1862;
    if(fabs(el->superClusterEta()) >= 1.479 && fabs(el->superClusterEta()) < 2.0)
        EffectiveArea = 0.1411;
    if(fabs(el->superClusterEta()) >= 2.0 && fabs(el->superClusterEta()) < 2.2)
        EffectiveArea = 0.1534;
    if(fabs(el->superClusterEta()) >= 2.2 && fabs(el->superClusterEta()) < 2.3)
        EffectiveArea = 0.1903;
    if(fabs(el->superClusterEta()) >= 2.3 && fabs(el->superClusterEta()) < 2.4)
        EffectiveArea = 0.2243;
    if(fabs(el->superClusterEta()) >= 2.4 && fabs(el->superClusterEta()) < 5.0)
        EffectiveArea = 0.2687;

    double isoCorr = 0;
    isoCorr = el->neutralHadronIso(3) + el->photonIso(3) - rho * EffectiveArea;
    float isolation = (el->chargedHadronIso(3) + (isoCorr > 0.0 ? isoCorr : 0.0)) / (el->Pt());

    return isolation;
}

float MuonRelIso(TRootMuon* mu)
{
    float isolation = (mu->chargedHadronIso(4) + max( 0.0, mu->neutralHadronIso(4) + mu->photonIso(4) - 0.5*mu->puChargedHadronIso(4) ) ) / (mu->Pt()); // dBeta corrected
    return isolation;
}

float PythiaTune(int jets)
{
    float sf = 1;

    if(jets == 0)
        sf = 0.9747749;
    else if(jets == 1)
        sf = 0.9764329;
    else if(jets == 2)
        sf = 0.9733197;
    else if(jets == 3)
        sf = 0.9815515;
    else if(jets == 4)
        sf = 0.9950933;
    else if(jets == 5)
        sf = 1.0368650;
    else if(jets == 6)
        sf = 1.1092038;
    else if(jets == 7)
        sf = 1.1842445;
    else if(jets == 8)
        sf = 1.3019452;
    else if(jets == 9)
        sf = 1.1926751;
    else if(jets >= 10)
        sf = 1.5920859;

    return sf;
}
