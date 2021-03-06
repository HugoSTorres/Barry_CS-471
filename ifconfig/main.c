#include <arpa/inet.h>
#include <net/if.h> // contains the flags
#include <net/if_dl.h>
#include <net/if_media.h>
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

// defines for convenience
#define IP_ADDR 0x01
#define P2P_ADDR 0x02
#define BCAST_ADDR 0x03
#define MASK_ADDR 0x04
#define AUTOCONF_ADDR 0x05
#define AUTOCONF_MASK 0x06
#define IPV4ALL_ADDR 0x07
#define LINK_LEVEL_ADDR 0x08
// assumption is no more than 16 network devices on a computer
// wasting space but quick solution
#define MAX_NUM_IFS 16

void printIPAddress(char *, int, int, struct ifreq);
void printFlags(unsigned int);
void printHeader(char *, void (*)(unsigned int), unsigned int);
void printCapabilities(unsigned int);
void displayComputerInfo();
void getInterfaceList(char *[]);
int isInterface(char *[], char *);
void manipulateIfState(char *, int);
void printInterfaceInfo(struct ifaddrs *);
void handleSingleInterface(char *, void(*)(struct ifaddrs *));

int
main(int argc, char *argv[]){
	char *interface_list[MAX_NUM_IFS];

	if (argc <= 1){
		displayComputerInfo();
	} else {
		if (!strcmp(argv[1], "help")){
			puts("Usage: ./ifconfig [options]\n"
				 "Options:\n\n"
				 "up [interface name]					Brings the interface up\n"
				 "down [interface name]					Puts the interface down\n"
				 "[interface name]						Displays information only for a given interface\n"
				 "help									Shows this screen\n"
				 "[no options]							Shows the computer network interface configurations\n");
			exit(0);
		}
		// gets the interface list
		getInterfaceList(interface_list);

		if (!strcmp(argv[1], "up")){
			// bring the interface up
			if (argv[2] != NULL && isInterface(interface_list, argv[2])){
				manipulateIfState(argv[2], 1);
				printf("The device %s has been brought up\n", argv[2]);
			} else {
				puts("No such device exists. Exiting...");
			}
		} else if (!strcmp(argv[1], "down")){
			// bring the interface down
			if (argv[2] != NULL && isInterface(interface_list, argv[2])){
				manipulateIfState(argv[2], 0);
				printf("The device %s has been brought down\n", argv[2]);
			} else {
				puts("No such device exists. Exiting...");
			}
		} else if(isInterface(interface_list, argv[1])){
			// display only the particular interface's network information
			// it's done via callback
			handleSingleInterface(argv[1], &printInterfaceInfo);
		} else{
			puts("Type in ./ifconfig help for usage.");
		}
	}
    exit(0);
}

/**
 * Prints the IP addresses, given the call method and name
 */
void printIPAddress(char *name, int method, int fd, struct ifreq ifr){
	int result;
	switch(method){
		case IP_ADDR:
			result = ioctl(fd,SIOCGIFADDR, &ifr);
			break;
		case P2P_ADDR:
			result = ioctl(fd,SIOCGIFDSTADDR, &ifr);
			break;
		case BCAST_ADDR:
			result = ioctl(fd, SIOCGIFBRDADDR, &ifr);
			break;
		case MASK_ADDR:
			result = ioctl(fd, SIOCGIFNETMASK, &ifr);
			break;
		case AUTOCONF_ADDR:
			result = ioctl(fd, SIOCAUTOADDR, &ifr);
			break;
		case AUTOCONF_MASK:
			result = ioctl(fd, SIOCAUTONETMASK, &ifr);
			break;
		case IPV4ALL_ADDR:
			result = ioctl(fd, SIOCARPIPLL, &ifr);
			break;
		case LINK_LEVEL_ADDR:
			result = ioctl(fd, SIOCSIFLLADDR, &ifr);
			break;
		default:
			return;
	}
	// check if not failed
	if (result==-1) {
		return;
	}

	struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
	char *ip = inet_ntoa(ipaddr->sin_addr);

	// no need to print 0's address
	if (strcmp(ip, "0.0.0.0"))
		printf("\t%s address: %s\n", name, inet_ntoa(ipaddr->sin_addr));
}

