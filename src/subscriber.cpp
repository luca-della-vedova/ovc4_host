#include <unistd.h>

#include <subscriber.hpp>

#include <ethernet_packetdef.h>

Subscriber::Subscriber(int cam_id)
{
  std::cout << "Creating subscriber n. " << cam_id << std::endl;
  sock = socket(AF_INET, SOCK_STREAM, 0);

  si_self.sin_family = AF_INET;
  si_self.sin_port = htons(BASE_PORT + cam_id);
  si_self.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(sock, (struct sockaddr *) &si_self, sizeof(si_self));
  std::cout << "Constructed subscriber n. " << cam_id << std::endl;
}

Subscriber::~Subscriber()
{
  close(sock);
  close(recv_sock);
}

// TODO atomic to signal from receive thread and mutexes to avoid corrupted frames
OVCImage Subscriber::getFrame()
{

  return ret_img;
}

void Subscriber::receiveThread()
{
  unsigned int si_size = sizeof(si_other);
  listen(sock,1);
  recv_sock = accept(sock, (struct sockaddr *)&si_other, &si_size);
  std::cout << "Listening" << std::endl;
  // TODO proper while condition
  int cur_off = 0;
  int frame_size = 0;
  int exp_status = 0;
  ether_tx_packet_t recv_pkt;
  while (1)
  {
    if (state_ == ReceiveState::WAIT_HEADER)
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
          int recv_len = recv(recv_sock, &recv_pkt.data[cur_off], sizeof(recv_pkt) - cur_off, MSG_WAITALL);
          cur_off += recv_len;
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
      // Allocate CV mat
      ret_img.image.create(recv_pkt.frame.height, recv_pkt.frame.width, CV_8UC1); // TODO flexible data type, for now only yuv420
      if (/*recv_len != sizeof(recv_pkt)*/ false)
      {
        std::cout << "Incomplete header received" << std::endl;
      }
      else
      {
        cur_off = 0;
        state_ = ReceiveState::WAIT_PAYLOAD;
      }
    }
    else if (state_ == ReceiveState::WAIT_PAYLOAD)
    {
      int recv_len;
      while (cur_off < frame_size)
      {
        recv_len = recv(recv_sock, &ret_img.image.data[cur_off], frame_size - cur_off, MSG_WAITALL);
        //recv_len = read(recv_sock, buff, std::min((int)sizeof(buff), frame_size - cur_off));
        cur_off += recv_len;
        //usleep(10);
        // TODO memcpy's here
      }
      cur_off -= frame_size;
      if (cur_off > 0)
      {
        // We received a part of the header
        //memcpy(&recv_pkt,  &buff[recv_len - cur_off], cur_off);
      }

      state_ = ReceiveState::WAIT_HEADER;

    }

    continue;
    //int recv_len = recvfrom(sock, recv_pkt.data, sizeof(recv_pkt), 0, (struct sockaddr*)&si_other, &sock_len);
    int recv_len = read(recv_sock, recv_pkt.data, sizeof(recv_pkt));
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

}
