.. rootplot documentation master file, created by
   sphinx-quickstart on Mon Mar  1 13:54:08 2010.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. module:: tree2hists
   :synopsis: Easily make histograms from TTrees and TNtuples
.. moduleauthor:: Mike Anderson <mbanderson@wisc.edu>

Easily Make Histograms from TTrees and TNtuples
===============================================

The script tree2hists allows a user to easily create a root file containing histograms filled with data from one or more ROOT TTrees or TNtuples.  For each source file, different cuts and scale factors can be specified.  Plots can be made for "sets of cuts" that allow for easy comparison of plots when different cuts are used.

All of these and more are specified in a configuration file.

Running
------------------------------------------
Running tree2hists is a 2-step processes.  First you must create and edit a configuration file that tells tree2hists where to find the data, and what to do with it.  Then, you can run tree2hists by passing it the name of this configuration file:

    $ tree2hists config.py

That's it.  See below for information on how to create & edit the configuration file.


Creating a Configuration File
------------------------------------------
To create a default tree2hists config file, simply type ``tree2hists``::

    $ tree2hists
    Created default configuration file: t2h_config.py
    Edit it, then run by typing:
     tree2hists t2h_config.py

The default config file, ``t2h_config.py``, is created and contains 5 variables the user should change:

* ``list_of_files``
* ``output_filename``
* ``cut_for_all_files``
* ``cut_sets``
* ``list_of_plots``

Each of those variables must be found in the config file, or :mod:`tree2hists` will print a warning and not run.  Typical examples of these variables are shown below.

Editing the Configuration File
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The first parameter is ``list_of_files`` and tells tree2hists where to find the TTrees (or TNtuples), how to scale them, and any cuts to apply.  For example::

    list_of_files = [
        RootTree("Analysis", fileName="files/PhotonJet_Pt15.root", scale=0.0168097, cuts="ptHat>15&&ptHat<30"),
        RootTree("Analysis", fileName="files/PhotonJet_Pt30", scale=0.00255086, cuts="ptHat>30&&ptHat<80")
    ]

Each element in the list is a ``RootTree`` object which simply takes in the name of the TTree/TNtuple (which is "Analysis" in this case), the name of the file it's in, a scale, and any cuts to be applied only to that TTree.

The variable ``cut_for_all_files`` is a string of cuts to be applied to all given TTrees.  They must correspond to variables in the TTree::

    cut_for_all_files = "(!vtxIsFake && vtxNdof>4 && abs(vtxZ)<=15)"

The variable ``cut_sets`` is optional.  It is a list of tuples, and each tuple specifies a "cut set" for which to make all plots.  The tuple is of the form ``(<dir name>, <optional title suffix>, <string of cuts>)``

For example, if a user specified::

    cut_sets = [
        ("barrel15to20", "(|#eta|<1.45, 15<E_{T}<20)", "et>15&&et<20&&abs(eta)<1.45"),
        ("endcap15to20", "(1.7<|#eta|<2.5, 15<E_{T}<20)", "et>15&&et<20&&abs(eta)>1.7&&abs(eta)<2.5")
        ]

then all plots would be drawn for the two given cut sets and placed into their respective folders, ``barrel15to20`` and ``endcap15to20``.  The middle string is appended to the title for each of those plots (it is optional).  The 3rd (last) element specifies the cuts.

Finally, plots are specified in the ``list_of_plots`` variable.  This is a
list of ``Plot`` objects which take 2 arguments: ``(<tree variable>, <histogram to draw into>)``

Here is a simple example ``list_of_plots``::

    list_of_plots = [
        Plot("et"           , TH1F("pho_et"           , "Lead #gamma: E_{T};E_{T} (GeV);entries/bin", 25, 0.0, 100.0)),
        Plot("eta"          , TH1F("pho_eta"          , "Lead #gamma: #eta;#eta;entries/bin"        , 25, -3.0, 3.0)),
        Plot("sigmaIetaIeta", TH1F("pho_sigmaIEtaIEta", "Lead #gamma: #sigma_{i#etai#eta};#sigma_{i#etai#eta};entries/bin",20, 0, 0.06)),
        Plot("metEt/et"     , TH1F("metEt_over_phoEt" , "MET / E_{T}(#gamma);MET/E_{T}(sc);entries/bin"   , 20, 0.0, 3.0)),
        Plot("phi:eta"      , TH2F("phoPhi_vs_phoEta" , "Lead #gamma: #phi vs #eta;#eta;#phi"             , 50, -2.5, 2.5, 18, -3.14, 3.14))
        ]

The first strings (like ``et``, ``phi:eta``) must be variables found in the given TTrees.  Math can be performed between variables (like ``metEt/et``).

The next argument to Plot() is the histogram to plot into. It can be any ROOT histogram: TH1F, TH2F, etc...

Advanced Configuration File Examples
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
To create histogram with variable-sized bins one has to use an ``array``::

    from array import array  # This is already in default example t2h_config.py
    bins_et     = array("f", [15.0, 20.0, 30.0, 50.0, 80.0, 120.0])
    list_of_plots = [
        Plot("et", TH1F("pho_et_binned", "Lead #gamma: E_{T};E_{T} (GeV);entries/bin", len(bins_et)-1, bins_et)),
        ]

The ``array`` function stores a python list of numbers in memory in the format ROOT needs.  The "f" within ``array()`` specifies ``float``.


To have one or more cuts apply only to a specific plot, pass one more string argument::

    list_of_plots = [
        Plot("et", TH1F("pho_et"  , "Lead #gamma: E_{T};E_{T} (GeV);entries/bin", 25, 0.0, 100.0), "abs(eta)<1.5"),
        Plot("eta", TH1F("pho_eta", "Lead #gamma: #eta;#eta;entries/bin"        , 25, -3.0, 3.0)),
        ]

In the example above, ``et`` will be plotted with an extra cut that the ``eta`` plot won't have.


How tree2hists works
------------------------------------------
Essentially, tree2hists creates ROOT histograms in one output ROOT file using scales, input files, and cuts specified by the user.

The very core of tree2hists is simply a ROOT Draw() command::

    all_cuts = join_cuts(cut_for_all_files, rootTree.cuts,
                         current_cut_set, plot.cuts) # Set cuts
    rootTree.ttree.Draw( "%s >> temp" % plot.treeVariable, all_cuts,
                                 "goff")

These two lines above are located inside two nested for-loops.  The outer loop is a "loop over all input files", and the inner loop is a "loop over all plots to Draw()".

At any given moment, only one input ROOT file is open and being used to draw into histograms.  (This results in far faster execution time.)

The ``join_cuts()`` function simply connects strings with a ``&&`` symbol.
