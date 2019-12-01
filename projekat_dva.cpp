#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PKT_SIZE (640 * 480 * 4)
#define PORT 8080

void sock_write(int sockfd, std::string s) {
  write(sockfd, s.c_str(), s.size());
}

int release_sec(int *pids, int pid) {
  for (int i = 0; i < 4; i++) {
    if (pids[i] == pid) {
      pids[i] = 0;
      return i;
    }
  }
  return -1;
}

int acquire_sec(int *pids, int pid) {
  for (int i = 0; i < 4; i++) {
    if (pids[i] == 0) {
      pids[i] = pid;
      return i;
    }
  }
  return -1;
}

void print_sec_list(int *pids, int n) {
  std::cout << "Client list: ";
  for (int i = 0; i < n; i++)
    std::cout << pids[i] << " ";
  std::cout << "\n";
}

std::string sec_n_to_str(int n) {
  if (n == 0)
    return "Top left";
  if (n == 1)
    return "Top right";
  if (n == 2)
    return "Bottom left";
  if (n == 3)
    return "Bottom right";

  return "Undefined";
}

int first_free_sec(int *pids, int n) {
  for (int i = 0; i < n; i++) {
    if (pids[i] == 0)
      return i;
  }
  return -1;
}
std::string sock_read(int sockfd) {
  const int MAX = 256;
  char buff[MAX];
  bzero(buff, MAX);
  read(sockfd, buff, sizeof(buff));
  std::string message(buff);

  return message;
}

using namespace std;

