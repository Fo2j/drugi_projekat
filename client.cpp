#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define PORT 8080

void sock_write(int sockfd, std::string s) {
  write(sockfd, s.c_str(), s.size());
}

std::string sock_read(int sockfd) {
  const int MAX = 256;
  char buff[MAX];
  bzero(buff, MAX);
  read(sockfd, buff, sizeof(buff));
  std::string message(buff);

  return message;
}

int main(int argc, char **argv) {
  std::string serv_ip;
  if (argc < 2) {
    std::cerr << "Please enter Server IP addr\n";
    return -1;
  } else {
    serv_ip = argv[1];
  }

  int sockfd = 0;
  struct sockaddr_in serv_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "\n Error: Could not create socket \n";
    return 1;
  } else {
    std::cout << "Socket Succesfully created...\n";
  }

  bzero(&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(serv_ip.c_str());
  serv_addr.sin_port = htons(PORT);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "\nError : Connection Failed \n";
    return 1;
  } else {
    std::cout << "\nConnected to server " << serv_ip << ":" << PORT << "\n";
    std::string message = sock_read(sockfd);
    std::cout << message << "\n";
  }

  char key;
  std::string send_str;
  while (1) {
    std::cin >> key;
    send_str = key;
    if (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == 'q')
      sock_write(sockfd, send_str);

    if (key == 'q') {
      close(sockfd);
      return 0;
    }
  }

  close(sockfd);
  return 0;

  // char key;
  // std::string send_str;
  // while (1) {
  //   key = getch();
  //   send_str = key;
  //   if (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == 'q')
  //     sock_write(sockfd, send_str);

  //   if (key == 'q') {
  //     close(sockfd);
  //     return 0;
  //   }
  // }
}
