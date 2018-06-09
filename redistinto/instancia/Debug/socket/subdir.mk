################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/common.c \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/socket.c \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/testCoordinador.c 

OBJS += \
./socket/common.o \
./socket/operacion.o \
./socket/socket.o \
./socket/testCoordinador.o 

C_DEPS += \
./socket/common.d \
./socket/operacion.d \
./socket/socket.d \
./socket/testCoordinador.d 


# Each subdirectory must supply rules for building sources it contributes
socket/common.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/common.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/operacion.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/socket.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/testCoordinador.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/testCoordinador.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


