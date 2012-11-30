#! /bin/env perl

use strict;
use warnings;

my $now = 0;
while ($now <= 256)
{
  print "\033[48;5;${now}m256-color tty test, this is color`$now`", "\033[0m\t";
  print "\033[38;5;${now}m256-color tty test, this is color`$now`", "\033[0m\n";
  $now++;
}
