/*
 * routetracer - an implementation of traceroute for CS-471 project
 * Written by: Hugo Torres
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#define ICMP_MSG_LEN 100

int main(int argc, const char *argv[])
{
	if (argc != 2) {
		printf("usage: routetracer <ip address or hostname>\n");
		return -1;
	}

	struct addrinfo hints, *ret; //params for ret val of getaddrinfo
	struct sockaddr_in reply_addr, *ip;

  const char* dest_port = "9001";
	char *msg = "THE PORT IS OVER 9000!!!!";
	int ttl = 0;

	socklen_t reply_addr_len = sizeof(struct sockaddr);

  char ipv4[INET_ADDRSTRLEN];
  char dest[INET_ADDRSTRLEN];
  char icmp_msg[ICMP_MSG_LEN];
  int status;
  int src_sock;
  int recv_sock;

	//define what we want from getaddrinfo
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; //IPv4
	hints.ai_socktype = SOCK_DGRAM; //UDP packets

	//call getaddrinfo to fill ret, w/ error chk
	if ((status = getaddrinfo(argv[1], dest_port, &hints, &ret)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}

	//extract IPv4 address from ret
	ip = (struct sockaddr_in *)ret->ai_addr;

	//convert address from pure numbers to something easier to read
	inet_ntop(ret->ai_family, &(ip->sin_addr), dest, INET_ADDRSTRLEN);

	//kindly inform the user of which computer they are connecting to
	printf("Route for: %s\n", dest);

	//create an outgoing socket
	if ((src_sock = socket(ret->ai_family, ret->ai_socktype,
					ret->ai_protocol)) < 0) {
		fprintf(stderr, "Error creating host socket: %s\n", strerror(errno));
		return -1;
	}

	//create an incoming socket
	if ((recv_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP)) < 0){
		fprintf(stderr, "Error creating recv socket: %s\n", strerror(errno));
	}

	/*
	 * We go from hop to hop by incrementing the time to live in the IP header
	 * for each hop we visit until we reach the destination IP address (which we
	 * already have). Time to live decrements for every hop, so once it reaches
	 * zero we report the IP address of the node we are connected to.
	 */
	while (ttl < 10) {
		ttl++;
    if ((setsockopt(src_sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl))) < 0) {
      fprintf(stderr, "Error setting ttl: %s\n", strerror(errno));
      return -1;
    }

    if ((sendto(src_sock, msg, strlen(msg), 0, ret->ai_addr,
                ret->ai_addrlen)) == -1) {
      fprintf(stderr, "Error sending msg: %s\n", strerror(errno));
      return -1;
    }

    if ((recvfrom(recv_sock, icmp_msg, ICMP_MSG_LEN, 0,
                   (struct sockaddr*)&reply_addr, &reply_addr_len)) != -1) {
      inet_ntop(AF_INET, &(reply_addr.sin_addr), ipv4, INET_ADDRSTRLEN);
    	printf("hop: %d, address: %s\n", ttl, ipv4);
    } else {
    	fprintf(stderr, "Error receiving packet: %s\n", strerror(errno));
      return -1;
    }
	}

	return 0;
}
