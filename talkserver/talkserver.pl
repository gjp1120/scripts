#! /bin/env perl
# AIO Talk Server
#
# author: gjp1120@gmail.com
# distribute under GPLv2

use strict;
use warnings;

use IO::Socket;
use Text::Iconv;
use IO::Handle;
use IO::Poll qw/POLLIN/;
use Carp;

use Getopt::Long;

#use Smart::Comments;

my $DEFAULT_ENCODING = 'cp936';
my $PORT = 8000;

GetOptions('encoding|e=s' => \$DEFAULT_ENCODING,
           'port|p=i' => \$PORT);

# Declare of ATTRS
our %encoding_of;
our %decode_h;
our %encode_h;
our %name_of;
our %is_colored;

our @USERS;

# Main Program
# ==============

### Create Socket
my $sock = IO::Socket::INET->new( Listen    => 10,
                                  LocalPort => $PORT )
           or croak "can't create listening soket: $!";

### Create Poll Handle
my $poll = IO::Poll->new();
$poll->mask(\*STDIN => POLLIN);
$poll->mask($sock => POLLIN);

### Set Handle Opts
*{STDOUT}->autoflush(1);
binmode STDOUT;

### Setup Log Handle
open my $log_fd, '>>', "talk.log";

### Start!
print <<"END_SECION";
=================================
 Talk Server Started (Telnet)
     port:     $PORT
     encoding: $DEFAULT_ENCODING
=================================
END_SECION

# Show Prompt
print_prompt(\*STDOUT);

### Loop Start
READY:
while ($poll->poll) {
  my @ready = $poll->handles(POLLIN);

  HANDLE:
  foreach my $handle (@ready) {
    if ($handle == $sock) {
      my $session = $sock->accept or carp "error occured when try to accept connection";

      print $session <<'END_MSG';
Welcome to telnet talk server
press <Enter> to send message
==============================
END_MSG

      print_prompt($session);
      $poll->mask($session => POLLIN);

      set_client_encoding($session, $DEFAULT_ENCODING);

      next HANDLE;
    }

    ### $handle
    my $line = <$handle>;

    # Server    
    if ($handle == \*STDIN) {
      last READY if not defined $line;

      chomp $line;

      broadcast_msg('server: '.$line."\r\n", \*STDOUT);
      print_prompt(\*STDOUT);

      next HANDLE;
    }

    # Client
    if (not defined $line) {
      $poll->remove($handle);
      
      broadcast_msg('* client disconnected', $sock);

      close $handle;
    }
    else {
      $line = $decode_h{$handle}->convert($line) if defined $decode_h{$handle};

      broadcast_msg('client: '.$line, $handle);
      print_prompt($handle);
    }
  } 
}

### Clean Socket
close $sock;
close $log_fd;


# END Main()
# ===========

sub broadcast_msg {
  my ($raw_msg,  $sender) = @_;

  if ($raw_msg !~ /\r\n$/) {
    $raw_msg =~ s/\n?$/\r\n/;
  }

  DEST:
  foreach my $dest ($poll->handles()) { ### Broadcasting... END
    next DEST if $dest == $sock;

    $dest = \*STDOUT if $dest == \*STDIN;
    next DEST if $dest == $sender;
    
    if ($encode_h{$dest}) {
      my $encoded_msg = $encode_h{$dest}->convert($raw_msg);
      clear_prompt($dest);
      print $dest $encoded_msg;
      print_prompt($dest);
    }
    else {
      clear_prompt($dest);
      print $dest $raw_msg;
      print_prompt($dest);
    }
  }

  ### Logging
  log_msg($raw_msg)
}

# Prompt Function
# -----------------

sub clear_prompt {
  my $conn = shift;
  print $conn "\r" . ""x20 ."\r";
}

sub print_prompt {
  my $conn = shift;

  if (defined $name_of{$conn}) {
    print $conn "$name_of{$conn}>";
  }
  else {
    print $conn 'talk> ';
  }
}

# ChatLog
# ----------
sub log_msg {
  my $msg = shift;
  print $log_fd localtime().' '.$msg;
}

sub set_client_encoding {
  my ($session, $encoding) = @_;

  $encoding_of{$session} = $encoding;

  $encode_h{$session} = Text::Iconv->new($encoding, $DEFAULT_ENCODING);
  $decode_h{$session} = Text::Iconv->new($DEFAULT_ENCODING, $encoding);
}
