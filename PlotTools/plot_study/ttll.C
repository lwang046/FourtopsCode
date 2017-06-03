void ttll()
{
//=========Macro generated from canvas: Canvas_1/Canvas_1
//=========  (Fri Jul 22 11:49:44 2016) by ROOT version6.02/13
   TCanvas *Canvas_1 = new TCanvas("Canvas_1", "Canvas_1",754,113,739,519);
   Canvas_1->Range(-0.975,-1.60754,0.775,14.46786);
   Canvas_1->SetFillColor(0);
   Canvas_1->SetBorderMode(0);
   Canvas_1->SetBorderSize(2);
   Canvas_1->SetFrameBorderMode(0);
   Canvas_1->SetFrameBorderMode(0);
   
   TH1F *BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2 = new TH1F("BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2","tt + ll",14,-0.8,0.6);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(2,0.01078876+0.01537154);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(3,1.259082+1.88868);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(4,6.136602+10.12789);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(5,12.10304+20.9122);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(6,10.49252+20.48398);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(7,8.157854+17.3474);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(8,5.209528+12.37426);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(9,2.67928+7.795326);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(10,1.152246+3.985495);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(11,0.3529314+1.622777);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(12,0.07786927+0.4006394);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(13,0.009695718+0.09595769);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinContent(14,0.001213391+0.005991659);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(2,0.004429513+0.005489509);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(3,0.04775305+0.05794667);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(4,0.1040395+0.1335305);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(5,0.1448846+0.1894927);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(6,0.1327956+0.184192);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(7,0.1159051+0.1674308);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(8,0.09201141+0.1402384);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(9,0.06570919+0.1102602);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(10,0.04197866+0.07748134);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(11,0.02261942+0.04836812);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(12,0.01013587+0.02285782);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(13,0.003305611+0.01082066);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetBinError(14,0.001213391+0.002706147);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetEntries(32114);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#ff0000");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetLineColor(ci);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetLineStyle(2);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetLineWidth(2);

   ci = TColor::GetColor("#ff0000");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetMarkerColor(ci);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->SetMarkerSize(0.5);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetXaxis()->SetTitle("BDT_DilepCombined22ndJune2016_13TeVHadTop");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetXaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetXaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetXaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetXaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetYaxis()->SetTitle("Events #backslash 0.10 units");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetYaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetYaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetYaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetYaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetZaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetZaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetZaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->GetZaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_ll2->Draw("");
   
   TPaveText *pt = new TPaveText(0.4491837,0.94,0.5508163,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *AText = pt->AddText("tt + ll");
   pt->Draw();
   Canvas_1->Modified();
   Canvas_1->cd();
   Canvas_1->SetSelected(Canvas_1);
}
