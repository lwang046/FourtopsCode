## IDEAS
## The split canvas should allow ratios, efficiencies, or differences

import argparse
import sys
import textwrap

class CustomHelpAction(argparse.Action):
    """
    Action that mimics argparse._HelpAction.

    If *dest* is 'help', it gives a general help message, showing a
    basic set of the most useful options.  When *dest* is set to
    something like 'legend_help', it suppresses usage information and
    normal help messages while enabling help messages for a detailed
    list of extra commands within that group.
    """
    def __init__(self,
                 option_strings,
                 dest=argparse.SUPPRESS,
                 default=argparse.SUPPRESS,
                 help=None):
        super(CustomHelpAction, self).__init__(
            option_strings=option_strings,
            dest=dest,
            default=default,
            nargs=0,
            help=help)
    def __call__(self, parser, namespace, values, option_string=None):
        thisgroup = self.dest.replace('_help', '')
        help_groups = []
        for action in parser._actions:
            if action.dest.endswith('_help'):
                help_groups.append(action.dest.replace('_help', ''))
                action.help = argparse.SUPPRESS
        if thisgroup == 'help':
            for action in parser._actions:
                for group in [x for x in help_groups if x != thisgroup]:
                    if action.dest != group and action.dest.startswith(group):
                        action.help = argparse.SUPPRESS
        else:
            parser.usage = argparse.SUPPRESS
            parser.description = argparse.SUPPRESS
            for action in parser._actions:
                if not action.dest.startswith(thisgroup):
                    action.help = argparse.SUPPRESS
        parser.print_help()
        parser.exit()



class CustomArgumentParser(argparse.ArgumentParser):
    """Argument parser that adds some new options to add_argument."""
    def add(self, *args, **kwargs):
        if 'metadefault' in kwargs:
            metadefault = kwargs.pop('metadefault')
            kwargs['metavar'] = metadefault
            kwargs['default'] = metadefault
        return self.add_argument(*args, **kwargs)
    


class Target(object):
    """Container to hold a root file name, directory, and other options."""
    def __init__(self, file, dir='/', scale=1.):
        self.file = file
        self.dir = dir
        self.scale = scale
    def __repr__(self):
        args = self.__dict__
        for key in [x for x in args if isinstance(args[x], str)]:
            args[key] = "'%s'" % args[key]
        return "Target(%s, %s)" % (
            args.pop('file'),
            ', '.join(["%s=%s" % (x, args[x]) for x in args])
            )


def clifunc(
    targets,
    legend=True, legend_top=0., legend_bottom=0.,
    ratio=False, split=False, ratio_fraction=0.3125,
    ):
    """We are clifunc."""
    pass


if __name__ == "__main__":

    ## Fetch defaults for the parser from the API.
    varnames = [x for x in clifunc.func_code.co_varnames if x != "targets"]
    vardefaults = clifunc.func_defaults
    defaults = {}
    for i, (varname, vardefault) in enumerate(zip(varnames, vardefaults)):
        defaults[varname] = vardefault

    usage = textwrap.dedent("""\
    
        %(prog)s file1.root [file2.root ...] [options]             (1st form)
        %(prog)s file1.root [file2.root ...] path [options]        (2nd form)
        %(prog)s file1.root path1 [path2 ...] [options]            (3rd form)
        %(prog)s file1.root:path1 [file2.root:path2 ...] [options] (4th form)

    In each of the 4 forms above, the arguments will be used to generate a set of
    'targets' which will be overlaid or stacked in the plots.  For the 1st form,
    all plots in the files will be processed.  For the other forms, the 'paths'
    should specify the locations of directories or plots within the root files.

    Call with '-h' for a list of options.
    
    Full documentation is available at http://packages.python.org/rootplot/
    """)

    ## Parse the arguments from the command-line.
    parser = CustomArgumentParser(
        description="Test Jeff's crazy ideas for generating useful messages.",
        usage=usage, add_help=False,
        )
    parser.add('args', nargs='+', help=argparse.SUPPRESS)
    parser.add('-h', '--help', action=CustomHelpAction,
               help="show a relatively simple help message and exit")
    parser.add('-hh', '--fullhelp', action='help',
               help="show an exhaustive help message and exit")
    parser.add('--legend', type=bool, help="Decide whether or not a legend "
               "should be drawn.  Call '--legend-help' for extra options.")
    parser.add('--color', nargs='+',
               help="a list of colors to use for the various targets.  Accepts "
               "matplotlib colors, root colors, etc.")
    parser.add('--fill', nargs='*',
               help="fill the plots.")
    parser.add('--legend-help', action=CustomHelpAction)
    parser.add('--legend-top', default=None,
               help="Specify the top coordinate of the legend.")
    parser.add('--legend-bottom', default=None,
               help="Specify the bottom coordinate of the legend.")
    parser.add('--legend-left', default=None,
               help="Specify the left coordinate of the legend.")
    parser.add('--legend-right', default=None,
               help="Specify the right coordinate of the legend.")
    parser.add('--ratio', metavar='INDEX',
               help="divide histograms by the specified target (indexed "
               "from 1).  Call '--ratio-help' for extra options.")
    parser.add('--ratio-help', action=CustomHelpAction)
    parser.add('--ratio-split', action='store_true',
               help="same as --ratio, but splits the figure in two.")
    parser.add('--ratio-fraction', metadefault=defaults['ratio_fraction'],
               help="fraction of the canvas taken up by the ratio plot.")
    parser.add('--ratio-max', metavar='MAX',
               help="upper y-axis limit for the split ratio plot.")
    parser.add('--ratio-min', metavar='MIN',
               help="lower y-axis limit for the split ratio plot.")
    args = parser.parse_args()

    ## Parse the non-option arguments to build targets.
    files, configs, dirs, filedirpairs = [], [], [], []
    for arg in args.args:
        if arg.endswith('.root'):
            files.append(arg)
        elif arg.endswith('.py'):
            configs.append(arg)
        elif ':' in arg:
            filedirpairs.append(arg)
        else:
            dirs.append(arg)
    if dirs:
        if len(dirs) > 1:
            parser.error("More than one dir specified")
        if filedirpairs:
            parser.error("Cannot specify dir when specifying file.root:/dir")
        args.targets = [Target(x, dirs[0]) for x in files]
    elif filedirpairs:
        if files:
            parser.error("When specifying file.root:/dir, all targets must "
                         "specify a directory")
        args.targets = [Target(*x.split(':')) for x in filedirpairs]
    else:
        args.targets = [Target(x) for x in files]


    print args.targets
