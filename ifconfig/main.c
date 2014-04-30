#include <arpa/inet.h>
#include <net/if.h> // contains the flags
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs/linkedList.h"
int
main(int argc, char *argv[])
{
    struct ifaddrs *ifaddr, *ifa;
    struct ifreq *ifr;
    int family, s;
    char host[NI_MAXHOST];
    char *family_str;
    unsigned int flags;
    linked_list_t *flag_list = malloc(sizeof(linked_list_t)); // list for flags

    // get the interfaces addresses
    if (getifaddrs(&ifaddr) == -1) {
        perror("Could not access the interfaces addresses. Exiting...");
        exit(1);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    	// make sure the address is not null
        if (ifa->ifa_addr != NULL){
        	// get the family
			family = ifa->ifa_addr->sa_family;

			family_str = (family == AF_INET) ? "AF_INET" : (family == AF_INET6) ?  "AF_INET6" : "";

			printf("%s	  \naddress family: %d %s\n",
								ifa->ifa_name, family,family_str);
			if (family == AF_INET || family == AF_INET6) {
				// get the regular address
				s = getnameinfo(ifa->ifa_addr,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s == 0) {
					printf("\tinet6 address: <%s>\n", host);
				}

				// get netmask addresss
				s = getnameinfo(ifa->ifa_netmask,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s == 0) {
					printf("\tnetmask address: <%s>\n", host);
				}
				// get broadcast address
				s = getnameinfo(ifa->ifa_addr,	(family == AF_INET) ? sizeof(struct sockaddr_in) :
											  sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s == 0) {
					printf("\tinet address: <%s>\n", host);
				}

				// assign flags
				flags = ifa->ifa_flags;

				// add all the flags to the list
				if(flags){
					if (flags & IFF_UP)
						addTop(flag_list, IFF_UP);
					if (flags & IFF_BROADCAST)
						addTop(flag_list, IFF_BROADCAST);
					if (flags & IFF_DEBUG)
						addTop(flag_list, IFF_DEBUG);
					if (flags & IFF_LOOPBACK)
						addTop(flag_list, IFF_LOOPBACK);
					if (flags & IFF_POINTOPOINT)
						addTop(flag_list, IFF_POINTOPOINT);
					if (flags & IFF_NOTRAILERS)
						addTop(flag_list, IFF_NOTRAILERS);
					if (flags & IFF_RUNNING)
						addTop(flag_list, IFF_RUNNING);
					if (flags & IFF_NOARP)
						addTop(flag_list, IFF_NOARP);
					if (flags & IFF_PROMISC)
						addTop(flag_list, IFF_PROMISC);
					if (flags & IFF_ALLMULTI)
						addTop(flag_list, IFF_ALLMULTI);
					if (flags & IFF_OACTIVE)
						addTop(flag_list, IFF_OACTIVE);
					if (flags & IFF_SIMPLEX)
						addTop(flag_list, IFF_SIMPLEX);
					if (flags & IFF_MULTICAST)
						addTop(flag_list, IFF_MULTICAST);
				}

			}


        }

    }

    // free the interfaces
    freeifaddrs(ifaddr);
    exit(0);
}
