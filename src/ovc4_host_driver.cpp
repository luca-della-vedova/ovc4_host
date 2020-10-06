
#include <thread>

#include <subscriber.hpp>

const std::vector<int> CAMERA_IDS = {2,4};

int main(int argc, char **argv)
{
  std::vector<std::unique_ptr<std::thread>> threads;
  std::vector<std::unique_ptr<Subscriber>> subs;
  for (int i = 0; i < CAMERA_IDS.size(); ++i)
  {
    subs.push_back(std::make_unique<Subscriber>(CAMERA_IDS[i]));
    threads.push_back(std::make_unique<std::thread>(&Subscriber::receiveThread, subs[i].get()));
  }
  threads[0]->join();
  return 0;
}
