
#include "TH1.h"
#include "TString.h"


TH1F* worker(TString samplestring="__NP_overlay_ttttNLO__nominal", Color_t sColour=kBlack){
    // TFile *file = new TFile("shapefileMu_DATA_BDT_with_TTX_JTS.root","read");
    TFile *file = new TFile("shapefileMu_DATA_BDT_ttbarPlusTTX_JTS2.root","read");
    file->cd();
    TString beginstring[12];
    beginstring[0]="ttttmu6nJets2nMtags";
    beginstring[1]="ttttmu6nJets3nMtags";
    beginstring[2]="ttttmu6nJets4nMtags";
    beginstring[3]="ttttmu7nJets2nMtags";
    beginstring[4]="ttttmu7nJets3nMtags";
    beginstring[5]="ttttmu7nJets4nMtags";
    beginstring[6]="ttttmu8nJets2nMtags";
    beginstring[7]="ttttmu8nJets3nMtags";
    beginstring[8]="ttttmu8nJets4nMtags";
    beginstring[9]="ttttmu9nJets2nMtags";
    beginstring[10]="ttttmu9nJets3nMtags";
    beginstring[11]="ttttmu9nJets4nMtags";
    
    TH1F *workhist = (TH1F*) file->Get(beginstring[0]+samplestring);
    double lowval =workhist->GetBinLowEdge(1);

    int nbins = workhist->GetNbinsX();
    double width = nbins*workhist->GetBinWidth(1);
    TH1F *newhist = new TH1F("summary_"+samplestring,"",12*nbins,lowval,lowval+(12*width));// histogram that contains all bins;
    newhist->SetLineColor(sColour);
    for(int ihist=0; ihist<12; ihist++){
        TH1F *workhist = (TH1F*) file->Get(beginstring[ihist]+samplestring);
        // workhist->Scale(1.0/workhist->Integral());

        int binworker = nbins*ihist;
        for(int ibin=1; ibin<=nbins; ibin++){
            newhist->SetBinContent(binworker+ibin,workhist->GetBinContent(ibin));
            // newhist->SetBinError(binworker+ibin,workhist->GetBinError(ibin));
                // newhist->GetXaxis()->SetBinLabel(ibin, "6jets 2 tags");

        }
        
    }
    // TCanvas *canv = new TCanvas();
    // canv->Draw();
    // newhist->Scale(1.0/newhist->Integral());

    // newhist->Draw();
    return newhist;
    // after this you can do some file operations like saving in a new alternative file.
}

// TH1F* worker(TString samplestring="__NP_overlay_ttttNLO__nominal", Color_t sColour=kBlack){
//     // TFile *file = new TFile("shapefileMu_DATA_BDT_with_TTX_JTS.root","read");
//     TFile *file = new TFile("shapefileMu_DATA_BDT_ttbarPlusTTX_JTS.root","read");
//     TFile *file = new TFile("ScaleFilesMu_light/ErrorBDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents.root","read");
//     file->cd();
//     TString beginstring_Minus[12];
//     beginstring_Minus[0]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets2nMtags/Minus";
//     beginstring_Minus[1]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets3nMtags/Minus";
//     beginstring_Minus[2]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets4nMtags/Minus";
//     beginstring_Minus[3]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets2nMtags/Minus";
//     beginstring_Minus[4]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets3nMtags/Minus";
//     beginstring_Minus[5]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets4nMtags/Minus";
//     beginstring_Minus[6]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets2nMtags/Minus";
//     beginstring_Minus[7]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets3nMtags/Minus";
//     beginstring_Minus[8]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets4nMtags/Minus";
//     beginstring_Minus[9]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets2nMtags/Minus";
//     beginstring_Minus[10]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets3nMtags/Minus";
//     beginstring_Minus[11]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets4nMtags/Minus";
    
