import rootplot.root2matplotlib as r2m
import ROOT
from matplotlib import pyplot as plt
from random import gauss

th2f = ROOT.TH2F("data", "", 20, -3, 3, 20, -3, 3)
for i in range(20000):
    th2f.Fill(gauss(0., 1.), gauss(0., 1.))

hist = r2m.Hist2D(th2f)
fig = plt.figure(figsize=[6,3])
ax1 = plt.subplot(121)
hist.colz()
ax2 = plt.subplot(122)
hist.contour()

plt.show()
