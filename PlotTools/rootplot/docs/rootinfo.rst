.. rootplot documentation master file, created by
   sphinx-quickstart on Mon Mar  1 13:54:08 2010.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. module:: rootinfo
   :synopsis: Fast listing of ROOT file contents.
.. moduleauthor:: Mike Anderson <mbanderson@wisc.edu>

Fast Listing of ROOT File Contents
==================================

The simple :mod:`rootinfo` module is a quick way to get text output about the contents of your ROOT file, best explained by example::

    $ rootinfo Hists_data_fisher.root --entries --max
    barrel15to20/pho_fisher  7153  735 
    barrel15to20/pho_ecalIso04  7153  1766 
    barrel15to20/pho_hcalIso04  7153  5232 
    barrel15to20/pho_trackHollowIso  7153  1716 
    barrel15to20/pho_totIso  7153  649 
    barrel20to30/pho_fisher  3425  268 
    barrel20to30/pho_ecalIso04  3425  741 
    barrel20to30/pho_hcalIso04  3425  2277 
    barrel20to30/pho_trackHollowIso  3425  683 
    barrel20to30/pho_totIso  3425  246 
    barrel30to50/pho_fisher  810  68 
    barrel30to50/pho_ecalIso04  810  160 
    barrel30to50/pho_hcalIso04  810  495 
    barrel30to50/pho_trackHollowIso  810  181 
    barrel30to50/pho_totIso  810  60 
    barrel50to80/pho_fisher  99  10 
    barrel50to80/pho_ecalIso04  99  14 
    barrel50to80/pho_hcalIso04  99  52 
    barrel50to80/pho_trackHollowIso  99  21 
    barrel50to80/pho_totIso  99  9 
    pho_fisher  1425  138 
    pho_ecalIso04  1425  323 
    pho_hcalIso04  1425  1017 
    pho_trackHollowIso  1425  326 
    pho_totIso  1425  124 


The full options can be accessed by running ``rootinfo --help``:

.. literalinclude:: rootinfohelp.txt
