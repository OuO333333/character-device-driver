#include <linux/kernel.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

#define BUFFER_LENGTH 512
#define WR_BUFFER_INDEX _IOW('a','a',int32_t*)
#define RD_BUFFER_INDEX _IOR('a','b',int32_t*)
#define WR_BUFFER _IOW('a','c',char*)
#define RD_BUFFER _IOR('a','d',char*)

void test_access_restriction();
void test_rw_operation(char *ans, char *block);
void test_ioctl(char *ans);

int main(int argc, char *argv[])
{
    if (argc == 1){
        printf("Incorrect format.\n");
        return 0;
    }

    int fd;
    fd = open("/dev/mychardev-0", O_RDWR);
    if(fd < 0){
        printf("FD[0] not open\n");
        return 0;
    }
    /* 字元陣列的變數名稱可當指標 */
    char result[BUFFER_LENGTH] = {'\0'};

    if(strcmp(argv[1], "r") == 0){
        if(argc == 2){
            /* timdev r */
            read(fd, result, BUFFER_LENGTH);
            printf("result = \"%s\"\n", result);
        }
        else if(argc == 3){
            /* timdev r index */
            int32_t buffer_index = atoi(argv[2]);
            if(buffer_index >= BUFFER_LENGTH){
                printf("Buffer index should no bigger than 512.\n");
                return 0;
            }
            ioctl(fd, WR_BUFFER_INDEX, (int32_t *)&buffer_index);
            ioctl(fd, RD_BUFFER, (char *)result + buffer_index);
            printf("result[%d] = \"%c\"\n", buffer_index, result[buffer_index]);
        }
        else{
            printf("Incorrect format.\n");    
        }
    }
    else if(strcmp(argv[1], "w") == 0){
        if(argc == 3){
            /* timdev w string */
            if(strlen(argv[2]) >= BUFFER_LENGTH){
                printf("String length should no bigger than BUFFER_LENGTH.\n");
                return 0;
            }
            write(fd, argv[2], BUFFER_LENGTH);
        }
        else if(argc == 4){
            /* timdev w index char */
            int32_t buffer_index = atoi(argv[2]);
            if(buffer_index >= BUFFER_LENGTH){
                printf("Buffer_index should no bigger than BUFFER_LENGTH.\n");
                return 0;
            }
            if(strlen(argv[3]) > 1){
                printf("Should only write one character.\n");
                return 0;
            }
            ioctl(fd, WR_BUFFER_INDEX, (int32_t *)&buffer_index);
            ioctl(fd, WR_BUFFER, (char *)argv[3]);
        }
        else{
            printf("Incorrect format.\n");    
        }
    }
    else{
        printf("Incorrect format.\n");    
    }
}
