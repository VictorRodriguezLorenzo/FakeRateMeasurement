# Everything begins here

Log in to lxplus.

    ssh -Y piedra@lxplus.cern.ch -o ServerAliveInterval=240
    bash -l

# First time

Setup the CMSSW release. The code `nanoFakes.C` fails with (at least) `10_2_0` and `10_6_4`.

    cd work
    mkdir fakes

    export SCRAM_ARCH=slc7_amd64_gcc11
    cmsrel CMSSW_13_0_5_patch2
    cd CMSSW_13_0_5_patch2/src
    cmsenv

Original code.

    git clone https://github.com/latinos/FakeRateMeasurement

Updated code.

    git clone https://github.com/piedraj/FakeRateMeasurement

# Get ready

    cd work/fakes/CMSSW_10_1_0/src
    cmsenv
    cd FakeRateMeasurement

# Check before job submission

Check (and edit if needed) the following files.

   * `nanoFakes.h` contains the tight lepton names. They might differ between 2016, 2017, 2018.
   * `nanoFakes.C` contains the triggers and the corresponding prescales. Edit this file if, for example, you need to move from Ele12 to Ele8.
   * `submitJobs.py` contains the data and MC samples names. Verify that they match the current production.

# Submit jobs



### 2016_HIPM

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Summer20UL16_106x_nAODv9_HIPM_Full2016v9/MCl1loose2016v9__fakeSelKinMC/ -y 2016_HIPM
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Run2016_UL2016_nAODv9_HIPM_Full2016v9/DATAl1loose2016v9__fakeSel/ -y 2016_HIPM

Read trees without any mtw1 cut to debug the 2016_HIPM negative fakes.

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Summer20UL16_106x_nAODv9_HIPM_Full2016v9/MCl1loose2016v9__MCCorr2016v9/ -y 2016_HIPM
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Run2016_UL2016_nAODv9_HIPM_Full2016v9/DATAl1loose2016v9/ -y 2016_HIPM

### 2016_noHIPM

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Summer20UL16_106x_nAODv9_noHIPM_Full2016v9/MCl1loose2016v9__fakeSelKinMC/ -y 2016_noHIPM
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Run2016_UL2016_nAODv9_noHIPM_Full2016v9/DATAl1loose2016v9__fakeSel/ -y 2016_noHIPM

### 2017

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Summer20UL17_106x_nAODv9_Full2017v9/MCl1loose2017v9__fakeSelKinMC/ -y 2017
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Run2017_UL2017_nAODv9_Full2017v9/DATAl1loose2017v9__fakeSel/ -y 2017

### 2018

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Summer20UL18_106x_nAODv9_Full2018v9/MCl1loose2018v9__fakeSelKinMC/ -y 2018
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/amassiro/HWWNano/Run2018_UL2018_nAODv9_Full2018v9/DATAl1loose2018v9__fakeSel -y 2018

### 2022 (NEW POST PROD ON THE WAY)

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/calderon/HWWNano/Summer22_130x_nAODv12_Full2022v12/MCl1loose2022v12__fakeSelKinMC/ -y 2022
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/calderon/HWWNano/Run2022_ReReco_nAODv12_Full2022v12/DATAl1loose2022v12__fakeSel/ -y 2022


### 2022EE

    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/calderon/HWWNano/Summer22EE_130x_nAODv12_Full2022v12/MCl1loose2022EEv12__MCCorr2022EEv12_ReRecoE_PromptFG/ -y 2022EE
    python submitJobs.py -d /eos/cms/store/group/phys_higgs/cmshww/calderon/HWWNano/Run2022EE_Prompt_nAODv12_Full2022v12/DATAl1loose2022EEv12__fakeSel/ -y 2022EE

# Babysit jobs

    condor_q
    condor_q -hold -af HoldReason

# Wrap it up

### FOR THE YEARS 2016, 2017 AND 2018
    
    cd results

    hadd -f -k hadd_wjets.root nanoLatino_WJetsToLNu*.root
    hadd -f -k hadd_zjets.root nanoLatino_DYJetsToLL*.root
    hadd -f -k hadd_data.root  nanoLatino_*_Run201*.root

For 2018 data there are too many files, and the `hadd` has to be done in two steps.

    hadd -f -k hadd_EGamma_Run2018.root nanoLatino_EGamma_Run2018*.root
    hadd -f -k hadd_DoubleMuon_Run2018.root nanoLatino_DoubleMuon_Run2018*.root

    hadd -f -k hadd_data.root hadd_EGamma_Run2018.root hadd_DoubleMuon_Run2018.root

    rm hadd_EGamma_Run2018.root
    rm hadd_DoubleMuon_Run2018.root

### FOR THE YEAR 2022

    cd results

    hadd -f -k hadd_wjets.root *WToLNu-2Jets__*.root

    hadd -f -k hadd_zjets.root *DYto2L*.root

    hadd -f -k hadd_data.root  nanoLatino_*_Run202*.root

Move the merged files to their corresponding year directory.

    rm nanoLatino*
    mkdir <year>
    mv hadd* <year>/.

# Extract the fake and prompt rates

    root -l -b -q getFakeRate.C

# Some relevant physics

   * The jet pt thresholds for electrons are 35 GeV for 0-jet, 1-jet and 2-jets.
   * The jet pt thresholds for muons are 20 GeV for 0-jet, 25 GeV for 1-jet, and 35 GeV for 2-jets.

