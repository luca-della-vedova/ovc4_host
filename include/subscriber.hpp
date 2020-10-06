#include <arpa/inet.h>
#include <sys/socket.h>

#include <opencv2/opencv.hpp>


enum class ReceiveState
{
  WAIT_HEADER,
  WAIT_PAYLOAD,
};

// TODO sensor name / mode?
typedef struct OVCImage
{
  uint64_t t_sec;
  uint64_t t_nsec;
  uint64_t frame_id;
  cv::Mat image;
} OVCImage;

class Subscriber
{
private:
  static constexpr int BASE_PORT = 12345;
  static constexpr size_t TCP_RECEIVE_SIZE = 8192;
  ReceiveState state_ = ReceiveState::WAIT_HEADER;

  struct sockaddr_in si_self = {0}, si_other = {0};
  int sock;
  int recv_sock;

  OVCImage ret_img;


public:
  Subscriber(int cam_id);
  ~Subscriber();

  void receiveThread();

  OVCImage getFrame();

};
