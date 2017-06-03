
python /group/log/torque/torque-user-info.py | awk '{if(($4 != "0") && ($2 >= "15257239"))
                                                          print "qsub SubmitScripts/2_6_2017/Dilep/"$13";"}'
