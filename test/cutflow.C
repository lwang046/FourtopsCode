{

  cout << "Getting Files" << endl;
//  string Data = "../Cutflow_Comb/Data_Run2_TopTree.root";  //Getting Files
  string NP_overlay_ttttNLO = "../Cutflow_Comb/NP_overlay_ttttNLO_Run2_TopTree.root";
  string ST_tW = "../Cutflow_Comb/ST_tW_Run2_TopTree.root";
  string DYJets = "../Cutflow_Comb/DYJets_Run2_TopTree.root";
  string TTH = "../Cutflow_Comb/TTH_Run2_TopTree.root";
  string TTZ = "../Cutflow_Comb/TTZ_Run2_TopTree.root";
  string TTW = "../Cutflow_Comb/TTW_Run2_TopTree.root";
  string TTDileptPowheg = "../Cutflow_Comb/TTJetsPowheg_Run2_TopTree.root";

  vector < string > files;

//  files.push_back(Data);
  files.push_back(NP_overlay_ttttNLO);
  files.push_back(ST_tW);
  files.push_back(DYJets);
  files.push_back(TTH);
  files.push_back(TTZ);
  files.push_back(TTW);
  files.push_back(TTDileptPowheg);


  float xsec[7] = {0.009103, 71.7, 5075.6, 0.2934, 0.2529, 0.2043, 831.76};
  float lumi_mumu = 35822.4374055; 
  float lumi_muel = 35822.4374055;
  float lumi_elel = 35826.306834;

 for(int i = 0; i < files.size(); i++){
   TFile f(files[i].c_str(),"read");
   TNtuple * tupmumu = (TNtuple*)f.Get("Craneen__MuMu_CutFlow");
   TNtuple * tupmuel = (TNtuple*)f.Get("Craneen__MuEl_CutFlow");
   TNtuple * tupelel = (TNtuple*)f.Get("Craneen__ElEl_CutFlow");

   cout << "*********************" << endl;
   cout << "In Process : " << files[i] << endl;

   cout << "In MuMu channel:" << endl;
   float init_mumu = xsec[i] * lumi_mumu;
   int N_mumu = tupmumu->Draw("initevent>>hist","initevent==1","goff");
   cout << "init=" << init_mumu << endl;
   cout << "tgPV=" << (init_mumu/(float)N_mumu)*tupmumu->Draw("isGoodPV>>hist","isGoodPV==1","goff") << endl;
   cout << "osmu=" << (init_mumu/(float)N_mumu)*tupmumu->Draw("Lep1>>hist","Lep1==1","goff") << endl;
   cout << "veto=" << (init_mumu/(float)N_mumu)*tupmumu->Draw("Lep2>>hist","Lep2==1","goff") << endl;
   cout << "4jet=" << (init_mumu/(float)N_mumu)*tupmumu->Draw("nJets4>>hist","nJets4==1","goff") << endl;
   cout << "2btg=" << (init_mumu/(float)N_mumu)*tupmumu->Draw("nTags>>hist","nTags==1","goff") << endl;
   cout << "HT  =" << (init_mumu/(float)N_mumu)*tupmumu->Draw("HT>>hist","HT==1","goff") << endl;
   cout << "" << endl;

   cout << "In MuEl channel:" << endl;
   float init_muel = xsec[i] * lumi_muel;
   int N_muel = tupmuel->Draw("initevent>>hist","initevent==1","goff");
   cout << "init=" << init_muel << endl;
   cout << "tgPV=" << (init_muel/(float)N_muel)*tupmuel->Draw("isGoodPV>>hist","isGoodPV==1","goff") << endl;
   cout << "osmu=" << (init_muel/(float)N_muel)*tupmuel->Draw("Lep1>>hist","Lep1==1","goff") << endl;
   cout << "veto=" << (init_muel/(float)N_muel)*tupmuel->Draw("Lep2>>hist","Lep2==1","goff") << endl;
   cout << "4jet=" << (init_muel/(float)N_muel)*tupmuel->Draw("nJets4>>hist","nJets4==1","goff") << endl;
   cout << "2btg=" << (init_muel/(float)N_muel)*tupmuel->Draw("nTags>>hist","nTags==1","goff") << endl;
   cout << "HT  =" << (init_muel/(float)N_muel)*tupmuel->Draw("HT>>hist","HT==1","goff") << endl;
   cout << "" << endl;

   cout << "In ElEl channel:" << endl;
   float init_elel = xsec[i] * lumi_elel;
   int N_elel = tupelel->Draw("initevent>>hist","initevent==1","goff");
   cout << "init=" << init_elel << endl;
   cout << "tgPV=" << (init_elel/(float)N_elel)*tupelel->Draw("isGoodPV>>hist","isGoodPV==1","goff") << endl;
   cout << "osmu=" << (init_elel/(float)N_elel)*tupelel->Draw("Lep1>>hist","Lep1==1","goff") << endl;
   cout << "veto=" << (init_elel/(float)N_elel)*tupelel->Draw("Lep2>>hist","Lep2==1","goff") << endl;
   cout << "4jet=" << (init_elel/(float)N_elel)*tupelel->Draw("nJets4>>hist","nJets4==1","goff") << endl;
   cout << "2btg=" << (init_elel/(float)N_elel)*tupelel->Draw("nTags>>hist","nTags==1","goff") << endl;
   cout << "HT  =" << (init_elel/(float)N_elel)*tupelel->Draw("HT>>hist","HT==1","goff") << endl;
   cout << "" << endl;

   cout << "*********************" << endl;
   delete tupmumu; delete tupmuel; delete tupelel;
   f.Close();
 }
}
