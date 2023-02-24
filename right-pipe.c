// right-pipe.c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const int buf_size = 40; //  ограничение на размер сообщения
    char str_buf[buf_size];

    char msg[3][40] = {"Hello, left!", "Nice to meet you too!", "Wonderful"};
    int msg_amount = 3;


    char *name_read = "from_left"; //  pipe откуда читает правый
    char *name_write = "from_right"; //  pipe куда пишет правый
    if (argc == 3) {
        name_read = argv[1];
        name_write = argv[2];
    }

    (void) umask(0);

    mknod(name_write, S_IFIFO | 0666, 0);

    int fd;
    size_t size;
    for (int i = 0; i < msg_amount; ++i) {
        // Секция чтения
        if ((fd = open(name_read, O_RDONLY)) < 0) {
            printf("Can\'t open FIFO for reading\n");
            exit(-1);
        }
        size = read(fd, str_buf, buf_size);
        if (size < 0) {
            printf("Can\'t read string from FIFO\n");
            exit(-1);
        }
        printf("Right received: %s\n", str_buf);
        if (close(fd) < 0) {
            printf("Right: Can\'t close FIFO\n");
            exit(-1);
        }
        // ---------------

        // Секция записи
        printf("Right writing: %s\n", msg[i]);
        if ((fd = open(name_write, O_WRONLY)) < 0) {
            printf("Can\'t open FIFO for writting\n");
            exit(-1);
        }
        int msg_size = sizeof(msg[i]);
        size = write(fd, msg[i], msg_size);
        if (size != msg_size) {
            printf("Can\'t write all string to FIFO\n");
            exit(-1);
        }
        if (close(fd) < 0) {
            printf("Right: Can\'t close FIFO\n");
            exit(-1);
        }
        // ---------------
    }
    printf("right pipe exit\n");
    return 0;
}
