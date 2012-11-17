#! /bin/env perl
# 文本文件预览脚本
#

use strict;
use warnings;

#use Smart::Comments;
# 命令行处理

use Getopt::Long;

my $lines = 20;
my $in_ranger = 0;

GetOptions(
  "lines|n=i" => \$lines,
  "ranger" => \$in_ranger,
  );

my $filename = shift @ARGV;
die 'must have filename' if not defined $filename;
die "Can't use options '".join(',',@ARGV)."'" if not $#ARGV;

die "file '$filename' not exist!" if ! -e $filename;

# 编码识别
# FIXME: 提高性能
chomp (my $encoding = `uchardet "$filename"`);
$encoding = 'ascii' if $encoding =~ /unknown$/i;
$encoding = 'gbk' if lc($encoding) eq 'gb18030';

open my $fh, '<', $filename;
binmode $fh => "raw:encoding($encoding)";
binmode STDOUT => "raw:encoding(utf8)";

print "\033[1;34mfile encoding: ".uc($encoding)."\033[0m\n" if $in_ranger;

for(my $i = 0; $i < $lines; $i++)
  {
    my $line = <$fh>;
    last if not $line;
    print $line;
  }


__END__
