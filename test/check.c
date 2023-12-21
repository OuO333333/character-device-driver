#include <linux/kernel.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<sys/ioctl.h>

#define WR_BUFFER_INDEX _IOW('a','a',int32_t*)
#define RD_BUFFER_INDEX _IOR('a','b',int32_t*)

void test_access_restriction();
void test_rw_operation(unsigned char *ans, unsigned char *block);
void test_ioctl();

int main(void)
{
    unsigned char ans[] = {
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00,
    };

    unsigned char block[] = {
        'm', 'o', 'd', 'i', 'f', 'y', ' ', 'm', 's', 'g'
    };
	
    test_access_restriction();
	
    test_rw_operation(ans, block);
    // test_rw_operation(&ans, &block);會出現奇怪的warning

    test_ioctl();
    return 0;
}

void test_rw_operation(unsigned char *ans, unsigned char *block){
	
    int fd;
    int i;
    fd = open("/dev/mychardev-0", O_RDWR);
    if (fd < 0)
        printf("FD[0] not open\n");
    /* 
     * read時，read長度必須小於maintain的buffer的長度
     * 大於則會回傳maintain的buffer的長度
     */
    read(fd, ans, 11);
    /* write時，write長度必須小於傳送的的buffer的data長度 */
    write(fd, block, 10);
    read(fd, ans + 20, 10);
	
    for (i = 0; i < 32; i++)
        printf("ans[%d] = %c\n", i, ans[i]);
	
    for (i = 20; i < 30; i++)
        assert(ans[i] == block[i - 20]);
	
    close(fd);
}

void test_access_restriction(){
    int i, count = 0;
    int fd[10];
    for(i = 0; i < 10; i++){
        fd[i] = open("/dev/mychardev-0", O_RDWR);
        if(fd[i] < 0)
            count++;
    }
    for(i = 0; i < 10; i++){
        if(fd[i] >= 0)
            close(fd[i]);
    }
    assert(count == 9);
}

void test_ioctl(){
	int fd;
	int32_t buffer_index = 5;
	char buf[4] = {'x', 'y', 'z'};
	fd = open("/dev/mychardev-0", O_RDWR);

    /* writing buffer_index to driver */
    ioctl(fd, WR_BUFFER_INDEX, (int32_t*) &buffer_index);

    /* reading buffer_index from driver */
    ioctl(fd, RD_BUFFER_INDEX, (int32_t*) &buffer_index);
	assert(buffer_index == 5);
    printf("buffer_index is %d\n", buffer_index);
	
	close(fd);
}
