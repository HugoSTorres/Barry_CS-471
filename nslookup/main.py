import dns
import dns.name
import dns.query
import dns.resolver

my_resolver = dns.resolver.Resolver()
nameserver = my_resolver.nameservers
port = my_resolver.port