/**
 * Callback function to print the flags
 */
void printFlags(unsigned int flags){
	linked_list_t *flag_list = malloc(sizeof(linked_list_t)); // list for flags
	node_t* node;
	int comma_flag = 0; // flag for comma

	// add all the flags to the list
	if(flags){
		if (flags & IFF_UP)
			addBottom(flag_list, IFF_UP);
		if (flags & IFF_BROADCAST)
			addBottom(flag_list, IFF_BROADCAST);
		if (flags & IFF_DEBUG)
			addBottom(flag_list, IFF_DEBUG);
		if (flags & IFF_LOOPBACK)
			addBottom(flag_list, IFF_LOOPBACK);
		if (flags & IFF_POINTOPOINT)
			addBottom(flag_list, IFF_POINTOPOINT);
		if (flags & IFF_NOTRAILERS)
			addBottom(flag_list, IFF_NOTRAILERS);
		if (flags & IFF_RUNNING)
			addBottom(flag_list, IFF_RUNNING);
		if (flags & IFF_NOARP)
			addBottom(flag_list, IFF_NOARP);
		if (flags & IFF_PROMISC)
			addBottom(flag_list, IFF_PROMISC);
		if (flags & IFF_ALLMULTI)
			addBottom(flag_list, IFF_ALLMULTI);
		if (flags & IFF_OACTIVE)
			addBottom(flag_list, IFF_OACTIVE);
		if (flags & IFF_SIMPLEX)
			addBottom(flag_list, IFF_SIMPLEX);
		if (flags & IFF_MULTICAST)
			addBottom(flag_list, IFF_MULTICAST);
	}

	for(node = flag_list->first; node != NULL; node = node->next){
		// comma is not needed on the first element
		if(comma_flag){
			printf(", ");
		} else {
			comma_flag = 1;
		}
		switch(node->value){
			case IFF_UP:
				printf("UP");
				break;
			case IFF_BROADCAST:
				printf("BROADCAST");
				break;
			case IFF_DEBUG:
				printf("DEBUG");
				break;
			case IFF_LOOPBACK:
				printf("LOOPBACK");
				break;
			case IFF_POINTOPOINT:
				printf("POINT-TO-POINT");
				break;
			case IFF_NOTRAILERS:
				printf("NO TRAILERS");
				break;
			case IFF_RUNNING:
				printf("RUNNING");
				break;
			case IFF_NOARP:
				printf("NO ARP");
				break;
			case IFF_PROMISC:
				printf("PROMISCUOUS");
				break;
			case IFF_ALLMULTI:
				printf("ALL MULTICAST");
				break;
			case IFF_OACTIVE:
				printf("ACTIVE");
				break;
			case IFF_SIMPLEX:
				printf("SIMPLEX");
				break;
			case IFF_MULTICAST:
				printf("MULTICAST");
				break;
			default:
				break;
		}
	}
}

/**
 * Callback function to print the options/capabilities
 */
