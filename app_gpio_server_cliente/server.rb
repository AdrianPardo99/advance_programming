#!/usr/bin/env  ruby
require "socket"

if Process.uid!=0
  puts "Error uid #{Process.uid}"
  exit(1)
end

begin
  port_app=1313
  s=TCPServer.open(port_app)
  puts "Iniciando servidor"
  loop{
    cliente=s.accept()
    boleta=cliente.gets.chomp
    if !boleta.empty?
      puts "Boleta a escribir en el modulo: #{boleta}"
      File.open("/sys/module/gpio_module/parameters/boleta", "w") {|f|
        f.write "#{boleta}"
      }
      File.open("/sys/module/gpio_module/parameters/des", "w") {|f|
        f.write "1"
      }
    end
  }
rescue Interrupt,Errno => e
  s.close()
  puts "Bye"
end
