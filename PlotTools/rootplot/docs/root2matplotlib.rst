.. rootplot documentation master file, created by
   sphinx-quickstart on Mon Mar  1 13:54:08 2010.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. module:: root2matplotlib
   :synopsis: Library for plotting ROOT histograms in matplotlib
.. moduleauthor:: Jeff Klukas <klukas@wisc.edu>

Beautifying ROOT with matplotlib
==========================================
The goal of |root2matplotlib| is to enable easy plotting of ROOT histograms using the full-featured and mature |matplotlib| library.

Some possibilities in matplotlib that are unavailable in ROOT include transparent fills and text output using LaTeX.

You may want to use |root2matplotlib| to achieve effects or complex diagrams that would be difficult or impossible in ROOT, or you may simply want to recreate a ROOT figure with the higher-quality text available through a LaTeX engine.

For immediate figures with a minimum of effort (with output directly from ROOT or through |matplotlib|, take a look at the section on the |rootplot| command-line tool.

.. _root2matplotlib-examples:

Examples
-----------------------------------------
First plot
^^^^^^^^^^
A first example:

.. plot:: pyplots/first.py
   :include-source:

So far, this is probably not the prettiest output you've seen, but we're relying on all of |matplotlib|'s defaults.  The real power of the solution is the amount of customization that's possible, and how comparitively easy it is to achieve the results you desire.

Notice that ROOT's simplified TeX-esque syntax doesn't isn't understood by |matplotlib|.  By default, matplotlib uses an internal typesetting engine that allows simple expressions following the official TeX syntax, using ``$`` to denote math mode and ``\`` characters to start commands.  To handle conversion from the names you gave the histograms in ROOT to names that are compatible with matplotlib, you can provide a list of ordered pairs in the ``show_titles`` method with the ``replacements`` keyword::

    replacements = [('p_{x}', r'$p_x$'),
                    ('#eta', r'$\eta$')]
    hist.bar(color='r')
    hist.show_titles(replacements=replacements)

This will replace all instances of ``p_{x}`` with ``$p_x$``, generating the correct LaTeX expressions, and you should give it a try.  Notice the ``r`` prefix on the second string, which denotes a "raw" string.  In this case, it doesn't matter, but if the LaTeX string were to contain any commands using ``\``, a normal string would interpret these as escaped characters.

Now, let's add the replacements above and make a more ROOT-style filled histogram with a single line along the top of the bins:

.. plot:: pyplots/firstrep.py
   :include-source:   

A more complicated example
^^^^^^^^^^^^^^^^^^^^^^^^^^
Imagine that you are preparing a LaTeX document using the ``mathpazo`` package, which sets up Palatino as the default font.  With |matplotlib|, you can generate an output PS or PDF that has all its text rendered with the full-blown LaTeX distribution on your system, so that your plots match exactly with the rest of your document.  The following example loads the PDF backend, and enables the ``text.usetex`` option to tell |matplotlib| to use your real LaTeX engine.  It also highlights some extra matplotlib functions, like transparent fills, legends, and colormaps:

.. plot:: pyplots/second.py
   :include-source:

A Two-Dimensional Histogram
^^^^^^^^^^^^^^^^^^^^^^^^^^^
|root2matplotlib| also supports 2D histograms.  A Hist2D object has functions such as ``box``, ``col``, or ``colz`` to replicate ROOT drawing options, but also the ability to make ``contour`` plots:

.. plot:: pyplots/demo2D.py
   :include-source:

Navigating Within a ROOT file
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
In real life, you won't be making random Gaussian distributions from within python.  Instead, you'll be pulling histograms from an existing ROOT file.  The |root2matplotlib| module includes a ``RootFile`` class as a wrapper to provide easier, more pythonic access to your files.

In the following example, we open a file, navigate to a directory, and get a histogram as a ``Hist`` object::

    from matplotlib import pyplot as plt
    import ROOT
    import root2matplotlib as r2m

    f = r2m.RootFile("histograms.root")
    f.cd("muons")

    print r2m.pwd()
    print r2m.ls()

    hist = r2m.get("muon_pt")
    hist.bar()

    plt.show()

The next example shows an efficient way of getting several histograms from a file and plotting them in a grid (download :download:`the ROOT file <_static/ratios.root>`):

.. plot:: pyplots/grid.py
   :include-source:

The root2matplotlib Library
------------------------------------------
.. automodule:: rootplot.root2matplotlib
   :members:

