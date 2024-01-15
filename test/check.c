#include <linux/kernel.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/wait.h>
#include<sys/ioctl.h>

#define WR_BUFFER_INDEX _IOW('a','a',int32_t*)
#define RD_BUFFER_INDEX _IOR('a','b',int32_t*)
#define WR_BUFFER _IOW('a','c',char*)
#define RD_BUFFER _IOR('a','d',char*)

void test_access_restriction();
void test_rw_operation(char *ans, char *block);
void test_ioctl(char *ans);
void test_waiting_queue();

int main(void)
{
    char ans[] = {
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
    };

    char *ptr_ans = &ans[0];

    char block[] = {
        'm', 'o', 'd', 'i', 'f', 'y', ' ', 'm', 's', 'g'
    };

    char *ptr_block = &block[0];

    test_access_restriction();
	
    test_rw_operation(ptr_ans, ptr_block);

    test_ioctl(ptr_ans);

    return 0;
}

void test_rw_operation(char *ans, char *block){
	
    int fd;
    int i;
    fd = open("/dev/mychardev-0", O_RDWR);
    if (fd < 0)
        printf("FD[0] not open\n");

    /* write時，write長度必須小於傳送的的buffer的data長度 */
    write(fd, block, 10);

    /* 
    * read時，read長度必須小於maintain的buffer的長度
    * 大於則會回傳maintain的buffer的長度
    * read(fd, ans + 20, 10); 表示將結果從 ans[20] 開始寫入
    */
    read(fd, ans, 10);

    for (i = 0; i < 10; i++)
        assert(ans[i] == block[i]);
	
    close(fd);
}

void test_access_restriction(){
    int i, count = 0;
    int fd[10];
    for(i = 0; i < 10; i++){
        fd[i] = open("/dev/mychardev-0", O_RDWR | O_NONBLOCK);
        if(fd[i] < 0)
            count++;
    }
    for(i = 0; i < 10; i++){
        if(fd[i] >= 0)
            close(fd[i]);
    }
    assert(count == 9);
}

void test_ioctl(char *ans){
    int fd;
    int32_t buffer_index = 5;
    char ch = 'Y';
    fd = open("/dev/mychardev-0", O_RDWR);

    /* writing buffer_index to driver */
    ioctl(fd, WR_BUFFER_INDEX, (int32_t*) &buffer_index);

    /* reading buffer_index from driver */
    ioctl(fd, RD_BUFFER_INDEX, (int32_t*) &buffer_index);
    assert(buffer_index == 5);

    /* writing character to driver(buffer) */
    ioctl(fd, WR_BUFFER, (char*) &ch);

    /* reading character from driver(buffer) */
    ioctl(fd, RD_BUFFER, (char*) ans + buffer_index);
    assert(ans[buffer_index] == 'Y');

    close(fd);
}