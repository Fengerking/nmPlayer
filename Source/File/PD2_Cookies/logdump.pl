open logfile , "<" , "log";

open file_critical, ">" , "critical.log";
open file_important, ">" , "important.log";
open file_normal, ">" , "normal.log";
open file_detail, ">" , "detail.log";
open file_allinfo, ">" , "allinfo.log";

open file_pdlog, ">" , "pdlog.log";
open file_nonepdlog, ">" , "nopdlog.log";

while( <logfile> )
{
  	if( /PDLog.*\[\*    \]/ )
  	{
    	print file_critical "$_";
   		print file_important "$_";
   		print file_normal "$_";
   		print file_detail "$_";
   		print file_allinfo "$_";
   	}
   	elsif( /PDLog.*\[\*{1,2} {3,4}\]/ )
   	{
   		print file_important "$_";
   		print file_normal "$_";
   		print file_detail "$_";
   		print file_allinfo "$_";
   	}
   	elsif( /PDLog.*\[\*{1,3} {2,4}\]/ )
   	{
   		print file_normal "$_";
   		print file_detail "$_";
   		print file_allinfo "$_";
   	}
   	elsif( /PDLog.*\[\*{1,4} {1,4}\]/ )
   	{
   		print file_detail "$_";
   		print file_allinfo "$_";
   	}
   	elsif( /PDLog.*\[\*{1,5} {0,4}\]/ )
   	{
   		print file_allinfo "$_";
   	}
   	
   	if( /PDLog/ )
   	{
   		print file_pdlog "$_";
   	}
   	else
   	{
   		print file_nonepdlog "$_";
   	}
}

close logfile;
close file_allinfo;
close file_detail;
close file_normal;
close file_important;
close file_critical;
close file_nonepdlog;
close file_pdlog;