import dns
import dns.name
import dns.query
import dns.resolver
import socket, time, getopt, signal, sys
from termcolor import colored
from setuptools.command import easy_install

'''
    The object basically ensures that all the required imports are available on the computer.
    If they are not, it attempts to install it. If it fails, the program exits
'''
class ImportsChecker():
    _MODULES = ('termcolor', 'dns', 'time', 'socket', 'getopt')
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
    
    def __init__(self, domain, reverse = False):
        self._resolver = dns.resolver.Resolver()  
        self._authoritative_list = []
        
        if (not reverse):
            self._domain = domain
        else:
            self._printServerInfo()
            self._reverseDNSLookup(domain)
  
    '''
        Prints the local DNS server information
    '''
    def _printServerInfo(self):
        # get nameserver, port and print it
        self._nameserver = self._resolver.nameservers[0]
        self._port = self._resolver.port
        print "Server: %s" % self._nameserver
        print "Address: %s#%s\n\n" % (self._nameserver, self._port)
        print "Non-authoritative answer:" 
        
    '''
        Prints the authoritative information for the remote host
    '''
    def _printAuthoritative(self):
        print "\nAuthoritative answers can be found from:"
        for item in self._authoritative_list:
            print item[0] + " \tinternet address: " + item[1] 

    '''
        Prints A type records
    '''
    def _printAInfo(self):
        answer = self._resolver.query(self._domain, 'A')
        
        # index for substring
        idx = str(answer.response.question[0]).rindex('IN A')
        
        for item in answer.response.answer[0].items:
            print "Name: \t" + str(answer.response.question[0])[:idx -2]
            print "Address: \t" + item.address
    
    '''
        Prints MX type records
    '''        
    def _printMXInfo(self):
        answer = self._resolver.query(self._domain, 'MX')
        # let's iterate through A's array and print
    
        for data in answer:
            print str(answer.canonical_name) + "\t mail exchanger: " + str(data)
            
            # extract the hostname of MX name server
            idx = str(data).index(' ')
            mx_name = str(data)[idx +1:len(str(data)) - 1]
            
            # append it to the list for later printing
            self._authoritative_list.append([mx_name, socket.gethostbyname(mx_name)])
    
    '''
        Prints NS type records
    '''        
    def _printNSInfo(self):
        answer = self._resolver.query(self._domain, 'NS')
        # let's iterate through A's array and print
        
        for data in answer:
            print str(answer.canonical_name) + "\t nameserver: " + str(data)
            
            # extract the hostname of NS name server
            ns_name = str(data)[:len(str(data)) - 1]
            
            # append it to the list for later printing
            self._authoritative_list.append([ns_name, socket.gethostbyname(ns_name)])
    
    '''
        Prints SOA type records
    '''
    def _printSOAInfo(self):
        answer = self._resolver.query(self._domain, 'SOA')
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
    
    '''
        Prints AAAA type records
    '''
    def _printAAAAInfo(self):
        answer = self._resolver.query(self._domain, 'AAAA')
        idx = str(answer.response.answer[0]).rindex(' ')
        
        print str(answer.canonical_name) + "\t has AAAA address" + str(answer.response.answer[0])[idx:]
    
    '''
        Prints CNAME type records
    '''
    def _printCNAMEInfo(self):
        answer = self._resolver.query(self._domain, 'CNAME')
        idx = str(answer.response.answer[0]).rindex(' ')
        print str(answer.canonical_name) + "\t canonical name:" + str(answer.response.answer[0])[idx:]
    '''
        Since it was hard to find websites that use order records (like SRV or PTR) to test it, I did not filter the results
        from DNS records for those type and just print them directly
    '''
    def _printOtherInfo(self):
        answer = self._resolver.query(self._domain, self._type)
        print answer.response.answer[0]
    '''
        Prints ANY type records
    '''
    def _printANYInfo(self):
        self.printSOAInfo()
        self.printMXInfo()
        self.printAAAAInfo()
        self.printAInfo()
        self.printNSInfo()
        self.printCNAMEInfo()
    
    '''
        Looks up the domain name from the IP address
    '''
    def _reverseDNSLookup(self,ip):
        addr = dns.reversename.from_address(ip)
        print addr 
    
    '''
        Sets the port through which to establish the communication
    '''
    def setPort(self, port):
        self._port = port
    '''
        Sets the timeout for a single try
    '''
    def setTimeout(self, timeout):
        self._timeout
        
    '''
        Sets what kind of request to perform and executes it
    '''
    def executeType(self, type):
        self._type = type
        
        self._printServerInfo()
        
        try:
            if type == 'A':
                self._printAInfo()
            elif type == 'CNAME':
                self._printCNAMEInfo()
            elif type == 'NS':
                self._printNSInfo()
            elif type == 'MX':
                self._printMXInfo()
            elif type == 'AAAA':
                self._printAAAAInfo()
            elif type == 'SOA':
                self._printSOAInfo()
            elif type == 'ANY':
                self._printANYInfo()
            else:
                self._printOtherInfo()
        except dns.resolver.NoAnswer:
            print colored("No answer has been received. Please try different type", 'red')
            exit()
        except dns.resolver.Timeout:
            print colored("Your request has timed out. Please try again", 'red')
        
        # at the end print authoritative response
        self._printAuthoritative()
