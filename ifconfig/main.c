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
    unsigned int test;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;


        printf("%s	  address family: %d%s\n",
                ifa->ifa_name, family,
                (family == AF_INET) ?   " (AF_INET)" :
                (family == AF_INET6) ?  " (AF_INET6)" : "");

        if (family == AF_INET || family == AF_INET6) {
            s = getnameinfo(ifa->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                          sizeof(struct sockaddr_in6),
                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                //exit(EXIT_FAILURE);
            } else{
            	printf("\taddress: <%s>\n", host);
            }
        }

        test = ifa->ifa_flags;
                            if (test & IFF_RUNNING){
                            	printf("labas");
                            }

        if (family == AF_INET || family == AF_INET6) {
                    s = getnameinfo(ifa->ifa_netmask,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                  sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                    if (s != 0) {
                        printf("getnameinfo() failed: %s\n", gai_strerror(s));
                        //exit(EXIT_FAILURE);
                    }else{
                    	printf("\taddress: <%s>\n", host);
                    }
                }
        if (family == AF_INET || family == AF_INET6) {
                            s = getnameinfo(ifa->ifa_dstaddr,
                                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                                          sizeof(struct sockaddr_in6),
                                    host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
                            if (s != 0) {
                                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                                //exit(EXIT_FAILURE);
                            }else{
                            	printf("\taddress: <%s>\n", host);
                            }
                        }

    }
    freeifaddrs(ifaddr);
    exit(EXIT_SUCCESS);
}
