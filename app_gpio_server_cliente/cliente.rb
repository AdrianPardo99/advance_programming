#!/usr/bin/env  ruby

require "socket"

if ARGV.length<3
  puts "Error...\nUsage #{$0} <HOST-or-IP> <PORT> <Boleta>"
  exit(1)
end

hostname=ARGV[0]
port_app=ARGV[1].to_i
boleta=ARGV[2]
s=TCPSocket.open(hostname,port_app)
begin
  s.puts("#{boleta}")
  s.close
rescue Interrupt => e
  s.close
end
