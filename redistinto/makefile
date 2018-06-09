
planificador: planificador/consola.c planificador/logger_planificador.c planificador/planificador.c   ./socket/operacion.c ./socket/socket.c ./socket/common.c
	gcc -o planificador planificador/consola.c planificador/logger_planificador.c planificador/planificador.c   ./socket/operacion.c ./socket/socket.c ./socket/common.c -I"./socket" -I"./" -L/usr/lib -lcommons -lpthread -lreadline

instancia: instanica/Instancia/circular.c  instanica/Instancia/configuracionInstancia.c  instanica/Instancia/handlerCoordinador.c  instanica/Instancia/instancia.c /socket/operacion.c /socket/socket.c /socket/common.c
	gcc -o instancia instanica/Instancia/circular.c  instanica/Instancia/configuracionInstancia.c  instanica/Instancia/handlerCoordinador.c  instanica/Instancia/instancia.c /socket/operacion.c /socket/socket.c /socket/common.c -I"./socket" -I"./" -L/usr/lib -lcommons -lpthread -lreadline

esi: esi esi/esi.c /parsi/src/parsi/parser.c /socket/operacion.c /socket/socket.c /socket/common.c 
	gcc -o esi esi/esi.c /parsi/src/parsi/parser.c /socket/operacion.c /socket/socket.c /socket/common.c -I"./socket" -I"./" -L/usr/lib -lcommons -lpthread -lreadline

