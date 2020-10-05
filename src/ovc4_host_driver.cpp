
#include <iostream>
#include <cstring>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ethernet_packetdef.h>

#include <subscriber.hpp>

#define BUFLEN 16384
#define BASE_PORT 12345 + 2

int main(int argc, char **argv)
{
  ReceiveState state = ReceiveState::WAIT_HEADER;
  struct sockaddr_in si_self = {0}, si_other = {0};
  unsigned int si_size = sizeof(si_other);
  ether_tx_packet_t recv_pkt;
  int sock;
  unsigned int sock_len;
  std::cout << "Hello world" << std::endl;

  sock = socket(AF_INET, SOCK_STREAM, 0);

  si_self.sin_family = AF_INET;
  si_self.sin_port = htons(BASE_PORT);
  si_self.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(sock, (struct sockaddr *) &si_self, sizeof(si_self));
  listen(sock,1);
  int newsock = accept(sock, (struct sockaddr *)&si_other, &si_size);
  int exp_status = 0;

  int cur_off = 0;
  int frame_size = 0;

  while (1)
  {
    char buff[8192];
    if (state == ReceiveState::WAIT_HEADER)
    {
      // Receive a header
      if (cur_off > sizeof(recv_pkt))
      {
        std::cout << "UNHANDLED CASE ERROR" << std::endl;
      }
      else if (cur_off < sizeof(recv_pkt))
      {
        while (cur_off < sizeof(recv_pkt))
        {
          std::cout << "Cur off was " << cur_off << std::endl;
          int recv_len = read(newsock, &recv_pkt.data[cur_off], sizeof(recv_pkt) - cur_off);
          cur_off += recv_len;
          std::cout << "Receiving partial header of size " << recv_len << std::endl;
        }
      }
      else
      {
        // Received a whole header, all is good
        std::cout << "Whole header received" << std::endl;
      }
      frame_size = recv_pkt.frame.height * recv_pkt.frame.step;
      std::cout << "Received a header with frame_id " << recv_pkt.frame.frame_id << 
        " height " << recv_pkt.frame.height << " width = " << recv_pkt.frame.width << std::endl;
      std::cout << "Camera is " << recv_pkt.frame.camera_name << " format is " << recv_pkt.frame.data_type << std::endl;
      if (/*recv_len != sizeof(recv_pkt)*/ false)
      {
        std::cout << "Incomplete header received" << std::endl;
      }
      else
      {
        cur_off = 0;
        state = ReceiveState::WAIT_PAYLOAD;
      }
    }
    else if (state == ReceiveState::WAIT_PAYLOAD)
    {
      int recv_len;
      while (cur_off < frame_size)
      {
        recv_len = read(newsock, buff, std::min((int)sizeof(buff), frame_size - cur_off));
        cur_off += recv_len;
        // TODO memcpy's here
      }
      cur_off -= frame_size;
      if (cur_off > 0)
      {
        // We received a part of the header
        memcpy(&recv_pkt,  &buff[recv_len - cur_off], cur_off);
      }

      state = ReceiveState::WAIT_HEADER;

    }

    continue;
    //int recv_len = recvfrom(sock, recv_pkt.data, sizeof(recv_pkt), 0, (struct sockaddr*)&si_other, &sock_len);
    int recv_len = read(newsock, recv_pkt.data, sizeof(recv_pkt));
    //std::cout << "Received packet of size " << recv_len << " status is " << recv_pkt.status << std::endl;
    if (recv_pkt.status != exp_status)
    {
      std::cout << "UNEXPECTED STATUS MISMATCH " << exp_status << " - " << recv_pkt.status << std::endl;
      exp_status = recv_pkt.status + 1;
    }
    else
      exp_status++;
    std::cout << "Received packet of size " << recv_len << " with status " << recv_pkt.status << std::endl;
  }
  close(sock);
  close(newsock);
  return 0;
}
