#include <arpa/inet.h>
#include <net/if.h> // contains the flags
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    struct ifaddrs *ifaddr, *ifa;
    struct ifreq *ifr;
    int family, s;
    char host[NI_MAXHOST];
    char *family_str;
    unsigned int flags;

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

				flags = ifa->ifa_flags;

				if(flags){
					printf("< ");
					if (flags & IFF_UP)
						printf("UP, ");
					if (flags & IFF_BROADCAST)
						printf("BROADCAST, ");
					if (flags & IFF_DEBUG)
						printf("DEBUG, ");
					if (flags & IFF_LOOPBACK)
						printf("LOOPBACK, ");
					if (flags & IFF_POINTOPOINT)
						printf("POINT-TO-POINT, ");
					if (flags & IFF_NOTRAILERS)
						printf("NO-TRAILERS, ");
					if (flags & IFF_RUNNING)
						printf("RUNNING, ");
					if (flags & IFF_NOARP)
						printf("NOARP, ");
					if (flags & IFF_PROMISC)
						printf("PROMISCUOUS, ");
					if (flags & IFF_ALLMULTI)
						printf("RECEIVE ALL MULTICAST, ");
					if (flags & IFF_OACTIVE)
						printf("ACTIVE, ");
					if (flags & IFF_SIMPLEX)
						printf("SIMPLEX, ");
					if (flags & IFF_MULTICAST)
						printf("MULTICAST, ");
					printf(">\n");
				}

			}

			printf("\n");

        }

    }

    // free the interfaces
    freeifaddrs(ifaddr);
    exit(EXIT_SUCCESS);
}
