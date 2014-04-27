import dns
import dns.name
import dns.query
import dns.resolver
import socket

authoritative_list = []

def printAuthoritative():
    print "\nAuthoritative answers can be found from:"
    for item in authoritative_list:
        print item[0] + " \tinternet address: " + item[1]
def printServerInfo():
    # get nameserver, port and print it
    nameserver = my_resolver.nameservers[0]
    port = my_resolver.port
    print "Server: %s" % nameserver
    print "Address: %s#%s\n\n" % (nameserver, port)

def printAInfo(domain):
    answer = my_resolver.query(domain_name, 'A')
    
    # index for substring
    idx = str(answer.response.question[0]).rindex('IN A')
    
    for item in answer.response.answer[0].items:
        print "Name: \t" + str(answer.response.question[0])[:idx -2]
        print "Address: \t" + item.address
        
def printMXInfo(domain):
    answer = my_resolver.query(domain, 'MX')
    # let's iterate through A's array and print

    for data in answer:
        print answer.canonical_name[0] + '.' + answer.canonical_name[1] + "\t mail exchanger: " + str(data)
        
        # extract the hostname of MX name server
        idx = str(data).index(' ')
        mx_name = str(data)[idx +1:len(str(data)) - 1]
        
        # append it to the list for later printing
        authoritative_list.append([mx_name, socket.gethostbyname(mx_name)])
        
def printNSInfo(domain):
    answer = my_resolver.query(domain, 'NS')
    # let's iterate through A's array and print
    
    for data in answer:
        print answer.canonical_name[0] + '.' + answer.canonical_name[1] + "\t nameserver: " + str(data)
        
        # extract the hostname of NS name server
        ns_name = str(data)[:len(str(data)) - 1]
        
        # append it to the list for later printing
        authoritative_list.append([ns_name, socket.gethostbyname(ns_name)])

def printSOAInfo(domain):
    answer = my_resolver.query(domain, 'SOA')
    # let's iterate through A's array and print
    
    info =  str(answer.response.answer[0]).split()
    
    print answer.canonical_name[0] + '.' + answer.canonical_name[1]
    
    origin = info[4][:len(info[4]) - 1]
    mail_addr = info[5][:len(info[5]) - 1]
    print "\t origin: " + origin # simulate how the actual NSLOOKUP SOA request looks like
    print "\t mail address: " + mail_addr
    print "\t serial number: " + info[6]
    print "\t refresh time: " + info[7]
    print "\t retry interval: " + info[8]
    print "\t expiration time: " + info[9]
    print "\t minimum time: " + info[10]
    
    authoritative_list.append([origin, socket.gethostbyname(origin)])

def printAAAAInfo(domain):
    answer = my_resolver.query(domain, 'AAAA')
    idx = str(answer.response.answer[0]).rindex(' ')
    
    print answer.canonical_name[0] + '.' + answer.canonical_name[1] + "\t has AAAA address" + str(answer.response.answer[0])[idx:]

def printANYInfo(domain):
    printSOAInfo(domain)
    printMXInfo(domain)
    printAAAAInfo(domain)
    printAInfo(domain)
    printNSInfo(domain)
    
my_resolver = dns.resolver.Resolver()
printServerInfo()

domain_name = "google.com"
print "Non-authoritative answer:"
#printAInfo(domain_name)
#printMXInfo(domain_name)
#printNSInfo(domain_name)
#printSOAInfo(domain_name)
#printAAAAInfo(domain_name)
printANYInfo(domain_name)
# exceptions to handle : NoAnswer, Timeout
printAuthoritative()