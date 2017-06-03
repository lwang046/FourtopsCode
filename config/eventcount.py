from ROOT import TChain
from glob import glob

path = '/pnfs/iihe/cms/store/user/fblekman/TopTree/CMSSW_76X_v3/TTTT_TuneCUETP8M1_13TeV-amcatnlo-pythia8/crab_atnlopythia8Fall15PU25nsData2015v176XmcRun2asymptoticv12ext1v176XmcRun2asymptoticFall15DR76v1crab35/160531_182635/0000/*.root'
files = glob(path)
root_files = []
for f in files:
	root_files.append('dcap://maite.iihe.ac.be' + f)
print root_files
chain = TChain('eventTree')
for rf in root_files:
	chain.Add(rf)
print 'added files'
nEntries = chain.GetEntries();
print nEntries

