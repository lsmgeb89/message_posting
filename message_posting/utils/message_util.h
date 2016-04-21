#ifndef MESSAGE_POSTING_MESSAGE_UTIL_H
#define MESSAGE_POSTING_MESSAGE_UTIL_H

#include <ctime>
#include <cstring>
#include <sstream>

#include "socket_util.h"
#include "log_util.h"

namespace message_posting {

namespace utils {

enum RequestType {
  Login = 0,
  DisplayKnownUsersNames = 1,
  DisplayConnectedUsersNames = 2,
  SendMessage2User = 3,
  SendMessage2ConnectedUsers = 4,
  SendMessage2KnownUsers = 5,
  GetMessages = 6,
  Exit = 7
};

inline std::istream& operator>>(std::istream& is, RequestType& request_type) {
  std::string content;
  uint8_t val(0);
  if (is >> content) {
    val = std::stoul(content);
    if (val >= DisplayKnownUsersNames && val <= Exit) {
      request_type = static_cast<RequestType>(val);
    } else {
      error_client << "Wrong Input" << std::endl;
    }
  } else {
    // TODO:: error handling
  }
  return is;
}

enum MessageType {
  Invalid = 0,
  Request = 1,
  Response = 2
};

typedef std::pair<std::string, std::string> MessageProperty;
typedef std::vector<MessageProperty> PropertyList;
typedef struct EmptyContent {} InvalidContent;

constexpr uint8_t max_len = 32;
static PropertyList dummy_list;

struct TextMessage {
  char sender_[max_len];
  std::time_t time_;
  char words_[max_len];
};

struct TextMessageContent {
  char recipient_[max_len];
  TextMessage text_msg_;
};

struct RequestMessage {
  RequestType req_type_;
  union {
    EmptyContent empty_;
    char name_[max_len];
    TextMessageContent text_;
  };
};

struct Message {
  MessageType type_;
  union {
    InvalidContent invalid_;
    RequestMessage request_;
    char response_[max_len];
  };
};

class MessageUtil {
 public:
  // default constructor
  MessageUtil(const Socket& socket)
    : internal_socket_(socket) {
    ClearStream();
    ClearBuf();
    ResetMsg();
  }

  // Setters
  void SetResponse(PropertyList& property_list = dummy_list) {
    PackHead(Response);
    for (auto property : property_list) {
      msg_str_stream_ << property.first << delimiter_;
      msg_str_stream_ << property.second << delimiter_;
    }
    PackTail();
  }

  void SetRequest(const RequestType& request_type,
                  PropertyList& property_list = dummy_list) {
    PackHead(Request);
    msg_str_stream_ << request_type << delimiter_;
    for (auto property : property_list) {
      msg_str_stream_ << property.first << delimiter_;
      msg_str_stream_ << property.second << delimiter_;
    }
    PackTail();
  }

  // Getters
  // TODO: check all getters' type

  const RequestType GetRequestType(void) const {
    return parsed_msg_.request_.req_type_;
  }

  const char* GetClientName(void) const {
    return parsed_msg_.request_.name_;
  }

  // TODO: copy or not
  const char* GetRecipient(void) const {
    return parsed_msg_.request_.text_.recipient_;
  }

  // TODO: copy or not
  const TextMessage& GetTextMessage(void) const {
    return parsed_msg_.request_.text_.text_msg_;
  }

  const char* GetResponse(void) const {
    return parsed_msg_.response_;
  }

  // Communication
  ssize_t Write(void) {
    ClearBuf();
    std::memcpy(msg_buf_.data(), msg_str_stream_.str().c_str(), msg_str_stream_.str().size());

    info_message << "[Write]" << msg_str_stream_.str() << std::endl;

    ssize_t count = internal_socket_.Write(msg_buf_.data(), msg_buf_size_);
    if (-1 == count) {
      perror(LOG_ERROR_MODULE_MESSAGE"[write]");
      throw std::runtime_error("");
    }
    return count;
  }

