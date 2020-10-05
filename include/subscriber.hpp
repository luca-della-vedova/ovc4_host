


enum class ReceiveState
{
  WAIT_HEADER,
  WAIT_PAYLOAD,
};


class Subscriber
{
private:
  ReceiveState state_ = WAIT_HEADER;


public:
  Subscriber(int cam_id);

};
