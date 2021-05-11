# Compilación cruzada
Es la posibilidad de poder compilar en una arquitectura de cómputo, pero que a diferencia de compilación normal, esta se realiza con el fin de ejecutar en otra arquitectura de cómputo, sea el claro ejemplo compilar en una maquina X64\_86 hacia arquitectura ARM

Para esto en nuestro sistema operativo instalaremos los siguientes paquetes que tienen equivalencia entre distintas distros, listando los paquetes de las distribuciones basadas en Debian y en RedHat

__Debian__

* git
* bc
* libncurses5-dev
* flex
* bison
* openssl
* libssl-dev
* dkms
* make
* libelf-dev
* libudev-dev
* libpci-dev
* libiberty-dev
* autoconf

Que son todos los paquetes que gracias al empaquetado apt puedes instalar rápidamente, pero para el caso de otro sistema como el de RedHat es necesario buscar su equivalente los cuales son:

__RedHat__

* git
* bc
* libncurses5-devel
* flex
* bison
* openssl-devel
* dkms
* make
* elfutils-devel
* libgudev-devel
* pciutils-libs
* pciutils-devel
* binutils-devel

Después de instalar estos paquetes clonamos dos repositorios para instalar todo lo necesario en nuestra terminal y poder hacer cross compilation hacia la arquitectura ARM

Estos son:
* toolchain para ARM
* Kernel de ARM

Con esto solo escribiremos lo siguiente:
```bash
  # Toolchain para raspberry
  git clone https://github.com/raspberrypi/tools ~/tools
  # Kernel para raspberry
  git clone --depth=1 https://github.com/raspberrypi/linux ~/linux
```

Una vez clonando esto procederemos a agregar al Path los binarios de arm y que estos puedan compilar primero el gcc en arquitectura ARM, por ello escribiremos lo siguiente:
```bash
  # Agregamos a la variable PATH los binarios de arm
  echo "export PATH=\$PATH:~/tools/arm-bcm2708/arm-linux-gnueabihf/bin" >> ~/.bashrc
  # Damos un update al .bashrc
  source ~/.bashrc
```

Para compilar programas desarrollados en C es solo necesario hacer lo siguiente:
```bash
  # Parecido a gcc solo es necesario cambiar los makefile
  #  o la compilación manual con la llamada al binario
  arm-linux-gnueabihf-gcc
```
Ahora pasando del lado de cargando el kernel de ARM para raspberry es necesario pensar en que cambia la compilación según el modelo de la placa, por ello se procedera a mostrar lo siguiente:
```bash
  # Para todos los modelos:
  cd ~/linux
```
__Raspberry Pi Zero, Pi 1, MC__
```bash
  KERNEL=kernel
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig
```
__Raspberry Pi 2, Pi 3, Pi 3B, Pi 3B+, MC3__
```bash
  KERNEL=kernel7
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
```
__Raspberry Pi 4B__
```bash
  KERNEL=kernel7l
  make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig
```

Una vez especificando esto es necesario construir el kernel con el que vamos a trabajar y desarrollar, una vez seleccionando el modelo de placa de desarrollo es necesario proceder a escribir lo siguiente, para compilar el kernel y hacer uso de todo el procesador de nuestro equipo:

* Para esto debemos tomar en cuenta cuantos núcleos tenemos en hardware, y con esto procedemos a multiplicar comúnmente el número de procesadores por 1.5 de modo en que ese es el parámetro N.
* Ahora compilando de manera paralela para ahorrar tiempo, se utiliza el parametro -jN
```bash
  # Construyendo el kernel con 4 nucleos:
  make -j6 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs
```

Con esto finalmente agregaremos el kernel construido a una variable de entorno permanente la cual en el makefile de construcción de la arquitectura solo utilizara la ruta donde esta este kernel, y el ensamblador de ARM.
```bash
  # Creando la variable de entorno para compilación cruzada
  echo "export ARM_KERNEL=\"\$HOME/linux\"" >> ~/.bashrc
  source ~/.bashrc
```

Para finalmente tener el archivo Makefile de la siguiente manera:
```Makefile
  obj-m += <archivo(s)_aqui>.o
  all:
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) modules
  clean:
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(ARM_KERNEL) M=$(PWD) clean
```