void printCapabilities(unsigned int capabilities){
	// capabilities = 0 => nothing to print
	if(!capabilities)
		return;

	int comma_flag = 0; // flag for comma
	node_t* node;
	linked_list_t *option_list = malloc(sizeof(linked_list_t));

	// handle all the capabilites and push it up the list
	if(capabilities & IFCAP_RXCSUM)
		addBottom(option_list, IFCAP_RXCSUM);
	if(capabilities & IFCAP_TXCSUM)
		addBottom(option_list, IFCAP_TXCSUM);
	if(capabilities & IFCAP_VLAN_MTU)
		addBottom(option_list, IFCAP_VLAN_MTU);
	if(capabilities & IFCAP_VLAN_HWTAGGING)
		addBottom(option_list, IFCAP_VLAN_HWTAGGING);
	if(capabilities & IFCAP_JUMBO_MTU)
		addBottom(option_list, IFCAP_JUMBO_MTU);
	if(capabilities & IFCAP_TSO4)
		addBottom(option_list, IFCAP_TSO4);
	if(capabilities & IFCAP_TSO6)
		addBottom(option_list, IFCAP_TSO6);
	if(capabilities & IFCAP_LRO)
		addBottom(option_list, IFCAP_LRO);
	if(capabilities & IFCAP_AV)
		addBottom(option_list, IFCAP_AV);
	if(capabilities & IFCAP_TXSTATUS)
		addBottom(option_list, IFCAP_TXSTATUS);

	// identical handling as in printFlags()
	for(node = option_list->first; node != NULL; node = node->next){
		// comma is not needed on the first element
		if(comma_flag){
			printf(", ");
		} else {
			comma_flag = 1;
		}
		switch(node->value){
			case IFCAP_RXCSUM:
				printf("RXCSUM");
				break;
			case IFCAP_TXCSUM:
				printf("TXCSUM");
				break;
			case IFCAP_VLAN_MTU:
				printf("VLAN MTU");
				break;
			case IFCAP_JUMBO_MTU:
				printf("JUMBO MTU");
				break;
			case IFCAP_TSO4:
				printf("TS04");
				break;
			case IFCAP_TSO6:
				printf("TS06");
				break;
			case IFCAP_LRO:
				printf("LR0");
				break;
			case IFCAP_AV:
				printf("AV BRIDGING");
				break;
			case IFCAP_TXSTATUS:
				printf("TXSTATUS");
				break;
			default:
				break;
		}
	}
}

/**
 * Tiny utility function to print the formating for headers and call the callback
 */
void printHeader(char* name, void (*callback)(unsigned int), unsigned int param){
	printf("\n\t%s:%u<", name, param);
	callback(param);
	printf("> \n");
}

/**
 * Displays all the network interfaces and their parameters
 */
void displayComputerInfo(){
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];
	char *family_str;
	unsigned int flags;
	char *current_interface = "none"; // holds the value for the current interface.

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

		printInterfaceInfo(ifa);

	}

	// free the interfaces
	freeifaddrs(ifaddr);
}
/**
 * Computes the list of the network interfaces on the computer
 */
void getInterfaceList(char *interfaces[]){
	char *current_if = "none";
	int count = 0;
	struct ifaddrs *ifaddr, *ifa;
	// get the interfaces addresses
		if (getifaddrs(&ifaddr) == -1) {
			perror("Could not access the interfaces addresses. Exiting...");
			exit(1);
		}

		for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
			if (strcmp(current_if, ifa->ifa_name) != 0){
				// strings not equal
				if(ifa->ifa_addr != NULL){
					current_if = ifa->ifa_name;
					interfaces[count++] = current_if;
				}

			}
		}
		freeifaddrs(ifaddr);
}

/**
 * Finds the interface, given its name, and calls the callback on it
 */
void handleSingleInterface(char *ifname, void(*callback)(struct ifaddrs *)){
	struct ifaddrs *ifaddr, *ifa;
	// get the interfaces addresses
	if (getifaddrs(&ifaddr) == -1) {
		perror("Could not access the interfaces addresses. Exiting...");
		exit(1);
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
		if (strcmp(ifname, ifa->ifa_name) == 0){
			// strings equal
			callback(ifa);
			// I need to break out, once the callback has been called, since there are more than one
			// ifa instances with the same name
			break;
		}
	}
	freeifaddrs(ifaddr);
}

/**
 * Checks whether the item belongs to the interface list
 */
int isInterface(char *interfaces[], char *interface){
	int i = 0;
	for(; i < MAX_NUM_IFS; i++){
		if (!strcmp(interfaces[i], interface))
			return 1;
	}
	return 0;
}

/**
 * Brings the device up or down, depending on the state parameter
 */
