import rootplot.root2matplotlib as r2m
import ROOT
from matplotlib import pyplot as plt
from random import gauss

th1f = ROOT.TH1F("hpx", "Distribution of p_{x};p_{x};Events", 40, -4, 4)
for i in range(25000):
    th1f.Fill(gauss(0., 1.))

# Make a figure with width 6 inches and height 4 inches
plt.figure(1, (8, 6))
# Create an axes instance
ax1 = plt.axes()
hist = r2m.Hist(th1f)
# Plot a bar chart in red
hist.bar(color='r')
hist.show_titles()

plt.savefig('first')
plt.show()
