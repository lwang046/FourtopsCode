.. rootplot documentation master file, created by
   sphinx-quickstart on Mon Mar  1 13:54:08 2010.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. module:: rootplot
   :synopsis: Making ROOT plotting fun again
.. moduleauthor:: Jeff Klukas <klukas@wisc.edu>

.. module:: rootplotmpl
   :synopsis: Making ROOT plotting fun again
.. moduleauthor:: Jeff Klukas <klukas@wisc.edu>

Complex Plotting the Fun Way
===========================================

The basic philosophy of |rootplot| (stolen from :mod:`matplotlib`) is to make easy things easy and hard things possible.  In our case, this means taking away the tedious process of writing a custom macro every time you want to visualize your ROOT histograms.  Wouldn't it be easier if you could call a command that would do the tedious work for you and handle in a simple way all of the most common manipulations that physicists want to make?

Of course it would!

|rootplot| is ...
  ...a way to rapidly visualize your data and try out different layouts
    You can make a webpage containing images of all your histograms by a quick call to ``rootplot histograms.root``, area-normalize them by adding the ``-n`` option, and change the title with ``--title="My Fancy Title"``.
  ...the starting point for your perfect plot
    You can control most aspects of your plots directly from the command-line, but when you decide you need to add a fit to the data and overlay pictures of rainbows and unicorns, |rootplot| can output a python script or a ROOT ``.C`` macro to give you a jumpstart on your masterpiece.
  ...a tool that meets you where you are
    Although |rootplot| is a pure-python module, no familiarity with :mod:`python` or |pyROOT| is necessary.  You can work completely from the command-line, utilize the python API, or export ``.C`` macros.

The application is also a great way to test out the formidable plotting capabilities of :mod:`matplotlib`.  For that job, you'll need mod:`rootplot`'s more elegant twin, :mod:`rootplotmpl`, which shares the same interface, but produces output in :mod:`matplotlib` through use of the |root2matplotlib| library.

A full list of available options for ROOT output can be accessed by calling ``rootplot -h``:

.. literalinclude:: rootplothelp.txt

And the list of options for output with |matplotlib| can be accessed by calling ``rootplotmpl -h``:

.. literalinclude:: rootplotmplhelp.txt


The Application Programming Interface
-------------------------------------

.. automodule:: rootplot.core
   :members: plot, plotmpl, rootplot, rootplotmpl

Customization of rootplot
------------------------------------------
Most of the style options for your output can be specified in a `rootlogon.C <http://root.cern.ch/drupal/content/how-create-or-modify-style>`_ or `matplotlibrc <http://matplotlib.sourceforge.net/users/customizing.html>`_ depending on which kind of output you'd like.  Documentation on this kind of configuration can be found on the websites of ROOT and |matplotlib| respectively.

There are, however, several style options which are specific to |rootplot|.  These include the line colors and markers used, as well as the dictionary used for text replacement with |matplotlib|.  If you'd like to customize these for your own use, simply make a configuration file with a `.py` extension and add the file to your list of arguments to |rootplot|.  You can get a nice default with all the configurable parameters by calling::

    $ rootplot --config
