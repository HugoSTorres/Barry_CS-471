import dns
import dns.name
import dns.query
import dns.resolver
import socket, time, getopt, signal, sys
from termcolor import colored
from classes import ImportsChecker, NSLookup
usage = {'query': False, 'domain' : False, 'timeout' : False, 'port' : False, 'ip' : False}
def main(argv):
    global usage
    try:
        opts, args = getopt.getopt(argv,"hq:d:t:p:i:",
                                   ['query=', 'domain=', 'timeout=', 'port=', 'ip='])
    except getopt.GetoptError:
        print 'Type in \'python nslookup.py -h\' for help'
        exit()
    if not opts:
        print 'Type in \'python nslookup.py -h\' for help'
        exit()
    for opt, arg in opts:
        if opt == '-h':
            print '''usage: python nslookup.py [options]
            Options:
            
            -q type, --query=type                          Sets the type of the lookup (for example, A, MX, CNAME,
                                                           AAAA, ANY, NS, SOA). Default is A
            -d [domain name], --domain=[domain name]       Domain name (or IP address) for which to perform the NS lookup
            -t timeout, --timeout=timeout                  Change the initial timeout interval for waiting for a reply to number seconds
            -p port, --port=port                           Change the default TCP/UDP name server port value. Default is 53
            -i [IP Adress], --ip=[IP Address]              When chosen, the software will perform the reverse name lookup             
            -h                                             Shows this help menu
            '''
            sys.exit()
        elif opt in ('-q', '--query'):
            usage['query'] = arg.upper()
        elif opt in ('-d', '--domain'):
            usage['domain'] = arg
        elif opt in ('-t', '--timeout'):
            usage['timeout'] = arg
        elif opt in ('-p', '--port'):
            usage['port'] = arg
        elif opt in ('-i', '--ip'):
            usage['ip'] = arg
        
def cleanExit(signum, frm):
    print colored("Exiting the program", 'red')
    exit()
    
if __name__ == '__main__':
    main(sys.argv[1:])
    ch = ImportsChecker()

    signal.signal(signal.SIGINT, cleanExit)
    if usage['domain']:
        ns = NSLookup(usage['domain'])
        
        # set environment variables
        if usage['timeout']:
            ns.setTimeout(usage['timeout'])
        if usage['port']:
            ns.setPort(usage['port']) 
        
        #execute query
        if usage['query']:
            ns.executeType(usage['query'])
        else:
            ns.executeType('A')
    elif usage['ip'] :
        ns = NSLookup(usage['ip'], True)
    else:
        print colored('One of the arguments -d or -i must be supplied', 'red')
        exit()
