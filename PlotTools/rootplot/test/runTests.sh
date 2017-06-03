mvmerge () {
    echo $2 $3 $4 $5 $6 $7 $8 $9
    rootplot $2 $3 $4 $5 $6 $7 $8 $9 --merge --output=$1
    rm -r $1
}

mvmergempl () {
    echo $2 $3 $4 $5 $6 $7 $8 $9
    rootplotmpl $2 $3 $4 $5 $6 $7 $8 $9 --merge --output=$1
    rm -r $1
}

#### Generic ##############################################

rootplot --config

#### ROOT #################################################

# General Test
rootplot hists3.root --numbering

# 2D Tests
COMMON='--numbering --path=hcorr2D'
rootplot hists3.root $COMMON --draw2D="colz"
rootplot hists3.root $COMMON --draw2D="cont0"

# Output formats
rootplot hists6.root --numbering --ext="pdf"
rootplot hists6.root --numbering --ext="png"
rootplot hists6.root --numbering --ext="gif"
rootplot hists6.root --numbering --ext="ps"
rootplot hists6.root --numbering --merge

# Histogram types
mvmerge rootbar hists6.root --numbering --draw="b"
mvmerge rootlp hists6.root --numbering --draw="lp" --legend-location=lowercenter

# Other
mvmerge rootcomp hists1.root hsame hmean
mvmerge roottitles hists1.root hsame hmean --legend-entries="Same,Mean"
mvmerge rootratio hists6.root hists7.root hists8.root --numbering --ratio=1
mvmerge rootlog hists6.root --numbering --logx --logy
mvmerge rootoverflow hists6.root --numbering --ymin=0 --overflow --underflow

#### matplotlib ############################################

# General Test
rootplotmpl hists5.root --numbering

# 2D Tests
COMMON='hists8.root --numbering --path=hcorr2D'
mvmerge draw2Dcol $COMMON --draw2D=col
mvmerge draw2Dcolz $COMMON --draw2D=colz
mvmerge draw2Dcon $COMMON --draw2D=contour
mvmerge draw2Dbox $COMMON --draw2D=box

# Output formats
COMMON='hists9.root --numbering'
rootplotmpl $COMMON --ext=png
rootplotmpl $COMMON --ext=pdf
rootplotmpl $COMMON --ext=ps

# Histogram Types
COMMON='hists2.root --numbering'
mvmergempl mplhist $COMMON --hist
mvmergempl mplstack $COMMON --stack
mvmergempl mplbar $COMMON --bar
mvmergempl mplbarh $COMMON --barh
mvmergempl mplbarcluster $COMMON --barcluster
mvmergempl mplerror $COMMON --errorbar

# Other
COMMON='hists2.root hists3.root hists4.root hists5.root --numbering'
mvmergempl mplratio $COMMON --ratio=3
mvmergempl mpllog $COMMON --logx --logy
mvmergempl mploverflow $COMMON --overflow --underflow

# Configs
for f in `/bin/ls configs`; do
    rootplot configs/$f
done
