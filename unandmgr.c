//options --8G --mode=read/write (in file) (out file)

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define BUF_SIZE 512
#define PROGRESS_BAR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"

void progress_bar(char *current, char *total, double percent);
void read_to_file();
void write_to_drive();

char * arg_in_file;
char * arg_out_file;

size_t nand_sectors;

int main(int argc, char *argv[])
{
    //check for arguments
    if(argc == 5){
        arg_in_file = argv[3];
        arg_out_file = argv[4];
        if(strstr(argv[1], "8G")){
            nand_sectors = 0x1090500; 
        }else{ 
            nand_sectors = 0x3D70500;
        }

        if(strstr(argv[2], "read")){ 
            read_to_file(); 
        }else{ 
            write_to_drive(); 
        }
    }

    else {
        printf("Four arguments expected.\n");
        exit(0);
    }

    return 0;
}

void progress_bar(char *current, char *total, double percent){
    int val = (int) (percent * 100);
    int lpad = (int) (percent * 60);
    int rpad = 60 - lpad;
    printf ("\r%s / %s%3d%%  [%.*s%*s]", current, total, val, lpad, PROGRESS_BAR, rpad, "");
    fflush (stdout);
}


char* readable_fs(size_t size, char *buf)
{
    int i = 0;
    const char* units[] = {"B", "KB", "MB", "GB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*zu %s", i, size, units[i]);
    return buf;
}



void read_to_file()
{
    ssize_t in_file_fd, out_file_fd;    //file descriptors
    char buffer[BUF_SIZE], buffer_2[20], buffer_3[20];      //initialize buffers and file path
    
    printf("Now reading from %s to %s \n",arg_in_file, arg_out_file);

    //input file descriptor
    in_file_fd = open(arg_in_file, O_RDONLY);
    if (in_file_fd == -1){
        printf("Error reading drive, please run with sudo");
    }

    //output file descriptor
    out_file_fd = open(arg_out_file, O_WRONLY | O_CREAT);
    if(out_file_fd == -1){
        printf("Error creating file");
    }

    //seek past mbr
    lseek(in_file_fd, 512, SEEK_SET);
    
    for(double i = 0; i < nand_sectors; i ++){
        write(out_file_fd, &buffer, read(in_file_fd, &buffer, BUF_SIZE));
        progress_bar(readable_fs(i * 512, buffer_2), readable_fs(nand_sectors * 512, buffer_3), round((i / (double) nand_sectors)*100)/100);
    }

    //close file descrptors
    close(in_file_fd);
    close(out_file_fd);
}

void write_to_drive()
{
    ssize_t in_file_fd, out_file_fd;    //file descriptors
    char buffer[BUF_SIZE], buffer_2[20], buffer_3[20];      //initialize buffers and file path

    printf("Now writing image from %s to %s \n",arg_in_file, arg_out_file);

    //input file descriptor
    in_file_fd = open(arg_in_file, O_RDONLY);
    if (in_file_fd == -1){
        printf("Error reading image, does it exist ?");
    }

    out_file_fd = open(arg_out_file, O_WRONLY | O_CREAT, 0666);
    if(out_file_fd == -1){
        printf("Error reading drive, please run with sudo");
    }

    //seek past mbr
    lseek(out_file_fd, 512, SEEK_SET);

    for(double i; i < nand_sectors; i ++){
            write(out_file_fd, &buffer, read(in_file_fd, &buffer, BUF_SIZE));
            progress_bar(readable_fs(i * 512, buffer_2), readable_fs(nand_sectors * 512, buffer_3), round((i / (double) nand_sectors)*100)/100); 

    }

    //close file descriptors
    close(in_file_fd);
    close(out_file_fd);
}

