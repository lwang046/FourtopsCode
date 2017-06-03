{
  map<string, TNtuple*> oldtup;
  map<string, TNtuple*> newtup;
  TFile *oldfile = new TFile("Data_Run2_TopTree_Study.root");
  TFile* newfile = new TFile("NewData_Run2_TopTree_Study.root","recreate");
  oldtup["a"] = (TNtuple*)oldfile->Get("Craneen__MuMu");
  oldtup["b"] = (TNtuple*)oldfile->Get("Craneen__MuEl");
  oldtup["c"] = (TNtuple*)oldfile->Get("Craneen__ElEl");

  int i;
  string str[3] = {"a", "b", "c"};

  for(i=0; i<3; i++){
    set<int> RunID, LumiBlock, EventID;
    float RunNumber, lumisec, EventNumber;
    int nEntries = (int)oldtup[str[i].c_str()]->GetEntries();

    oldtup[str[i].c_str()]->SetBranchAddress("RunNumber",&RunNumber);
    oldtup[str[i].c_str()]->SetBranchAddress("LumiBlock",&lumisec);
    oldtup[str[i].c_str()]->SetBranchAddress("EventNumber",&EventNumber);
    
    TEventList *elist = new TEventList();

    for (int j = 0; j < nEntries; ++j)
    {
      oldtup[str[i].c_str()]->GetEvent(j);
      if (RunID.count(RunNumber) == 0 || LumiBlock.count(lumisec) == 0 || EventID.count(EventNumber) == 0)
      {
        RunID.insert(RunNumber);
        LumiBlock.insert(lumisec);
        EventID.insert(EventNumber);
        elist->Enter(j);
      }
    }

    oldtup[str[i].c_str()]->SetEventList(elist);
    newtup[str[i].c_str()] = (TNtuple*)oldtup[str[i].c_str()]->CopyTree("");

  }

  newfile->cd();

  for(i=0; i<3; i++){
    newtup[str[i].c_str()]->Write();
  }

  for(i=0; i<3; i++){
    delete oldtup[str[i].c_str()];
    delete newtup[str[i].c_str()];
  }
  delete oldfile;
  delete newfile;

}
