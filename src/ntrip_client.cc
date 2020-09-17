// Copyright 2019 Yuming Meng. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ntrip_client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <string>
#include <thread>  // NOLINT.
#include <list>
#include <vector>

#include "ntrip_util.h"


namespace libntrip {

namespace {

// GPGGA format example.
constexpr char gpgga_buffer[] =
    "$GPGGA,083552.00,3000.0000000,N,11900.0000000,E,"
    "1,08,1.0,0.000,M,100.000,M,,*57\r\n";

}  // namespace

//
// Public method.
//

bool NtripClient::Run(void) {
  service_is_running_.store(false);
  int ret = -1;
  char recv_buf[1024] = {0};
  char request_data[1024] = {0};
  char userinfo_raw[48] = {0};
  char userinfo[64] = {0};
  // Generate base64 encoding of username and password.
  snprintf(userinfo_raw, sizeof(userinfo_raw) , "%s:%s",
           user_.c_str(), passwd_.c_str());
  Base64Encode(userinfo_raw, userinfo);
  // Generate request data format of ntrip.
  snprintf(request_data, sizeof(request_data),
           "GET /%s HTTP/1.1\r\n"
           "User-Agent: %s\r\n"
           "Accept: */*\r\n"
           "Connection: close\r\n"
           "Authorization: Basic %s\r\n"
           "\r\n",
           mountpoint_.c_str(), kClientAgent, userinfo);

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port_);
  server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    printf("Create socket fail\n");
    return false;
  }

  // Connect to caster.
  if (connect(socket_fd, reinterpret_cast<struct sockaddr *>(&server_addr),
              sizeof(struct sockaddr_in)) < 0) {
    printf("Connect caster failed!!!\n");
    return false;
  }

  int flags = fcntl(socket_fd, F_GETFL);
  fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);

  // Send request data.
  if (send(socket_fd, request_data, strlen(request_data), 0) < 0) {
    printf("Send request failed!!!\n");
    close(socket_fd);
    return false;
  }

  // Waitting for request to connect caster success.
  int timeout = 3;
  while (timeout--) {
    ret = recv(socket_fd, recv_buf, sizeof(recv_buf), 0);
    if ((ret > 0) && !strncmp(recv_buf, "ICY 200 OK\r\n", 12)) {
      if (gga_buffer_.empty()) {
        GetGGAFrameData(latitude_, longitude_, 10.0, &gga_buffer_);
      }
      ret = send(socket_fd, gga_buffer_.c_str(), gga_buffer_.size(), 0);
      if (ret < 0) {
        printf("Send gpgga data fail\n");
        close(socket_fd);
        return false;
      }
      // printf("Send gpgga data ok\n");
      break;
    } else if (ret == 0) {
      printf("Remote socket close!!!\n");
      close(socket_fd);
      return false;
    }
    sleep(1);
  }

  if (timeout <= 0) {
    return false;
  }
  // TCP socket keepalive.
  int keepalive = 1;  // Enable keepalive attributes.
  int keepidle = 30;  // Time out for starting detection.
  int keepinterval = 5;  // Time interval for sending packets during detection.
  int keepcount = 3;  // Max times for sending packets during detection.
  setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive,
             sizeof(keepalive));
  setsockopt(socket_fd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
  setsockopt(socket_fd, SOL_TCP, TCP_KEEPINTVL, &keepinterval,
             sizeof(keepinterval));
  setsockopt(socket_fd, SOL_TCP, TCP_KEEPCNT, &keepcount, sizeof(keepcount));
  socket_fd_ = socket_fd;
  thread_ = std::thread(&NtripClient::TheradHandler, this);
  thread_.detach();
  printf("NtripClient service starting ...\n");
  gga_is_update_.store(false);
  return true;
}

void NtripClient::Stop(void) {
  service_is_running_.store(false);
  if (socket_fd_ != -1) {
    close(socket_fd_);
    socket_fd_ = -1;
  }
}

//
// Private method.
//

void NtripClient::TheradHandler(void) {
  service_is_running_.store(true);
  int ret;
  char recv_buffer[1024] = {};
  auto start_tp = std::chrono::steady_clock::now();
  int intv_ms = report_interval_ * 1000;
 
  while (service_is_running_) {

    ret = recv(socket_fd_, recv_buffer, sizeof(recv_buffer), 0);
    if(ret > 0){
      printf("ret:  %d\n",ret);
           if (gga_buffer_.empty()) {
      GetGGAFrameData(latitude_, longitude_, 10.0, &gga_buffer_);
    }
    time_t myt=time(NULL);
    if(myt%16==0){
    ret = send(socket_fd_, gga_buffer_.c_str(), gga_buffer_.size(), 0);
    printf("sendRet:  %s \n\n ret: %d",gga_buffer_.c_str(),ret);
  }
    char mytime[300] = {0};
    sprintf(mytime,"%li",myt);
    FILE *nema ;
    strcat(mytime,".nema");
    nema = fopen(mytime,"wb");
    printf("fileName:  %s\n",mytime);
    fwrite(recv_buffer,ret,1,nema);
    fclose(nema);
  }
    if (ret == 0) {
      printf("Remote socket close!!!\n");
      exit(0);
      break;
    } else if (ret < 0) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        continue;
      } else {
        printf("Remote socket error!!!\n");
        exit(0);
        break;
      }
    } else {
      callback_(recv_buffer, ret);
    }
    if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now()-start_tp).count() >= intv_ms) {
      start_tp = std::chrono::steady_clock::now();
      if (gga_is_update_ == false) {
        GetGGAFrameData(latitude_, longitude_, 10.0, &gga_buffer_);
      }
      send(socket_fd_, gga_buffer_.c_str(), gga_buffer_.size(), 0);
      gga_is_update_.store(false);
    }
  }
  close(socket_fd_);
  socket_fd_ = -1;
  service_is_running_ = false;
}

}  // namespace libntrip
