#include <gtk/gtk.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 512

char * arg_in_drive;
char * arg_out_drive;

GtkWidget *g_lbl_desc;
GtkWidget *g_lbl_status;
GtkFileChooser *g_sav_folder;

int main(int argc, char *argv[])
{
    //check for arguments
    if(argc == 3){
        arg_in_drive = argv[1];
        arg_out_drive = argv[2];
    }
    else {
        printf("Two arguments expected.\n");
        exit(0);
    }

    GtkBuilder      *builder;
    GtkWidget       *window;

    gtk_init(&argc, &argv);

    //load glade file
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "data/window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    g_lbl_desc = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_desc"));
    g_lbl_status = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_status"));
    g_sav_folder = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "sav_folder"));

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}


void status_text (char text[100])
{
    gtk_label_set_text(GTK_LABEL(g_lbl_status), text);
}


char* readable_fs(size_t size, char *buf)
{
    int i = 0;
    const char* units[] = {"B", "KB", "MB", "GB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*zu %s Done", i, size, units[i]);
    return buf;
}


void read_to_file(char drive[15], char sav_folder[2000])
{

    int drive_fd, file_path_fd;    //file descriptors
    ssize_t i = 0, progress = 0;    //initialize progress counters
    char buffer[BUF_SIZE], *full_file_path, buffer_hr[20];      //initialize buffers and file path


    full_file_path = strcat(sav_folder, "/backup.img");
    
    //input file descriptor
    drive_fd = open (drive, O_RDONLY);
    if (drive_fd == -1){
            status_text("Error reading drive, please run with sudo");
    }

    //output file descriptor
    file_path_fd = open(full_file_path, O_WRONLY | O_CREAT, 0666);
    if(file_path_fd == -1){
        status_text("Error creating file");
    }

    //seek past mbr
    lseek(drive_fd, 512, SEEK_SET);
    
    while(i < 64423168){
            i ++;
            write(file_path_fd, &buffer, read(drive_fd, &buffer, BUF_SIZE));
            progress += BUF_SIZE;
            g_print("\n progress: %zu \n i: %zu", progress, i);
            status_text(readable_fs(progress, buffer_hr));
            while(gtk_events_pending()) gtk_main_iteration();
    }

    //close file descrptors
    close(drive_fd);
    close(file_path_fd);
    progress = 0;
    status_text("Done");
}

void write_to_drive(char sav_folder[2000], char drive[15])
{

    int file_path_fd, drive_fd;    //file descriptors
    ssize_t i = 0, progress = 0;    //initialize progress counters
    char buffer[BUF_SIZE], *full_file_path, buffer_hr[20];      //initialize buffers and file path

    full_file_path = strcat(sav_folder, "/backup.img");
    
    //input file descriptor
    file_path_fd = open(full_file_path, O_RDONLY);
    if (file_path_fd == -1){
            status_text("Error opening file");
    }

    //output file descriptor
    drive_fd = open(drive, O_WRONLY);
    if(drive_fd == -1){
        status_text("Error opening drive");
    }

    //seek past mbr
    lseek(drive_fd, 512, SEEK_SET);

    while(i < 64423168){
            i ++;
            write(drive_fd, &buffer, read(file_path_fd, &buffer, BUF_SIZE));
            progress += BUF_SIZE;
            g_print("\n progress: %zu \n i: %zu", progress, i);
            status_text(readable_fs(progress, buffer_hr));
            while(gtk_events_pending()) gtk_main_iteration();
    }

    //close file descriptors
    close(drive_fd);
    close(file_path_fd);
    progress = 0;
    status_text("Done");
}


void on_btn_read_clicked()
{
    read_to_file(arg_in_drive, gtk_file_chooser_get_filename(g_sav_folder));
}


void on_btn_write_clicked()
{
    write_to_drive(gtk_file_chooser_get_filename(g_sav_folder), arg_out_drive);

}


void on_window_main_destroy()
{
    gtk_main_quit();
}