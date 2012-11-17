#! /bin/env perl
#
#

use strict;
use warnings;

use threads ('yield',
             'stack_size' => 64*4096,
             'exit' => 'threads_only');
use threads::shared;

use WWW::Curl::Easy;
use WWW::Curl::Multi;

use Smart::Comments;

use Thread::Semaphore;

my $s = Thread::Semaphore->new;
my @urls :shared;
my $num_urls :shared = 0;

sub get_content {
  my ($url) = @_;

  my $curl_handle = WWW::Curl::Easy->new();
  $curl_handle->setopt(CURLOPT_URL, $url);
  $curl_handle->setopt(CURLOPT_HEADER, 1);
  
  my $content;
  open my $fh, '>', \$content or die "can't open file handle";
  $curl_handle->setopt(CURLOPT_WRITEDATA, \$content);

  my $retcode = $curl_handle->perform;

  if ($retcode == 0) {
    return $content;
  }
  else {
    print "\033[31mERR:\033[0m" . $curl_handle->strerror($retcode) . "($retcode)\n";
    die 'Unhandled ERR...';
  }
}

sub _thread_parser {
  threads->detach();

  $s->down;
  
 LOOP:
  while (1) {
    push @urls, '2000';
    $num_urls ++;
    last LOOP if $num_urls >= 20;
  }
  sleep 3;

  $s->up;
}

sub _thread_downloader {
  my %handler_of;
  my %fh_of;
  my %data_of;
  my $active_handlers = 0;

  my $curlm = WWW::Curl::Multi->new;

  HANDLER:
  while ($num_urls) {
    $s->down;
    my $url = pop @urls;
    $num_urls--;
    $s->up;
    warn 'Errr ... "NULL" string in $url' if not defined $url;

    my $handler = WWW::Curl::Easy->new;
    my ($handler_id) = 
      $url =~ m/([^\/]+)$/;

    $handler->setopt(CURLOPT_PRIVATE, $handler_id);
    $handler->setopt(CURLOPT_HEADER, 1);
    $handler->setopt(CURLOPT_URL, $url);
    # setup OUTPUT
    my $res_data;
    open my $output_fd, '>', \$res_data;
    $data_of{$handler_id} = \$res_data;
    $handler->setopt(CURLOPT_WRITEDATA, $output_fd);
    $fh_of{$handler_id} = $output_fd;

    $handler_of{$handler_id} = $handler;

    $curlm->add_handle($handler);
    $active_handlers++;

    last HANDLER if $active_handlers >= 10;
    last HANDLER if $num_urls == 0;
  }

  ### start dowloading
  while ($active_handlers) {
    my $active_transfers = $curlm->perform;
    if ($active_handlers != $active_transfers) {
      while (my ($id, $return_val) = $curlm->info_read) {
        if ($id) {
          $active_handlers--;
          my $handler = $handler_of{$id};

          die 'ERR HANDLER' if not defined $handler;
          ### $handler
          ### $id
          
          if ($return_val == 0) {
            ### $handler
          }
          else {
            print "\033[31mERR:\033[0m" . $handler->strerror($return_val) . "($return_val)\n";
          }
          

          close $fh_of{$id} if defined $fh_of{$id};
          delete $fh_of{$id};
          delete $handler_of{$id};
        }
      }
    }
    if ($active_transfers == 0) {
      return;
    }
  }
}

my $parser_thr = threads->create(\&_thread_parser);
threads->create(\&_thread_downloader)->join();
