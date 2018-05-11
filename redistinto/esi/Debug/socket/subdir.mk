################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c \
/home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/socket.c 

OBJS += \
./socket/operacion.o \
./socket/socket.o 

C_DEPS += \
./socket/operacion.d \
./socket/socket.d 


# Each subdirectory must supply rules for building sources it contributes
socket/operacion.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/operacion.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

socket/socket.o: /home/utnso/workspace/tp-2018-1c-Memory-leak/redistinto/socket/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


