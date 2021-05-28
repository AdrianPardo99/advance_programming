#!/usr/bin/env  ruby
require "socket"

if ARGV.length<1
  puts "Error...\nUsage #{$0} <Kernel_module>"
  exit(1)
end

if Process.uid!=0
  puts "Error uid #{Process.uid}"
  exit(1)
end

begin
  port_app=1313
  km=ARGV[0]
  s=TCPServer.open(port_app)
  puts "Iniciando servidor"
  loop{
    cliente=s.accept()
    boleta=cliente.gets.chomp
    if !boleta.empty?
      puts "Boleta a escribir en el modulo: #{boleta}"
      File.open("/sys/module/#{km}/parameters/boleta", "w") {|f|
        f.write "#{boleta}"
      }
      if !km.include?("_cb")
        File.open("/sys/module/#{km}/parameters/des", "w") {|f|
          f.write "1"
        }
      end
    end
  }
rescue Interrupt,Errno => e
  s.close()
  puts "Bye"
end