Ahora solo falta actualizar el kernel de la tarjeta de desarrollo para poder insertar los modulos de kernel (En muchos casos es necesario hacerlo), para ello conectaremos la MicroSD y teclearemos lo siguiente:
```bash
  cd ~/linux
  mkdir -p mnt/boot mnt/rootfs
  # Listamos los dispositivos
  lsblk
  # Para esto verificaremos en donde se encuentran los dispositivos que esten
  #  montados cuya terminación sea /yyyyyy/xxxxxxx/boot y /yyyyyy/xxxxxxx/rootfs
  #  para ello desmontaremos las unidades cuyo nombre esta dada por
  #  /dev/xxxxxxxx1 y /dev/xxxxxxxx2
  # Aclaración fijarse en que unidades esta montado los x's son solo de ejemplo
  sudo umount /dev/xxxxxxxx1
  sudo umount /dev/xxxxxxxx2
  # Procedemos a montar xxxxxxxx1 pensando que es la partición que decía boot y
  #  la otra pensando en que la partición es rootfs, en caso contrario solo
  #  fijarse
  sudo mount /dev/xxxxxxxx1 mnt/boot
  sudo mount /dev/xxxxxxxx2 mnt/rootfs
  # Procedemos a escribir lo siguiente:
  sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
  INSTALL_MOD_PATH=mnt/rootfs modules_install
  # Ojo aquí se usa el \ solo para cuestiones de espacio de escritura en caso de
  #  desearlo eliminarlo y poner todo el comando anterior en una sola línea

  # Procedemos a escribir lo siguiente o pasarlo a un script de bash y que este
  # haga todo.
  sudo cp mnt/boot/$KERNEL.img mnt/boot/$KERNEL-backup.img
  sudo cp arch/arm/boot/zImage mnt/boot/$KERNEL.img
  sudo cp arch/arm/boot/dts/*.dtb mnt/boot/
  sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/boot/overlays/
  sudo cp arch/arm/boot/dts/overlays/README mnt/boot/overlays/
  sudo umount mnt/boot
  sudo umount mnt/rootfs
```
Y listo con todo esto, ya se puede trabajar con la compilación cruzada, de modo en que es mucho más sencillo compilar los modulos o programas desde un host de escritorio y solo enviar los archivos binarios o .ko a la raspberry para trabajar.

Para el caso de querer hacer todo en un script:

__Script para gcc de ARM y todos los archivos__
```bash
  #!/usr/bin/env bash
  # Descargar todo lo que necesitamos
  # Toolchain para raspberry
  git clone https://github.com/raspberrypi/tools ~/tools
  # Kernel para raspberry
  git clone --depth=1 https://github.com/raspberrypi/linux ~/linux
  echo "export ARM_KERNEL=\"\$HOME/linux\"" >> ~/.bashrc
  source ~/.bashrc
```
__Script para construcción de kernel de ARM__
```bash
  #!/usr/bin/env bash
  # Para ejecutar en directorio linux y compilar todo el kernel
  # Descomentar el modelo de placa a trabajar
  # Raspberry Pi Zero, Pi 1, MC
  # KERNEL=kernel
  # make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig

  # Raspberry Pi 2, Pi 3, Pi 3B, Pi 3B+, MC3
  # KERNEL=kernel7
  # make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig

  # Raspberry Pi 4
  # KERNEL=kernel7l
  # make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2711_defconfig

  # Cambiar el -j6 de acuerdo a las necesidades
  make -j6 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs

  # Creando la variable de entorno para compilación cruzada
  echo "export ARM_KERNEL=\"\$HOME/linux\"" >> ~/.bashrc
  source ~/.bashrc
```
__Script para copia de kernel en Raspberry__
```bash
  #!/usr/bin/env bash
  # Antes de ejecutar remplazar las rutas del lsblk de este script de acuerdo
  #  a la dirección de la unidad de la MicroSD
  # Verificar que la variable KERNEL siga persistente en caso contrario solo
  #  crearla aquí de acuerdo a la tarjeta de desarrollo
  # Ejecutar en la carpeta ~/linux
  mkdir -p mnt/boot mnt/rootfs
  # xxxxxxxx1 es equivalente a la partición boot
  # xxxxxxxx2 es equivalente a la partición rootfs
  sudo umount /dev/xxxxxxxx1
  sudo umount /dev/xxxxxxxx2

  sudo mount /dev/xxxxxxxx1 mnt/boot
  sudo mount /dev/xxxxxxxx2 mnt/rootfs

  sudo env PATH=$PATH make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- \
  INSTALL_MOD_PATH=mnt/rootfs modules_install
  sudo cp mnt/boot/$KERNEL.img mnt/boot/$KERNEL-backup.img
  sudo cp arch/arm/boot/zImage mnt/boot/$KERNEL.img
  sudo cp arch/arm/boot/dts/*.dtb mnt/boot/
  sudo cp arch/arm/boot/dts/overlays/*.dtb* mnt/boot/overlays/
  sudo cp arch/arm/boot/dts/overlays/README mnt/boot/overlays/
  sudo umount mnt/boot
  sudo umount mnt/rootfs
```
