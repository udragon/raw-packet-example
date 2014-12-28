/*

Raw Packets sender.
Written by idon at 28/12/14.

Usage: <device> <file>

*/

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
 
#define BUF_SIZ    1024
 
int main(int argc, char *argv[])
{
    int sockfd;
    struct ifreq if_idx;
    int tx_len = 0;
    char sendbuf[BUF_SIZ];
    struct sockaddr_ll socket_address;
    char ifName[IFNAMSIZ];
    FILE *filefd;
    int filesize = 0;
    char c;

    /* Parse arguments */
    if (argc != 3) {
	printf("Usage: %s <device> <file>\n", argv[0]);
        return -1;
    }

    /* Get interface name */
    strcpy(ifName, argv[1]);
  
    /* Open RAW socket to send on */
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
    perror("socket");
    }
     
    /* Get the index of the interface to send on */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
    perror("SIOCGIFINDEX");

    /* Index of the network device */
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen = 0x00;

    /* Open file */
    filefd = fopen(argv[2], "r");
    if (filefd == NULL) {
        printf("Cannot open file.\n");
        return -1;
    }
    
    /* Read data to buffer */ 
    memset(sendbuf, 0, BUF_SIZ);
    do {
      c = fgetc (filefd);
      sendbuf[filesize++] = c;
    } while (c != EOF);

    /* Send packet */
    if (sendto(sockfd, sendbuf, filesize, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0) {
        printf("Send failed.\n");
        return -1;
    }

    printf("Sent %d bytes packet on %s.\n", filesize, ifName);    
    return 0;
}
