#Auto generate code from ISO/IEC spec
#@author Jason Gao
#@date 11/22/2008 created

$SPEC_FILES = "*.spec";

use constant BLOCK_IF => 1;
use constant BLOCK_ELSE => 2;
use constant BLOCK_FOR => 3;

foreach (glob $SPEC_FILES)
{
	translateSpecFile($_);
}

sub translateSpecFile
{
	my $specfile = shift;
	print "\n\nTranslate file $specfile...\n";
	my $headfile = "$specfile.h";
	my $cppfile = "$specfile.cpp";
	open (my $fspec, "<$specfile");
	open (my $fhead, ">$headfile");
	open (my $fcpp, ">$cppfile");
	
	my $headfileflag = "_" . uc($specfile) . "_H_";
	$headfileflag =~ tr/-\. /_/;
	print $fhead "#ifndef $headfileflag\n";
	print $fhead "#define $headfileflag\n\n";
	print $fhead "#include \"tsbscls.h\"\n\n";
	print $fhead "namespace TS {\n\n";

	print $fcpp "#include \"$headfile\"\n\n";
	print $fcpp "using namespace TS;\n\n";
	
	my @blocks = (); #block of {}
	my $depth = 0;
	my $loopflag = 0;
	my $loopcount = 0;
	my ($classname, $codecons, $codedesc, $codeload, $codedump);
	while (<$fspec>)
	{
		if ($depth == 0) #find class
		{
			next unless /\s*(.+?)\s*\(\s*\)\s*\{/;
			$classname = $1;
			$classname =~ tr/- /_/; #replace non-word char
			$loopcount = 0;
			
			#header file
			print $fhead "class $classname\n";
			print $fhead ": public descriptor\n" if $classname =~ /(_d|D)escriptor$/;
			print $fhead ": public Item\n" if $classname =~ /_item$/;
			print $fhead "{\n";
			print $fhead "public:\n";
			print $fhead "\t$classname();\n";
			print $fhead "\tvirtual ~$classname();\n";
			print $fhead "\tvirtual bool Load(BitStream& is, void* pEnd);\n";
			print $fhead "#ifdef ENABLE_LOG\n";
			print $fhead "\tvirtual void Dump(Logger& os);\n";
			print $fhead "#endif //ENABLE_LOG\n";
			if ($classname =~ /_item$/)
			{
				print $fhead "\tinline $classname* Next() { return ($classname*)(more); }\n";
				print $fhead "\tstatic Item* Create(BitStream& is, void* pEnd) { return (uint8*)pEnd - is.Position() < 1 ? NULL : new $classname(); }\n";
				print $fhead "\tstatic $classname* LoadAll(BitStream& is, void* pEnd) { return ($classname*) Item::LoadAll(is, pEnd, Create); }\n";
			}
			print $fhead "\npublic:\n";

			#cpp file
			print $fcpp "\n// $classname\n";
			print $fcpp "//=============================================================================\n\n";
			$codecons = "$classname\::$classname()\n{\n";
			$codedesc = "$classname\::~$classname()\n{\n";
			$codeload = "bool $classname\::Load(BitStream& is, void* pEnd)\n{\n";
			$codedump = "#ifdef ENABLE_LOG\n";
			$codedump .= "void $classname\::Dump(Logger& os)\n{\n";
			$codedump .= "\tos.StartBlock(\"$classname\");\n";
			$codedump .= "\tos.WriteBits(\"tag\", tag);\n\tos.WriteBits(\"length\", length);\n" if $classname =~ /(_d|D)escriptor$/;
			
			$depth++;
		}
		else #($depth > 0)
		{
			my $rawfield = 0;
			my ($fieldname, $fieldwidth, $fieldtype, $fieldsize);
			if (/\s*(.+)\s+(\d+)\s+(uimsbf|bslbf)\s*(\S*)/) #find raw field
			{
				($fieldname, $fieldwidth, $fieldtype, $fieldsize) = ($1, $2, $3, $4);
				$rawfield = 1;
			}
			elsif (/bit\((\d+)\)\s+(.+);/) #another form of raw field
			{
				($filetype, $fieldwidth, $fieldname, $fieldsize) = ("bit", $1, $2, 1);
				$rawfield = 1;
			}
			if ($rawfield)
			{
				$fieldname =~ tr/- /_/; #replace non-word char
				next if ($fieldname eq "descriptor_tag");  #ignore this field since it is in base class
				next if ($fieldname eq "descriptor_length"); #ignore this field since it is in base class
				if ($fieldname =~ "^reserved" || $fieldname =~ "^['\"].+['\"]")
				{
					$codeload .= "\t" x $depth . "is.SkipBits($fieldwidth); //$fieldname\n";
				}
				else
				{
					if ($loopflag)
					{
						print $fhead "\tbit$fieldwidth* $fieldname;\n";
						$codecons .= "\t$fieldname = NULL;\n";
						$codedesc .= "\tif ($fieldname)\n";
						$codedesc .= "\t{\n";
						$codedesc .= "\t\tdelete $fieldname;\n";
						$codedesc .= "\t\t$fieldname = NULL;\n";
						$codedesc .= "\t}\n";
						if ($fieldsize =~ /\S+/)
						{
							$codeload .= "\t" x $depth . "$fieldname = new bit$fieldwidth\[$fieldsize + 1];\n";
							$codeload .= "\t" x $depth . "is.ReadData($fieldsize, $fieldname);\n";
							$codeload .= "\t" x $depth . "$fieldname\[$fieldsize] = 0;\n";
							$codedump .= "\t" x $depth . "os.WriteData(\"$fieldname\", $fieldname, $fieldsize);\n";
						}
						else
						{
							$codeload .= "\t" x $depth . "is.ReadBits($fieldwidth, $fieldname\[i]);\n";
							$codedump .= "\t" x $depth . "os.WriteBits(\"$fieldname\", $fieldname\[i]);\n";
						}
					}
					else
					{
						print $fhead "\tbit$fieldwidth $fieldname;\n";
						$codeload .= "\t" x $depth . "is.ReadBits($fieldwidth, $fieldname);\n";
						$codedump .= "\t" x $depth . "os.WriteBits(\"$fieldname\", $fieldname);\n";
					}
				}
			}
			elsif (/\s*(.+?)\s*\(\s*\)\s*(\S*)/) #find object field
			{
				my ($fieldtype, $fieldsize) = ($1, $2);
				$fieldtype =~ tr/- /_/; #replace non-word char
				my $fieldname = $fieldtype . "s";
				print $fhead "\t$fieldtype* $fieldname;\n";
				$codecons .= "\t$fieldname = NULL;\n";
				$codedesc .= "\tif ($fieldname)\n";
				$codedesc .= "\t{\n";
				$codedesc .= "\t\tdelete $fieldname;\n";
				$codedesc .= "\t\t$fieldname = NULL;\n";
				$codedesc .= "\t}\n";
				if ($fieldname =~ /[descriptor|_item]$/)
				{
					if ($fieldsize =~ /\S+/)
					{
						$fieldsize =~ tr/- /_/; #replace non-word char
						$codeload .= "\t" x $depth . "if (is.Position() + $fieldsize > pEnd) return false;\n";
						$codeload .= "\t" x $depth . "if ($fieldname) delete $fieldname; //Debug\n";
						$codeload .= "\t" x $depth . "$fieldname = $fieldtype\::LoadAll(is, is.Position() + $fieldsize);\n";
					}
					else
					{
						$codeload .= "\t" x $depth . "if ($fieldname) delete $fieldname; //Debug\n";
						$codeload .= "\t" x $depth . "$fieldname = $fieldtype\::LoadAll(is, pEnd);\n";
					}
					$codedump .= "\t" x $depth . "if ($fieldname) $fieldname->DumpAll(os); \n";
				}
				else
				{ 
					$codeload .= "\t" x $depth . "$fieldname->Load(is, pEnd); //TODO: new $fieldname & loop\n";
					$codedump .= "\t" x $depth . "$fieldname->Dump(os); //TODO: loop\n";
				}
				
			}
			elsif (/}/) #end of class, for or if
			{
				my $block = pop(@blocks);
				$depth-- unless $block == BLOCK_FOR;
				if ($depth == 0) #end of class
				{
					print $fhead "};\n\n";
					$codecons .= "}\n\n";
					$codedesc .= "}\n\n";
					$codeload .= "\treturn true;\n}\n\n";
					$codedump .= "\tos.EndBlock(\"$classname\");\n";
					$codedump .= "}\n";
					$codedump .= "#endif //ENABLE_LOG\n\n";
					print $fcpp $codecons;
					print $fcpp $codedesc;
					print $fcpp $codeload;
					print $fcpp $codedump;
				}
				else
				{
					if ($block == BLOCK_FOR)
					{
						#$codeload .= "\t" x $depth . "\ti++;\n";
						#$codeload .= "\t" x $depth . "}\n";
						#$codeload .= "\t" x $depth . "count$loopcount = i;\n";
						#$codedump .= "\t" x $depth . "}\n";
						$loopflag--;
					}
					else
					{
						$codeload .= "\t" x $depth . "}\n";
						$codedump .= "\t" x $depth . "}\n";
					}
				}
			}
			elsif (/^\s*if\s*\((\S+)\s*([=><]+)\s*['\"](.+)['\"]\)\s*{/i)
			{
				my ($fieldname, $op, $cond) = ($1, $2, $3);
				$fieldname =~ tr/- /_/; #replace non-word char
				$codeload .= "\t" x $depth . "if ($fieldname $op $cond)\n";
				$codeload .= "\t" x $depth . "{\n";
				$codedump .= "\t" x $depth . "if ($fieldname $op $cond)\n";
				$codedump .= "\t" x $depth . "{\n";
				$depth++;
				push(@blocks, BLOCK_IF);
			}
			elsif (/^\s*else/i)
			{
				$codeload .= "\t" x $depth . "else\n";
				$codeload .= "\t" x $depth . "{\n";
				$codedump .= "\t" x $depth . "else\n";
				$codedump .= "\t" x $depth . "{\n";
				$depth++;
				push(@blocks, BLOCK_ELSE);
			}
			elsif (/^\s*for/i)
			{
				$loopcount++;
				#print $fhead "\tint count$loopcount;\n";
				#$codeload .= "\t" x $depth . "int i = 0;\n" if $loopcount == 1;
				#$codeload .= "\t" x $depth . "while (is.Position() < pEnd)\n\t{\n";
				#$codedump .= "\t" x $depth . "int i = 0;\n" if $loopcount == 1;
				#$codedump .= "\t" x $depth . "for (i = 0; i < count$loopcount; i++)\n\t{\n";
				#$depth++;
				$loopflag++;
				push(@blocks, BLOCK_FOR);
			}
		}
	}
	

	print $fhead "} //namespace TS\n";
	print $fhead "#endif //$headfileflag\n";
	close ($fhead);
	close ($fcpp);
	close ($fspec);
}
