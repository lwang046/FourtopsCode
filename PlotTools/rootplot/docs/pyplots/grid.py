import matplotlib
matplotlib.rcParams['figure.subplot.wspace'] = 0.0 # no space between subplots
matplotlib.rcParams['figure.subplot.hspace'] = 0.0 # no space between subplots
import matplotlib.pyplot as plt
import pylab
import ROOT
import rootplot.root2matplotlib as r2m

# Get histograms from ROOT file
f = r2m.RootFile("../_static/ratios.root")
hists = [f.get("00_co_cs"),
         f.get("01_fo_fs"),
         f.get("02_cs_fs"),
         f.get("03_co_fs"),
         f.get("04_cs_fo"),
         f.get("05_co_fo")]

fig_main = plt.figure(1, (5.0, 6.0))
ax_list = []

for i in range(6):
    ax_list.append(fig_main.add_subplot(3, 2, i + 1))
    hists[i].errorbar(fmt='o', yerr=True, color='blue')
    plt.ylim(ymin=0, ymax=2.5)
    pylab.yticks(pylab.arange(0.0, 2.5, 0.5))

# Set x-axis labels
for i in [4, 5]:
    ax_list[i].set_xlabel(r"$p_{\mathrm{T}}^\gamma$")

# Turn off axis labels for all but bottom plot
for i in [0, 1, 2, 3]:
    pylab.setp(ax_list[i].get_xticklabels(), visible=False)
for i in [1, 3, 5]:
    pylab.setp(ax_list[i].get_yticklabels(), visible=False)

plt.savefig("grid", transparent=False)
