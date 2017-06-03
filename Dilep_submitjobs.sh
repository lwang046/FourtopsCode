#! /bin/bash

ls ./SubmitScripts/2_6_2017/Dilep/submit_*.sh | awk '{print "qsub "$1"\n"}' | sh