void manipulateIfState(char *ifname, int state /* 1 - up, 0 - down */){
	struct ifreq ifr;

	size_t if_name_len=strlen(ifname);
	if (if_name_len < sizeof(ifr.ifr_name)) {
		// set name for IFR
		memcpy(ifr.ifr_name,ifname,if_name_len);
		ifr.ifr_name[if_name_len]=0;
	} else {
		puts("Interface name is too long");
		exit(1);
	}
	int fd=socket(AF_INET,SOCK_DGRAM,0);


	if (fd < 0){
		puts("Error creating a socket.");
		exit(1);
	}
	// get the current flags
	if(ioctl(fd, SIOCGIFFLAGS, &ifr) == -1){
		puts("Error retrieving flags");
		exit(1);
	}
	// handle up or down
	if (state)
		ifr.ifr_flags |= IFF_UP;
	else
		ifr.ifr_flags = ifr.ifr_flags & ~IFF_UP;
	if(ioctl(fd, SIOCSIFFLAGS, &ifr) == -1){
		puts("Error setting flags");
		exit(1);
	}
	close(fd);
}

/**
 * Prints interface information for the interface ifa
 */
void printInterfaceInfo(struct ifaddrs *ifa){
	// create a puppet socket to act as a file descriptor for ioctl()
	// I can create it here, since I can reuse it
	int fd=socket(AF_INET,SOCK_DGRAM,0);

	// create the ifr & ifm
	// ifr is more general and more stuff can be retrieved, while
	// ifm is purely for media information
	struct ifreq ifr;
	struct ifmediareq ifm;

	size_t if_name_len=strlen(ifa->ifa_name);
	// ensure the interface name is not too long, since  ifr_name is fixed length buffer
	if (if_name_len < sizeof(ifr.ifr_name) && if_name_len < sizeof(ifm.ifm_name) ) {
		// set name for IFR
		memcpy(ifr.ifr_name, ifa->ifa_name, if_name_len);
		ifr.ifr_name[if_name_len]=0;

		// set name for IFM
		memcpy(ifm.ifm_name, ifa->ifa_name, if_name_len);
		ifm.ifm_name[if_name_len]=0;
	} else {
		puts("Interface name is too long");
		exit(1);
	}

	//output current interface
	printf("Interface: %s", ifa->ifa_name);

	if (fd==-1) {
		puts("Socket could not be created");
		exit(1);
	}

	// print flags
	printHeader("Flags", &printFlags, ifa->ifa_flags);
	// prints the MTU
	if (ioctl(fd, SIOCGIFMTU, &ifr) != -1)
			printf("\tMTU: %d", ifr.ifr_metric);

	// prints the options
	if (ioctl(fd, SIOCGIFCAP, &ifr) != -1)
		printHeader("Options", &printCapabilities, ifr.ifr_curcap);

	// prints the MAC address
	if(ifa->ifa_addr->sa_family == AF_LINK){
		struct sockaddr_dl *mac_addr = (struct sockaddr_dl *)ifa->ifa_addr;
		unsigned char mac[6];
		// check if he address length is 6 bytes
		if (6 == mac_addr->sdl_alen) {
		  memcpy(mac, LLADDR(mac_addr), mac_addr->sdl_alen);
		  printf("\tMAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}
	}

	// print status
	if (ioctl(fd, SIOCGIFMEDIA, &ifm) != -1)
				printf("\tStatus:%s\n", ifm.ifm_status == 3 ? "Active" : ifm.ifm_status == 1 ?
						"Inactive" : "Unknown"); // 3 is active and 1 is inactive

	/**
	 * I'm not going to handle media because it has way too many flags to implement
	 * in a timely manner
	 */

	// IP
	printIPAddress("IP", IP_ADDR, fd, ifr);

	// Point to point
	printIPAddress("Point-to-Point", P2P_ADDR, fd, ifr);

	// broadcast
	printIPAddress("Broadcast", BCAST_ADDR, fd, ifr);

	//netmask
	printIPAddress("Netmask", MASK_ADDR, fd, ifr);

	//autoconf
	printIPAddress("Autoconf", AUTOCONF_ADDR, fd, ifr);

	//autoconf mask
	printIPAddress("Autoconf mask", AUTOCONF_MASK, fd, ifr);

	//ipv4all
	printIPAddress("IPv4All", IPV4ALL_ADDR, fd, ifr);

	//link level
	printIPAddress("Link level", LINK_LEVEL_ADDR, fd, ifr);

	close(fd);
}
