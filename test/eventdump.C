{
TFile *f1  = new TFile("Data_Run2_TopTree_Study.root");
TNtuple * nTuple = (TNtuple*)f1->Get("Craneen__MuMu");
float runID, lumisec, eventID;
int nEntries = nTuple->GetEntries();
nTuple->SetBranchAddress("RunNumber",&runID);
nTuple->SetBranchAddress("LumiBlock",&lumisec);
nTuple->SetBranchAddress("EventNumber",&eventID);
cout<<nEntries<<endl;


for(int j=0; j<nEntries; j++){
  nTuple->GetEntry(j);
  cout << runID << "	" << lumisec << "	" << eventID << endl;
}





}
