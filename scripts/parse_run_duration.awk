#------------------------------------------------------------------------------
# parse data logger log files
# write out parsed data to be used by TTree::ReadFile()
# example: 
# > dir=/scratch/mu2e/mu2etrk_mu2e_pasha_018/Logs/datalogger
# > for f in `ls $dir/run*.log | grep 10507` ; do awk -f parse_run_duration.awk $f ; done
#
# the ntuple format - right below; mkt - outptu of maketime, in seconds
#------------------------------------------------------------------------------
BEGIN {
#  printf("run/I:start_date/C:start_time/C:start_mkt/I:stop_date/C:stop_time/C:stop_mkt/I:dt/I\n");
  flag      = 0;
  flag_stop = 0;
}

/Start transition started/ { flag      = 1 }
/Stop transition started/  { flag_stop = 1 }

/%MSG-i logger0_DataReceiverCore/ { 

  if (flag == 1) {
    split($4, arr , "-")

    month = "xx"
    if (arr[2] == "Jan") month = "01"
    if (arr[2] == "Feb") month = "02"
    if (arr[2] == "Mar") month = "03"
    if (arr[2] == "Apr") month = "04"
    if (arr[2] == "May") month = "05"
    if (arr[2] == "Jun") month = "06"
    if (arr[2] == "Jul") month = "07"
    if (arr[2] == "Aug") month = "08"
    if (arr[2] == "Sep") month = "09"
    if (arr[2] == "Oct") month = "10"
    if (arr[2] == "Nov") month = "11"
    if (arr[2] == "Dec") month = "12"

  
    start_date = sprintf("%s-%s-%s",arr[3],month,arr[1]);
  
    split($5,a2,":")
    # printf("tm: %s  arr[2]: %s  month: %2s day: %i \n",tm, arr[2],month,arr[1])

    x = sprintf("%s %s %s %s %s %s [CST]",arr[3], month,arr[1],a2[1],a2[2],a2[3]);
    start_mkt = mktime(x)
    #  printf("x:%12s mkt:%12i\n",x,mkt)

    start_time=$5

    flag = 0;
  }


  if (flag_stop == 1) {
    split($4, arr , "-")

    month = "xx"
    if (arr[2] == "Jan") month = "01"
    if (arr[2] == "Feb") month = "02"
    if (arr[2] == "Mar") month = "03"
    if (arr[2] == "Apr") month = "04"
    if (arr[2] == "May") month = "05"
    if (arr[2] == "Jun") month = "06"
    if (arr[2] == "Jul") month = "07"
    if (arr[2] == "Aug") month = "08"
    if (arr[2] == "Sep") month = "09"
    if (arr[2] == "Oct") month = "10"
    if (arr[2] == "Nov") month = "11"
    if (arr[2] == "Dec") month = "12"

  
    stop_date = sprintf("%s-%s-%s",arr[3],month,arr[1]);
  
    split($5,a2,":")
    # printf("tm: %s  arr[2]: %s  month: %2s day: %i \n",tm, arr[2],month,arr[1])

    x = sprintf("%s %s %s %s %s %s [CST]",arr[3], month,arr[1],a2[1],a2[2],a2[3]);
    stop_mkt = mktime(x)
    #  printf("x:%12s mkt:%12i\n",x,mkt)

    stop_time=$5

    flag_stop = 0;

  }
}

/Stopping run/{
  run_number = $3;

  dt = stop_mkt-start_mkt
  printf "%6i %s %s %10i %s %s %10i %10i\n", 
      run_number,start_date,start_time,start_mkt,stop_date,stop_time,stop_mkt,dt;
}


