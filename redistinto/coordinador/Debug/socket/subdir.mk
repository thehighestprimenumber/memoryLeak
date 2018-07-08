################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/common.c \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/logger_comun.c \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/socket.c \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/testCoordinador.c 

OBJS += \
./socket/common.o \
./socket/logger_comun.o \
./socket/operacion.o \
./socket/socket.o \
./socket/testCoordinador.o 

C_DEPS += \
./socket/common.d \
./socket/logger_comun.d \
./socket/operacion.d \
./socket/socket.d \
./socket/testCoordinador.d 


# Each subdirectory must supply rules for building sources it contributes
socket/common.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/common.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/logger_comun.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/logger_comun.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/operacion.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/socket.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/testCoordinador.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/testCoordinador.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


