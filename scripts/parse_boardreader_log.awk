#------------------------------------------------------------------------------
# write out parsed data to be used by TTree::ReadFile()
#------------------------------------------------------------------------------
BEGIN {
  printf("mkt/I:date/C:time/C:nfr/I:calls_per_sec/F:frag_rate/F:rmon_win/F:nmin/I:nmax/I:")
  printf("nfw/I:nfw_per_sec/F:rate/F:wmon_win/F:min_size/F:max_size/F:");
  printf("input_wtime/F:buffer_wtime/F:request_wtime/F:output_wtime/F");
  printf("\n");
}

/pre-events BoardReaderCore.cc/ { 
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
  time=$5

  split(time,a2,":")
  # printf("tm: %s  arr[2]: %s  month: %2s day: %i \n",tm, arr[2],month,arr[1])

  x = sprintf("%s %s %s %s %s %s [CST]",arr[3],month,arr[1],a2[1],a2[2],a2[3]);
  mkt = mktime(x)
#  printf("x:%12s mkt:%12i\n",x,mkt)
}

/ Fragments read:/{
  fragments_read = $3
  calls_per_second = $7
  fragment_rate = $13
  rmon_window = $18

  split($24, arr , ":")
  nmin = arr[1]
  nmax = arr[3]

  elapsed_time = $32
}

/Fragment output statistics:/ {
  nfrag_written = $4
  nfrag_written_per_second = $8 
  data_rate = $14
  wmon_window = $19
  split($25, arr , ":")
  min_event_size = arr[1]
  max_event_size = arr[3]
}


/Input wait time/ {

  input_wait_time = $5;
  buffer_wait_time = $11;
  request_wait_time = $17;
  output_wait_time = $23;
  
  
  printf("%10i %s %s",mkt, date, time);
  printf(" %6i %8.3f %8.2f %6.2f %3i %3i", 
         fragments_read, calls_per_second,fragment_rate, rmon_window, nmin, nmax);
  printf(" %6i %8.2f %8.3f %8.3f %10.3e %10.3e",
         nfrag_written, nfrag_written_per_second, data_rate, wmon_window, 
         min_event_size, max_event_size);
  printf(" %10.3e %10.3e %10.3e %10.3e",
         input_wait_time, buffer_wait_time, request_wait_time, output_wait_time);
  printf("\n");
}


