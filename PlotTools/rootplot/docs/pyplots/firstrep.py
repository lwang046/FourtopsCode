import rootplot.root2matplotlib as r2m
import ROOT
import matplotlib
from matplotlib import pyplot as plt
from random import gauss

th1f = ROOT.TH1F("hpx", "Distribution of p_{x};p_{x};Events", 40, -4, 4)
for i in range(25000):
    th1f.Fill(gauss(0., 1.))

plt.figure()
replacements = [('p_{x}', r'$p_x$'),
           ('#eta', r'$\eta$')]
hist = r2m.Hist(th1f)
hist.hist(color='r', histtype='stepfilled')
hist.show_titles(replacements=replacements)
plt.savefig('firstrep', dpi=50)
plt.show()