void background_write() {
  FILE *fp;
  for (int y = 0; y < 480; y++) {

    for (int x = 0; x < 640; x++) {
      // fp = fopen("/dev/vga_dma", "w");
      fp = fopen("/tmp/vga_buffer", "w");
      if (fp == NULL) {
        printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
      }

      fprintf(fp, "%d,%d,%#04x\n", x, y, 0x0000);
      fclose(fp);
      if (fp == NULL) {
        printf("Cannot close /dev/vga\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
void horizontal_write() {
  FILE *fp;
  for (int x = 0; x < 640; x++) {
    // fp = fopen("/dev/vga_dma", "w");
    fp = fopen("/tmp/vga_buffer", "w");
    if (fp == NULL) {
      printf("Cannot open /dev/vga for write\n");
      exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d,%d,%#04x\n", x, 239, 0x001F);
    fclose(fp);
    if (fp == NULL) {
      printf("Cannot close /dev/vga\n");
      exit(EXIT_FAILURE);
    }
  }
}

void vertical_write() {
  FILE *fp;
  for (int y = 0; y < 480; y++) {
    // fp = fopen("/dev/vga_dma", "w");
    fp = fopen("/tmp/vga_buffer", "w");
    if (fp == NULL) {
      printf("Cannot open /dev/vga for write\n");
      exit(EXIT_FAILURE);
    }

    fprintf(fp, "%d,%d,%#04x\n", 319, y, 0x001F);
    fclose(fp);
    if (fp == NULL) {
      printf("Cannot close /dev/vga\n");
      exit(EXIT_FAILURE);
    }
  }
}

void square(int x1, int x2, int y1, int y2) {

  FILE *fp;

  for (int y = y1; y < y2; y++) {
    for (int x = x1; x < x2; x++) {
      // fp = fopen("/dev/vga_dma", "w");
      fp = fopen("/tmp/vga_buffer", "w");
      if (fp == NULL) {
        printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
      }

      fprintf(fp, "%d,%d,%#04x\n", x, y, 0xF800);
      fclose(fp);
      if (fp == NULL) {
        printf("Cannot close /dev/vga\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void erase_square(int x1, int x2, int y1, int y2) {

  FILE *fp;

  for (int y = y1; y < y2; y++) {
    for (int x = x1; x < x2; x++) {
      // fp = fopen("/dev/vga_dma", "w");
      fp = fopen("/tmp/vga_buffer", "w");
      if (fp == NULL) {
        printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
      }

      fprintf(fp, "%d,%d,%#04x\n", x, y, 0x0000);
      fclose(fp);
      if (fp == NULL) {
        printf("Cannot close /dev/vga\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void move_rect(std::string cmd, int *x1, int *x2, int *y1, int *y2, int first_free) {

  int x1n = *x1;
  int x2n = *x2;
  int y1n = *y1;
  int y2n = *y2;

  if (cmd == "w") {
    y1n -= 10;
    y2n -= 10;
  }

  if (cmd == "s") {
    y1n += 10;
    y2n += 10;
  }

  if (cmd == "a") {
    x1n -= 10;
    x2n -= 10;
  }

  if (cmd == "d") {
    x1n += 10;
    x2n += 10;
  }

 switch(first_free)	{

	case 0:
		if(x1n<0) {
			x1n=0;
			x2n=40;
		}
		if(x2n>319) {
			x2n=319;
			x1n=279;
		}
		if(y1n<0) {
			y1n=0;
			y2n=40;
		}
		if(y2n>239){
			y2n=239;
			y1n=199;
		}
		break;

	case 1:
                if(x1n<319){
                        x1n=319;
			x2n=359;
		}
                if(x2n>639){
                        x2n=639;
			x1n=599;
               }
		 if(y1n<0) {
                        y1n=0;
			y2n=40;
		}
                if(y2n>239) {
                        y2n=239;
			y1n=199;
		}
                break;

	case 2:
                if(x1n<0) {
                        x1n=0;
			x2n=40;
		}
                if(x2n>319) {
                        x2n=319;
			x1n=279;
		}
                if(y1n<239){
                        y1n=239;
			y2n=279;
		}
                if(y2n>479) {
                        y2n=479;
			y1n=439;
               }
		 break;

	case 3:
                if(x1n<319) {
                        x1n=319;
			x2n=359;
		}
                if(x2n>639) {
                        x2n=639;
			x1n=599;
        	}
	        if(y1n<239) {
                        y1n=239;
			y2n=279;
		}
                if(y2n>479) {
                        y2n=479;
			y1n=439;
		}
                break;

}


  erase_square(*x1, *x2, *y1, *y2);
  square(x1n, x2n, y1n, y2n);

  *x1 = x1n;
  *x2 = x2n;
  *y1 = y1n;
  *y2 = y2n;
}

int main() {

  int sockfd, newsockfd, clilen;
  struct sockaddr_in serv_addr, cli_addr;
  int pids[4] = {0};

  // background_write();
  std::cout << "ovde sam\n";
  horizontal_write();
  vertical_write();

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "ERROR while opening socket\n";
    exit(1);
  }
  bzero(&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cerr << "ERROR while binding\n";
    exit(1);
  }
  std::cout << "Server Started, waiting for clients....\n";

  if ((listen(sockfd, 4)) != 0) {
    std::cout << "Listen Failed...\n";
    exit(1);
  } else
    std::cout << "Server listening...\n";

  int brojac = 0;
  clilen = sizeof(cli_addr);
  while (1) {
    /* If any slot is free check for new connections */
    if (pids[0] == 0 || pids[1] == 0 || pids[2] == 0 || pids[3] == 0) {
      /* Configure socket to be NONBLOCKING */
      int flags = fcntl(sockfd, F_GETFL, 0);
      fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
      newsockfd =
          accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

      if (newsockfd == -1) {
        /* If socket failed because there were no client requests, sleep 10ms */
        if (errno == EWOULDBLOCK) {
          usleep(10000);
        }
        /* Otherwise accept() failed with an error */
        else {
          std::cerr << "ERROR while accepting\n";
          exit(1);
        }
      }
      /* If accept() was succesfull, new client is connected */
      else {
        std::string cli_ip_str = inet_ntoa(cli_addr.sin_addr);
        /* Revert socket config to BLOCKING */
        int flags = fcntl(newsockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK));
        int first_free = first_free_sec(pids, 4);
        std::cout << "Client with IP: " << cli_ip_str
                  << " connected to section " << sec_n_to_str(first_free)
                  << "\n";
        int pid = fork();
        if (pid < 0) {
          std::cerr << "ERROR while forking\n";
          exit(1);
        }
        /* Child process serves the client
           Child process is terminating after if block
         */
        if (pid == 0) {
          int x1,x2,y1,y2;
          if(first_free==0) {

            x1 = 640/4-20; // na osnovu first_free
            x2 = 640/4+20;
            y1 = 480/4-20;
            y2 = 480/4+20;
          }
          else if(first_free==1) {

            x1 =  480 - 20; // na osnovu first_free
            x2 = 480 + 20;
            y1 = (480 / 4) - 20;
            y2 = (480 / 4) + 20;
          }
		
	else if(first_free==2) {

            x1 =  (640/4)-20; // na osnovu first_free
            x2 =  (640/4)+20;
            y1 =  360-20;
            y2 =  360+20;
          }

	else if(first_free==3) {

            x1 = 480 - 20; // na osnovu first_free
            x2 = 480 + 20;
            y1 = 360-20;
            y2 = 360+20;
          }

            std::cout << "main x1: " << x1 << " x2: " << x2 << " y1: "
                    << y1 << " y2: " << y2 << "\n";
          // clear_section(&frame, first_free);
          // draw_rect(&frame, first_free, 0, 0);
          close(sockfd);
          sock_write(newsockfd, "Controlling: " + sec_n_to_str(first_free));
          square(x1, x2, y1, y2);
          std::string cmd;
          do {
            cmd = sock_read(newsockfd);
            std::cout << "Section: " << sec_n_to_str(first_free)
                      << " cmd: " << cmd << "\n";
            move_rect(cmd, &x1, &x2, &y1, &y2, first_free);
            std::cout << "main x1: " << x1 << " x2: " << x2 << " y1: " << y1
                      << " y2: " << y2 << "\n";
            // move_rect(&frame, first_free, cmd, &x_off, &y_off);
          } while (cmd != "q" && cmd != "");
          // clear_section(&frame, first_free);
          close(newsockfd);

          exit(0);
        }
        /* Parent process add connected client to list
           Occupy free position in client list 'pids'
         */
        else {
          close(newsockfd);
          acquire_sec(pids, pid);
          print_sec_list(pids, 4);
        }
      }
    }

    /* Parrent process
       Checks if any of the child processes has terminated (client disconnected)
       On client disconnect release entry in PID list
     */
    for (int a = 0; a < 4; a++) {
      if (pids[a] != 0) {
        int status;
        if (waitpid(pids[a], &status, WNOHANG)) {
          std::cout << "Client disconnected\n";
          release_sec(pids, pids[a]);
          print_sec_list(pids, 4);
        }
      }
    }
  }
  close(sockfd);
  return 0;
}
