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

# 全局数组,保存URL
my @urls :shared;

# 获取一个URL的内容
# 使用CURL
#
# ARGS:
#  url: 需要下载的URL
# 返回内容
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
    printf "\033[31mERR:\033[0m%s($retcode)\n",
      $curl_handle->strerror($retcode);

    die 'Unhandled ERR...';
  }
}

sub _thread_parser {
  threads->detach();

  lock @urls;

  URL:
  while (1) {
    push @urls, 'file:/home/GaoJinpei/Downloads/Packages/cedet-1.1.tar.gz';
    last URL if @urls >= 20;
  }
}

{
  my %handler_of;
  my %fh_of;
  my %data_of;
  my $active_handlers = 0;

  # 分配一个未使用的ID
  # Return: ID
  sub alloc_id {
    ALLOC_ID:
    while (1) {
      my $id = int(rand(1000));
      return $id if not exists $handler_of{$id};
    }
  }

  # 创建并设置一个句柄
  # ARGS:
  #  url: 下载链接
  #  curlm: 管理器实例
  # 没有有价值的返回值
  sub create_handler {
    my ($url, $curlm) = @_;

    # 创建句柄
    my $handler = WWW::Curl::Easy->new;
    # 分配唯一的任务ID
    my $handler_id = alloc_id();
    # 设置ID
    $handler->setopt(CURLOPT_PRIVATE, $handler_id);

    # 设置句柄
    $handler->setopt(CURLOPT_HEADER, 1);
    $handler->setopt(CURLOPT_URL, $url);

    # 设定输出
    my $res_data;
    open my $output_fd, '>', \$res_data;
    $data_of{$handler_id} = \$res_data;
    $handler->setopt(CURLOPT_WRITEDATA, $output_fd);
    $fh_of{$handler_id} = $output_fd;

    # 保存句柄
    $handler_of{$handler_id} = $handler;

    # 添加句柄
    $curlm->add_handle($handler);
    $active_handlers++;
  }

  # 下载器线程函数
  #
  # 这个函数没有参数
  # 没有有价值的返回值
  sub _thread_downloader {
    # 分配一个下载管理器
    my $curlm = WWW::Curl::Multi->new;

    # 配置句柄
    do {
      lock @urls;

      HANDLER:
      while (@urls) {
        my $url = pop @urls;

        create_handler($url, $curlm);

        last HANDLER if $active_handlers >= 10;
        last HANDLER if @urls == 0;
      }
    };

    # 开始下载循环
    while ($active_handlers) {
      my $active_transfers = $curlm->perform;
      if ($active_handlers != $active_transfers) {
        while (my ($id, $return_val) = $curlm->info_read) {
          if ($id) {
            # 取出句柄
            $active_handlers--;
            my $handler = $handler_of{$id};
            die 'ERR HANDLER' if not defined $handler;

            if ($return_val == 0) {
              # TODO:
              # 处理内容
            }
            else {
              printf "\033[31mERROR:\033[0m%s($return_val)\n",
                $handler->strerror($return_val);
            }

            close $fh_of{$id} if defined $fh_of{$id};
            delete $fh_of{$id};
            delete $handler_of{$id};
          }
        }
      }
      if ($active_transfers == 0) {
        # 所有的下载已结束
        return;
      }
    }
  }

}
my $parser_thr = threads->create(\&_thread_parser);
threads->create(\&_thread_downloader)->join();