//     TString beginstring_Plus[12];
//     beginstring_Plus[0]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets2nMtags/Plus";
//     beginstring_Plus[1]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets3nMtags/Plus";
//     beginstring_Plus[2]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets4nMtags/Plus";
//     beginstring_Plus[3]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets2nMtags/Plus";
//     beginstring_Plus[4]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets3nMtags/Plus";
//     beginstring_Plus[5]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets4nMtags/Plus";
//     beginstring_Plus[6]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets2nMtags/Plus";
//     beginstring_Plus[7]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets3nMtags/Plus";
//     beginstring_Plus[8]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets4nMtags/Plus";
//     beginstring_Plus[9]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets2nMtags/Plus";
//     beginstring_Plus[10]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets3nMtags/Plus";
//     beginstring_Plus[11]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets4nMtags/Plus";

//         TString beginstring_Nominal[12];
//     beginstring_Nominal[0]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets2nMtags/Nominal";
//     beginstring_Nominal[1]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets3nMtags/Nominal";
//     beginstring_Nominal[2]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents6nJets4nMtags/Nominal";
//     beginstring_Nominal[3]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets2nMtags/Nominal";
//     beginstring_Nominal[4]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets3nMtags/Nominal";
//     beginstring_Nominal[5]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents7nJets4nMtags/Nominal";
//     beginstring_Nominal[6]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets2nMtags/Nominal";
//     beginstring_Nominal[7]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets3nMtags/Nominal";
//     beginstring_Nominal[8]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents8nJets4nMtags/Nominal";
//     beginstring_Nominal[9]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets2nMtags/Nominal";
//     beginstring_Nominal[10]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets3nMtags/Nominal";
//     beginstring_Nominal[11]="MultiSamplePlot_BDT_Mu3rdJune_adaBoost_alphaSTune_subAll_noMinEvents9nJets4nMtags/Nominal";    
//     TH1F *workhist = (TH1F*) file->Get(beginstring_Nominal[0]);
//     double lowval =workhist->GetBinLowEdge(1);

//     int nbins = workhist->GetNbinsX();
//     double width = nbins*workhist->GetBinWidth(1);
//     TH1F *newhist = new TH1F("summary_"+samplestring,"",12*nbins,lowval,lowval+(12*width));// histogram that contains all bins;
//     newhist->SetLineColor(sColour);
//     for(int ihist=0; ihist<12; ihist++){
//         TH1F *workhist = (TH1F*) file->Get(beginstring[ihist]+samplestring);
//         // workhist->Scale(1.0/workhist->Integral());

//         int binworker = nbins*ihist;
//         for(int ibin=1; ibin<=nbins; ibin++){
//             newhist->SetBinContent(binworker+ibin,workhist->GetBinContent(ibin));
//             // newhist->SetBinError(binworker+ibin,workhist->GetBinError(ibin));
            
//         }
        
//     }
//     TCanvas *canv = new TCanvas();
//     canv->Draw();
//     // newhist->Scale(1.0/newhist->Integral());

//     newhist->Draw();
//     return newhist;
//     // after this you can do some file operations like saving in a new alternative file.
// }

