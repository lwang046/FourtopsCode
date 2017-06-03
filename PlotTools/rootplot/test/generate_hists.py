import ROOT
from random import gauss
import array

for i in xrange(1,10):
    outfile = ROOT.TFile("hists%i.root" % i, "recreate")
    empty = ROOT.TH1I("hempty", "No Entries Here;Random Val;Other", 40, 0, 4)
    same = ROOT.TH1F("hsame", "Gausses with Same Means", 40, -4, 4)
    half = ROOT.TH1F("hhalf", "Half of same", 40, -4, 4)
    quart = ROOT.TH1F("hquart", "Quarter of same", 40, -4, 4)
    mean = ROOT.TH1F("hmean", "Gausses with Different Means", 40, -4, 14)
    orders = ROOT.TH1F("horders", "Orders of Magnitude", 40, -4, 14)
    corr2D = ROOT.TH2F("hcorr2D", "Two Dimensions!", 40, -4, 4, 40, -4, 4)
    labeled = ROOT.TH1F("hlabeled", "Labeled Plots", 10, 0, 100)
    for j in range(4):
        d = outfile.mkdir("dir%i" % j)
        d.cd()
        hist = ROOT.TH1F("hist", "Dull", 10, 0, 10)
        hist.Fill(i + j)
        hist.Write()
        del hist
    for j in range(6):
        f = d.mkdir("dir%i" % j)
        f.cd()
        hist = ROOT.TH1F("hist", "Dull", 10, 0, 10)
        hist.Fill(i + j)
        hist.Write()
        del hist
    for j in range(1, 11):
        labeled.GetXaxis().SetBinLabel(j, "Label %i" % j)
    for j in range(25000):
        x = gauss(0., 1.)
        y = gauss(0., 1.)
        same.Fill(x)
        half.Fill(x, 0.5)
        quart.Fill(x, 0.25)
        mean.Fill(i + x)
        orders.Fill(i + x, 10 ** i)
        corr2D.Fill(x, y)
        labeled.Fill(x * 5)
    if True:
        ## Make a TGraph
        n = 10
        x = array.array('f', range(n))
        y = array.array('f', [gauss(4., 1.) for j in range(n)])
        tgraph = ROOT.TGraph(n, x, y)
        tgraph.SetTitle("A Graph;x title; y title")
        outfile.Append(tgraph)
    outfile.Write()
    outfile.Close()
