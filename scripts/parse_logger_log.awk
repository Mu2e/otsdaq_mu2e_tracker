#------------------------------------------------------------------------------
# write out parsed data to be used by TTree::ReadFile()
#------------------------------------------------------------------------------
BEGIN {
  printf("mkt/I:date/C:time/C:nev/I:rate/F:dt/F\n");
}

/logger0_SharedMemoryEventManager/ { 
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

  
  date = sprintf("%s-%s-%s",arr[3],month,arr[1]);

  split($5,a2,":")
  # printf("tm: %s  arr[2]: %s  month: %2s day: %i \n",tm, arr[2],month,arr[1])

  x = sprintf("%s %s %s %s %s %s [CST]",arr[3], month,arr[1],a2[1],a2[2],a2[3]);
  mkt = mktime(x)
#  printf("x:%12s mkt:%12i\n",x,mkt)

  time=$5
}

/Event statistics:/{
  nev  = $3;
  rate = $13;
  dt   = $18

  printf "%12i %s %s %6i  %10.5f %10.4f \n", 
    mkt,date,time, nev,rate,dt;
}


