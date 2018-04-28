################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/socket.c 

OBJS += \
./socket/socket.o 

C_DEPS += \
./socket/socket.d 


# Each subdirectory must supply rules for building sources it contributes
socket/socket.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/socket/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


