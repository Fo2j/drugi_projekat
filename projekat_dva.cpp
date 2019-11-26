#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PKT_SIZE (640 * 480 * 4)

using namespace std;

void background_write() {
  FILE *fp;
  for (int y = 0; y < 480; y++)
  {

for (int x = 0; x < 640; x++)
    {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL)
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

        fprintf(fp, "%d,%d,%#04x\n", x, y, 0x0000);
 fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
    }
  }
}
void horizontal_write() {
        FILE *fp;
  for (int x=0 ; x < 640; x++)
  {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL)
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }


      fprintf(fp, "%d,%d,%#04x\n", x, 239, 0x001F);
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
  }
}

void vertical_write() {
  FILE *fp;
  for (int y=0 ; y < 480; y++)
  {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL)
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", 319, y, 0x001F);
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
  }
}

void square(int x1, int x2, int y1, int y2){

 FILE *fp;

for (int y = y1; y < y2; y++) 
  {
    for (int x = x1; x < x2; x++) 
    {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL) 
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", x, y, 0xF800); 
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
   }
  }

}

void erase_square(int x1, int x2, int y1, int y2){

 FILE *fp;

for (int y = y1; y < y2; y++) 
  {
    for (int x = x1; x < x2; x++) 
    {
        fp = fopen("/dev/vga_dma", "w");
         if (fp == NULL) 
        {
         printf("Cannot open /dev/vga for write\n");
        exit(EXIT_FAILURE);
         }

      fprintf(fp, "%d,%d,%#04x\n", x, y, 0x0000); 
        fclose(fp);
         if (fp == NULL)
         {
         printf("Cannot close /dev/vga\n");
         exit(EXIT_FAILURE);
         }
   }
  }

}



int y1=100, y2=140, x1=140, x2=180; //drugi kvadrant
int x1n=140, x2n=180, y1n=100, y2n=140;



int main (){
	
	
	
        char komanda='n';

        background_write();
        horizontal_write();
        vertical_write();
	square(x1,x2,y1,y2);


  while(komanda!='q'){

	 cin >> komanda;

	if(komanda=='w'){
       	 y1n=y1-10;
       	 y2n=y2-10;
	}

	if(komanda=='s'){
       	 y1n=y1+10; 
         y2n=y2+10;
	}

	if(komanda=='a'){
         x1n=x1-10;
         x2n=x2-10;
	}

	if (komanda=='d'){
         x1n=x1+10;
         x2n=x2+10;
	}
	
	
	erase_square(x1,x2,y1,y2);
	cout<<x1<<"      "<<x1n<<endl;
	square(x1n,x2n,y1n,y2n);
	y1=y1n;
	y2=y2n;
	x1=x1n;
	x2=x2n;
 }



return 0;
}

