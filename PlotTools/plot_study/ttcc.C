void ttcc()
{
//=========Macro generated from canvas: Canvas_1/Canvas_1
//=========  (Fri Jul 22 11:48:57 2016) by ROOT version6.02/13
   TCanvas *Canvas_1 = new TCanvas("Canvas_1", "Canvas_1",598,196,739,519);
   Canvas_1->Range(-0.975,-2.769598,0.775,24.92638);
   Canvas_1->SetFillColor(0);
   Canvas_1->SetBorderMode(0);
   Canvas_1->SetBorderSize(2);
   Canvas_1->SetFrameBorderMode(0);
   Canvas_1->SetFrameBorderMode(0);
   
   TH1F *BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1 = new TH1F("BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1","tt + cc",14,-0.8,0.6);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(2,0.01537154);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(3,1.88868);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(4,10.12789);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(5,20.9122);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(6,20.48398);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(7,17.3474);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(8,12.37426);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(9,7.795326);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(10,3.985495);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(11,1.622777);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(12,0.4006394);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(13,0.09595769);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinContent(14,0.005991659);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(2,0.005489509);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(3,0.05794667);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(4,0.1335305);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(5,0.1894927);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(6,0.184192);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(7,0.1674308);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(8,0.1402384);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(9,0.1102602);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(10,0.07748134);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(11,0.04836812);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(12,0.02285782);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(13,0.01082066);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetBinError(14,0.002706147);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetEntries(67129);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#cc3333");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetLineColor(ci);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetLineStyle(2);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetLineWidth(2);

   ci = TColor::GetColor("#cc3333");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetMarkerColor(ci);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->SetMarkerSize(0.5);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetXaxis()->SetTitle("BDT_DilepCombined22ndJune2016_13TeVHadTop");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetXaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetXaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetXaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetXaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetYaxis()->SetTitle("Events #backslash 0.10 units");
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetYaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetYaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetYaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetYaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetZaxis()->SetLabelFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetZaxis()->SetLabelSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetZaxis()->SetTitleSize(0.035);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->GetZaxis()->SetTitleFont(42);
   BDT_DilepCombined22ndJune2016_13TeVHadTop_TTJets_cc1->Draw("");
   
   TPaveText *pt = new TPaveText(0.4382993,0.94,0.5617007,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *AText = pt->AddText("tt + cc");
   pt->Draw();
   Canvas_1->Modified();
   Canvas_1->cd();
   Canvas_1->SetSelected(Canvas_1);
}
