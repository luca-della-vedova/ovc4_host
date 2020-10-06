#include <thread>
#include <iostream>

#include <subscriber.hpp>

int main(int argc, char **argv)
{
  Subscriber sub;
  std::thread thread(&Subscriber::receiveThread, &sub);
  while (1)
  {
    auto frames = sub.getFrames();
    std::cout << "Got a pair of frames" << std::endl;
    cv::Mat frame1, frame2;
    cv::cvtColor(frames[0].image, frame1, cv::COLOR_YUV2BGR_I420);
    cv::cvtColor(frames[1].image, frame2, cv::COLOR_YUV2BGR_I420);
    cv::imshow("Right", frame1);
    cv::waitKey(1);
    cv::imshow("Left", frame2);
    cv::waitKey(1);
  }
  thread.join();
  return 0;
}