  ssize_t Read(void) {
    ssize_t count(0);

    ClearBuf();
    while(count != msg_buf_size_) {
      ssize_t res = internal_socket_.Read(msg_buf_.data() + count,
                                          msg_buf_size_ - count);
      if (-1 == res) {
        perror(LOG_ERROR_MODULE_MESSAGE"[read]");
        throw std::runtime_error("");
      }
      count += res;
    }

    msg_str_stream_.str(msg_buf_.data());
    info_message << "[Read]" << msg_str_stream_.str() << std::endl;

    ParseMessage();
    return count;
  }

 private:
  const Socket& internal_socket_;
  static constexpr uint16_t msg_buf_size_ = 128;
  static constexpr char delimiter_ = '|';
  static constexpr char msg_beg_ = '<';
  static constexpr char msg_end_ = '>';
  // TODO: more const for res, req, etc
  std::stringstream msg_str_stream_;
  std::array<char, msg_buf_size_> msg_buf_;
  Message parsed_msg_;

  void ClearStream(void) {
    msg_str_stream_.str(std::string()); // clear the content in sstream
    msg_str_stream_.clear(); // clear error and eof flags
  }

  void ClearBuf(void) { msg_buf_.fill(0); }

  void ResetMsg(void) {
    parsed_msg_.type_ = Invalid;
    parsed_msg_.invalid_ = {};
  }

  void PackHead(const MessageType& type) {
    ClearStream();
    if (Request == type) {
      msg_str_stream_ << msg_beg_ << delimiter_ << "req" << delimiter_;
    } else {
      msg_str_stream_ << msg_beg_ << delimiter_ << "res" << delimiter_;
    }
  }

  void PackTail(void) {
    msg_str_stream_ << msg_end_;
  }

  // parser helper
  static std::string ExtractNext(const std::string& msg_str,
                                 std::string::size_type& parse_count) {
    std::string::size_type beg = msg_str.find(delimiter_, parse_count);
    parse_count = beg + 1;
    std::string::size_type end = msg_str.find(delimiter_, parse_count);
    parse_count = end;
    return msg_str.substr(beg + 1, end - beg - 1);
  }

  void ParseMessageType(const std::string& msg_str,
                        std::string::size_type& parse_count) {
    std::string info;
    uint8_t req_type;

    // not a valid message
    if (msg_str[0] != msg_beg_ ||
        msg_str.find(msg_end_) == std::string::npos) {
      goto error;
    }

    info = ExtractNext(msg_str, parse_count);

    if ("req" == info) {
      parsed_msg_.type_ = Request;
      parsed_msg_.request_.empty_ = {};
    } else if ("res" == info) {
      parsed_msg_.type_ = Response;
    } else {
      goto error;
    }

    if (Request == parsed_msg_.type_) {
      info = ExtractNext(msg_str, parse_count);
      req_type = std::stoul(info);
      if (req_type > Exit) {
        goto error;
      } else {
        parsed_msg_.request_.req_type_ = static_cast<RequestType>(req_type);
      }
    }

    return;

    error:
      ResetMsg();
      throw std::runtime_error("");
  };

  void ParseMessage(void) {
    const std::string& msg_str = msg_str_stream_.str();
    std::string::size_type parse_count(0);

    ResetMsg();

    ParseMessageType(msg_str, parse_count);

    // TODO: fill text message
    while (parse_count != msg_str.size() - 2) {
      std::string key = ExtractNext(msg_str, parse_count);
      std::string value = ExtractNext(msg_str, parse_count);
      if ("n" == key) {
        std::strcpy(parsed_msg_.request_.name_, value.c_str());
      } else if ("s" == key) {
        std::strcpy(parsed_msg_.request_.text_.text_msg_.sender_, value.c_str());
      } else if ("r" == key) {
        std::strcpy(parsed_msg_.request_.text_.recipient_, value.c_str());
      } else if ("t" == key) {
        std::istringstream stream(value);
        stream >> parsed_msg_.request_.text_.text_msg_.time_;
      } else if ("m" == key) {
        std::strcpy(parsed_msg_.request_.text_.text_msg_.words_, value.c_str());
      } else if ("z" == key) {
        std::strcpy(parsed_msg_.response_, value.c_str());
      }
    }
  }
};

} // namespace utils

} // namespace message_posting

#endif // MESSAGE_POSTING_MESSAGE_UTIL_H
