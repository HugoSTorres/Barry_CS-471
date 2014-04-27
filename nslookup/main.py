import dns
import dns.name
import dns.query
import dns.resolver

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
        print "Name: " + str(answer.response.question[0])[:idx -2]
        print "Address: " + item.address
        
def printMXInfo(domain):
    answers = my_resolver.query('google.com', 'MX')
    for rdata in answers:
        print 'Host', rdata.exchange, 'has preference', rdata.preference
my_resolver = dns.resolver.Resolver()
printServerInfo()

domain_name = "www.google.com"
#printAInfo(domain_name)
printMXInfo(domain_name)