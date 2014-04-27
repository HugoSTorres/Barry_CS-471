import dns
import dns.name
import dns.query
import dns.resolver
import socket

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
    
    # let's iterate through A's array and print
    print "Non-authoritative answer:"
    for item in answer.response.answer[0].items:
        print "Name: \t" + str(answer.response.question[0])[:idx -2]
        print "Address: \t" + item.address
        
def printMXInfo(domain):
    answer = my_resolver.query(domain, 'MX')
    # let's iterate through A's array and print
    authoritative_list = []
    
    # print the non authoritative answer
    print "Non-authoritative answer:"
    for data in answer:
        print answer.canonical_name[0] + '.' + answer.canonical_name[1] + "\t mail exchanger: " + str(data)
        
        # extract the hostname of MX name server
        idx = str(data).index(' ')
        mx_name = str(data)[idx +1:len(str(data)) - 1]
        
        # append it to the list for later printing
        authoritative_list.append([mx_name, socket.gethostbyname(mx_name)])
    print "\nAuthoritative answers can be found from:"
    for item in authoritative_list:
        print item[0] + " \tinternet address: " + item[1]
        
def printNSInfo(domain):
    answer = my_resolver.query(domain, 'NS')
my_resolver = dns.resolver.Resolver()
printServerInfo()

domain_name = "google.com"
print socket.gethostbyname("alt2.aspmx.l.google.com")
#printAInfo(domain_name)
printMXInfo(domain_name)

# exceptions to handle : NoAnswer, Timeout