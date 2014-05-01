#include <arpa/inet.h>
#include <net/if.h> // contains the flags
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs/linkedList.h"
#include <errno.h>
#include <netinet/in.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    char *family_str;
    unsigned int flags;
    linked_list_t *flag_list = malloc(sizeof(linked_list_t)); // list for flags
    char *current_interface = "none"; // holds the value for the current interface.
    char *if_name;

    /**
     * The way I use the flag is for detection whether ioctl request was successful
     * or failed.
     * If it was successful, continue. else - jump
     */
    unsigned char flag = 0; // the flag will be used for convenience


    // get the interfaces addresses
    if (getifaddrs(&ifaddr) == -1) {
        perror("Could not access the interfaces addresses. Exiting...");
        exit(1);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    	// make sure the address is not null
    	if (strcmp(current_interface, ifa->ifa_name) != 0){
    		// strings not equal
    		if(ifa->ifa_addr != NULL)
    			current_interface = ifa->ifa_name;
    	} else {
    		// strings equal => this interface already handled
    		continue;
    	}

    	// create the ifr
        struct ifreq ifr;
		size_t if_name_len=strlen(current_interface);
		// ensure the interface name is not too long, since  ifr_name is fixed length buffer
		if (if_name_len<sizeof(ifr.ifr_name)) {
			memcpy(ifr.ifr_name,current_interface,if_name_len);
			ifr.ifr_name[if_name_len]=0;
		} else {
			puts("Interface name is too long");
			exit(1);
		}

		// create a muppet socket to act as a file descriptor for ioctl()
		int fd=socket(AF_INET,SOCK_DGRAM,0);
		if (fd==-1) {
			puts("Socket could not be created");
			exit(1);
		}

		//
		if (ioctl(fd,SIOCGIFADDR,&ifr)==-1) {
			flag = 1;
		}
		close(fd);

		struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
		printf("IP address: %s\n",inet_ntoa(ipaddr->sin_addr));
//        if (ifa->ifa_addr != NULL){
//        	// get the family
//			family = ifa->ifa_addr->sa_family;
//
//			family_str = (family == AF_INET) ? "AF_INET" : (family == AF_INET6) ?  "AF_INET6" : "";
//
//			printf("%s	  \naddress family: %d %s\n",
//								ifa->ifa_name, family,family_str);
//			if (family == AF_INET || family == AF_INET6) {
//				// get the regular address
//				s = getnameinfo(ifa->ifa_addr,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
//											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
//				if (s == 0) {
//					printf("\tinet6 address: <%s>\n", host);
//				}
//
//				// get netmask addresss
//				s = getnameinfo(ifa->ifa_netmask,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
//											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
//				if (s == 0) {
//					printf("\tnetmask address: <%s>\n", host);
//				}
//				// get broadcast address
//				s = getnameinfo(ifa->ifa_addr,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
//											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
//				if (s == 0) {
//					printf("\tinet address: <%s>\n", host);
//				}
//
//				// assign flags
//				flags = ifa->ifa_flags;
//
//				// add all the flags to the list
//				if(flags){
//					if (flags & IFF_UP)
//						addTop(flag_list, IFF_UP);
//					if (flags & IFF_BROADCAST)
//						addTop(flag_list, IFF_BROADCAST);
//					if (flags & IFF_DEBUG)
//						addTop(flag_list, IFF_DEBUG);
//					if (flags & IFF_LOOPBACK)
//						addTop(flag_list, IFF_LOOPBACK);
//					if (flags & IFF_POINTOPOINT)
//						addTop(flag_list, IFF_POINTOPOINT);
//					if (flags & IFF_NOTRAILERS)
//						addTop(flag_list, IFF_NOTRAILERS);
//					if (flags & IFF_RUNNING)
//						addTop(flag_list, IFF_RUNNING);
//					if (flags & IFF_NOARP)
//						addTop(flag_list, IFF_NOARP);
//					if (flags & IFF_PROMISC)
//						addTop(flag_list, IFF_PROMISC);
//					if (flags & IFF_ALLMULTI)
//						addTop(flag_list, IFF_ALLMULTI);
//					if (flags & IFF_OACTIVE)
//						addTop(flag_list, IFF_OACTIVE);
//					if (flags & IFF_SIMPLEX)
//						addTop(flag_list, IFF_SIMPLEX);
//					if (flags & IFF_MULTICAST)
//						addTop(flag_list, IFF_MULTICAST);
//				}
//
//			}
//
//
//        }

    }

    // free the interfaces
    freeifaddrs(ifaddr);
    exit(0);
}
