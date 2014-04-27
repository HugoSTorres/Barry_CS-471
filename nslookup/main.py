import dns
import dns.name
import dns.query
import dns.resolver
import socket, time
from termcolor import colored
from setuptools.command import easy_install

class ImportsChecker():
    _MODULES = ('termcolor', 'dns', 'time', 'socket')
    def __init__(self):
        self.checkModules()
        print colored('[%s] [+] All necessary modules are installed' % time.strftime('%H:%M:%S'), 'green')
    def checkModules(self):
        for module in self._MODULES:
            try:
                __import__(module)
            except ImportError:
                print colored('[%s] %s module not found, trying to install it' % (time.strftime('%H:%M:%S'), module), 'yellow')
                self.installModule(module)
    def installModule(self, module):
        try:
            easy_install.main([module])
        except:
            print colored('''[%s] [-] Error installing %s module. Possibly setuptools module is not installed.
            Please visit https://pypi.python.org/pypi/setuptools to download setuptools, and try again.
            Exiting...''' % (time.strftime('%H:%M:%S'), module), 'red')
            exit()
        else:
            print colored('[%s] [+] %s module has been sucessfully installed' %(time.strftime('%H:%M:%S'), module), 'green')

class NSLookup():
    
    def __init__(self):
        self._resolver = dns.resolver.Resolver()  
        self._authoritative_list = []
  
    def _printServerInfo(self):
        # get nameserver, port and print it
        self._nameserver = my_resolver.nameservers[0]
        self._port = my_resolver.port
        print "Server: %s" % self._nameserver
        print "Address: %s#%s\n\n" % (self._nameserver, self._port)
        print "Non-authoritative answer:" 
    def _printAuthoritative(self):
        print "\nAuthoritative answers can be found from:"
        for item in self._authoritative_list:
            print item[0] + " \tinternet address: " + item[1] 

    def _printAInfo(self, domain):
        answer = my_resolver.query(domain_name, 'A')
        
        # index for substring
        idx = str(answer.response.question[0]).rindex('IN A')
        
        for item in answer.response.answer[0].items:
            print "Name: \t" + str(answer.response.question[0])[:idx -2]
            print "Address: \t" + item.address
            
    def _printMXInfo(self, domain):
        answer = my_resolver.query(domain, 'MX')
        # let's iterate through A's array and print
    
        for data in answer:
            print str(answer.canonical_name) + "\t mail exchanger: " + str(data)
            
            # extract the hostname of MX name server
            idx = str(data).index(' ')
            mx_name = str(data)[idx +1:len(str(data)) - 1]
            
            # append it to the list for later printing
            self._authoritative_list.append([mx_name, socket.gethostbyname(mx_name)])
            
    def _printNSInfo(self, domain):
        answer = my_resolver.query(domain, 'NS')
        # let's iterate through A's array and print
        
        for data in answer:
            print str(answer.canonical_name) + "\t nameserver: " + str(data)
            
            # extract the hostname of NS name server
            ns_name = str(data)[:len(str(data)) - 1]
            
            # append it to the list for later printing
            self._authoritative_list.append([ns_name, socket.gethostbyname(ns_name)])
    
    def _printSOAInfo(self, domain):
        answer = my_resolver.query(domain, 'SOA')
        # let's iterate through A's array and print
        
        info =  str(answer.response.answer[0]).split()
        
        print str(answer.canonical_name)
        
        origin = info[4][:len(info[4]) - 1]
        mail_addr = info[5][:len(info[5]) - 1]
        print "\t origin: " + origin # simulate how the actual NSLOOKUP SOA request looks like
        print "\t mail address: " + mail_addr
        print "\t serial number: " + info[6]
        print "\t refresh time: " + info[7]
        print "\t retry interval: " + info[8]
        print "\t expiration time: " + info[9]
        print "\t minimum time: " + info[10]
        
        self._authoritative_list.append([origin, socket.gethostbyname(origin)])
    
    def _printAAAAInfo(self, domain):
        answer = my_resolver.query(domain, 'AAAA')
        idx = str(answer.response.answer[0]).rindex(' ')
        
        print str(answer.canonical_name) + "\t has AAAA address" + str(answer.response.answer[0])[idx:]
    
    def _printCNAMEInfo(self, domain):
        answer = my_resolver.query(domain, 'CNAME')
        idx = str(answer.response.answer[0]).rindex(' ')
        print str(answer.canonical_name) + "\t canonical name:" + str(answer.response.answer[0])[idx:]
    def printOtherInfo(self, domain, type):
        answer = my_resolver.query(domain, type)
        print answer.response.answer[0]
    
    def _printANYInfo(self, domain):
        self.printSOAInfo(domain)
        self.printMXInfo(domain)
        self.printAAAAInfo(domain)
        self.printAInfo(domain)
        self.printNSInfo(domain)
        self.printCNAMEInfo(domain)
    
    def reverseDNSLookup(self,ip):
        addr = dns.reversename.from_address(ip)
        print addr 
    
    def setPort(self, port):
        self._port = port
    def setTimeout(self, timeout):
        self._timeout
my_resolver = dns.resolver.Resolver()
# setting the port:
#my_resolver.port = 

#setting timeout:
#my_resolver.timeout =

#printServerInfo()


domain_name = "google.com"
#printAInfo(domain_name)
#printMXInfo(domain_name)
#printNSInfo(domain_name)
#printSOAInfo(domain_name)
#printAAAAInfo(domain_name)
#printANYInfo(domain_name)
#printOtherInfo(domain_name, 'A')
#printCNAMEInfo(domain_name)
# exceptions to handle : NoAnswer, Timeout
#printAuthoritative()
#reverseDNSLookup("74.125.21.102")
