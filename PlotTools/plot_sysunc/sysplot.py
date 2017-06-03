#!/usr/bin/env python

"""
Draw systematic comparison in multiple bins.
"""

import os
import sys
import time
import argparse
import logging
import json
from pprint import pprint, pformat

from ROOT import TFile
from ROOT import gStyle

from plotter import *

def global_style():
	gStyle.SetTitleSize(0.2,"t")
	pass

def main(arguments):
	total = 1000
	i = 0

	json_dic = {}
	if arguments.config_json is not None:
		with open (arguments.config_json) as configuration_file:
			json_dic = json.load(configuration_file)
		logging.debug(pformat(json_dic))


	global_style()
	p = plotter(format_plot=arguments.format_plot) 
	if arguments.infile:
		p.set_file(arguments.infile)
	if arguments.ratio_reference:
		p.set_reference_file(arguments.ratio_reference)
	p.make_pages(json_dic)

        return 0

if __name__ == '__main__':
        start_time = time.time()

        parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
        parser.add_argument('infile', help="Input file")
        parser.add_argument('-f', '--format-plot', help="Output file format [.C, .root, .png, .pdf]", default=".png")
        parser.add_argument('-j', '--config-json', help="JSON configuration file")
        parser.add_argument('-r', '--ratio-reference', help="FILE for the denominator in the ratio plot")
        parser.add_argument(
                        '-d', '--debug',
                        help="Print lots of debugging statements",
                        action="store_const", dest="loglevel", const=logging.DEBUG,
                        default=logging.WARNING,
                        )
        parser.add_argument(
                        '-v', '--verbose',
                        help="Be verbose",
                        action="store_const", dest="loglevel", const=logging.INFO,
                        )
        parser.add_argument('-b', dest='batch', action='store_true', help="ROOT batch mode")

        args = parser.parse_args(sys.argv[1:])
	if args.batch:
		sys.argv.append( '-b-' )

        print(args)
        
        logging.basicConfig(level=args.loglevel)

        logging.info( time.asctime() )
        exitcode = main(args)
        logging.info( time.asctime() )
        logging.info( 'TOTAL TIME IN MINUTES:' + str((time.time() - start_time) / 60.0))
        sys.exit(exitcode)