void makesummaryplot(void){
    
    TH1F *histotttt = worker("__NP_overlay_ttttNLO__nominal",kGreen);
    TH1F *histoTTB = worker("__ttbarPlusTTX__nominal",kBlue);
    TH1F *histotttt_orig = worker("__NP_overlay_ttttNLO__nominal",kGreen);
    TH1F *histoTTB_orig = worker("__ttbarPlusTTX__nominal",kBlue);
    TH1F *histottbar = worker("__TTJets_powheg__nominal",kRed);
    TH1F *histottbarMESup = worker("__TTJets_powheg__ttMEScaleUp",kRed-9);
    TH1F *histottbarMESdown = worker("__TTJets_powheg__ttMEScaleDown",kRed-9);
    TH1F *histottbarMESup_orig = worker("__TTJets_powheg__ttMEScaleUp",kRed-9);
    TH1F *histottbarMESdown_orig = worker("__TTJets_powheg__ttMEScaleDown",kRed-9);

    TH1F *histoTTH = worker("__TTH__nominal",kMagenta);
    TH1F *histoTTH_orig = worker("__TTH__nominal",kMagenta);
    // TH1F *histottbar_orig = worker("__TTJets_powheg__nominal",kRed);
    // etc etc

    TCanvas *c1 = new TCanvas("c","c",1400, 600);
    TPad *pad1 =  new TPad("pad1","pad1",0,0.45,1,1);
    c1->cd();
    pad1->SetBottomMargin(0);
    pad1->Draw();
    pad1->cd();
    histottbar->SetStats(0);
    histotttt_orig->SetStats(0);
    histottbarMESup_orig->SetStats(0);
    histottbarMESdown_orig->SetStats(0);
    histoTTH_orig->SetStats(0);
    histoTTB_orig->SetStats(0);

    pad1->SetLogy();
    histottbar->Draw();
    histotttt_orig->Add(histottbar,1);
    histotttt->Add(histottbar,1);
    histoTTH_orig->Add(histottbar,1);
    histoTTH->Add(histottbar,1);

    histoTTH_orig->Draw("same");
    histoTTB_orig->Draw("same");
    histotttt_orig->Draw("same");
    histottbarMESup_orig->Draw("same");
    histottbarMESdown_orig->Draw("same");

    TLegend *leg_ttbar = new TLegend(0.75,0.8,0.9,1.0);
    leg_ttbar->AddEntry(histottbar,"ttbar","l");
    leg_ttbar->AddEntry(histoTTB,"ttbar_TTB","l");
    leg_ttbar->AddEntry(histoTTH,"ttbar_TTH","l");
    leg_ttbar->AddEntry(histotttt,"ttbar_tttt","l");
    leg_ttbar->AddEntry(histottbarMESup,"MEscale up/down","l");
    leg_ttbar->Draw();

    c1->cd();
    TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.45);

    pad2->SetTopMargin(0);
    pad2->SetGridy();
    pad2->Draw();
    pad2->cd();



    histottbar->SetTitle("");

    histotttt->SetStats(0);
    histotttt->GetYaxis()->SetLabelSize(0.1);
    histotttt->Add(histottbar,-1);
    histotttt->Divide(histottbar);
    histotttt->SetMarkerStyle(1);


    histotttt->Draw();

    histoTTB->SetStats(0);
    histoTTB->Add(histottbar,-1);
    histoTTB->Divide(histottbar);
    histoTTB->Draw("same");

    histoTTH->SetStats(0);
    histoTTH->Add(histottbar,-1);
    histoTTH->Divide(histottbar);
    histoTTH->Draw("same");

    histottbarMESup->SetStats(0);
    histottbarMESup->Add(histottbar,-1);
    histottbarMESup->Divide(histottbar);
    histottbarMESup->Draw("same");

    histottbarMESdown->SetStats(0);
    histottbarMESdown->Add(histottbar,-1);
    histottbarMESdown->Divide(histottbar);
    histottbarMESdown->Draw("same");

    c1->cd();

    // Text *t = new Text(0.5,0.5,"Hello World !");
    // // t->SetTextAlign(22);
    // // t->SetTextColor(kRed+2);
    // t->SetTextFont(43);
    // // t->SetTextSize(40);
    // // t->SetTextAngle(45);
    // t->Draw("same");
    

   // TPaveText *pt = new TPaveText(.05,.1,0.95,0.95,"NDC");
   // pt->AddText("A TPaveText can contain severals line of text.");
   // pt->AddText("They are added to the pave using the AddText method.");
   // pt->AddLine(.0,.5,1.,.5);
   // pt->AddText("Even complex TLatex formulas can be added:");
   // pt->AddText("F(t) = #sum_{i=-#infty}^{#infty}A(i)cos#[]{#frac{i}{t+i}}");
   // pt->Draw();


    c1->Update();
    c1->SaveAs("summaryTTBtttt.pdf");

    // TCanvas *c2= new TCanvas("c2","c2",1400, 600);
    // histotttt->GetYaxis()->SetLabelSize(0.05);

    // TLegend *leg2 = new TLegend(0.1,0.7,0.3,0.9);
    // leg2->AddEntry(histoTTB,"ttbar_TTB","l");
    // leg2->AddEntry(histoTTH,"ttbar_TTH","l");
    // leg2->AddEntry(histotttt,"ttbar_tttt","l");

    // histotttt->Draw();
    // histoTTB->Draw("same");
    // histoTTH->Draw("same");
    // histottbarMESdown->Draw("same");
    // histottbarMESup->Draw("same");
    // leg2->Draw();

    // c2->SaveAs("summaryTTBetc.pdf");



}
