#include <arpa/inet.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
namespace po = boost::program_options;

void isvalidip(const string &ipaddr) {
  struct sockaddr_in sock;
  if (inet_pton(AF_INET, ipaddr.c_str(), &(sock.sin_addr)) != 1) {
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "ip");
  }
}
void isvalidport(int port) {
  if (port < 1 || port > 65535) {
    throw po::validation_error(po::validation_error::invalid_option_value,
                               "port");
  }
}

int main(int argc, char **argv) {
  po::options_description desc("Options");
  desc.add_options()("help,h", "Вывод справки")(
      "ip,i",
      po::value<string>()->notifier(isvalidip)->default_value("172.16.40.1"),
      "Ввод IP-адреса")(
      "port,p", po::value<int>()->notifier(isvalidport)->default_value(7),
      "Ввод порта");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << endl;
      return 0;
    }

    string ipaddr = vm["ip"].as<string>();
    int port = vm["port"].as<int>();

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Ошибка создания сокета");
    }

    sockaddr_in srv_addr{};
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    srv_addr.sin_addr.s_addr = inet_addr(ipaddr.c_str());

    int rc = connect(client_socket, reinterpret_cast<sockaddr *>(&srv_addr),
                     sizeof(sockaddr_in));
    if (rc == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Ошибка подключения к серверу");
    }

    string MSG;
    cout << "Введите сообщение на сервер: ";
    getline(cin, MSG);

    rc = send(client_socket, MSG.c_str(), MSG.size(), 0);
    if (rc == -1) {
      throw std::system_error(errno, std::generic_category(),
                              "Ошибка отправки сообщения");
    }
    
    const int bufsize = 1024;
    std::unique_ptr<char[]> buffer(new char[bufsize]);
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);

    string res;
    while (true) {
      rc = recvfrom(client_socket, buffer.get(), bufsize - 1, 0,
                    reinterpret_cast<sockaddr *>(&from_addr), &from_len);

      if (rc == -1) {
        throw std::system_error(errno, std::generic_category(),
                                "Ошибка получения сообщения");
      }
      buffer[rc] = '0';
      res.append(buffer.get(), rc);

      if (rc < bufsize - 1) {
        break;
      }
    }
    cout << "Ответ от сервера: " << res << endl;

    close(client_socket);
  } catch (const std::system_error &e) {
    cerr << e.what() << endl;
    return 1;
  } catch (const std::exception &e) {
    cerr << "Ошибка ввода: " << e.what() << endl;
    return 1;
  } catch (...) {
    cerr << "Неизвестная ошибка" << endl;
    return 1;
  }

  return 0;
}
