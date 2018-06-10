################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Instancia/circular.c \
../Instancia/configuracionInstancia.c \
../Instancia/handlerCoordinador.c \
../Instancia/instancia.c 

OBJS += \
./Instancia/circular.o \
./Instancia/configuracionInstancia.o \
./Instancia/handlerCoordinador.o \
./Instancia/instancia.o 

C_DEPS += \
./Instancia/circular.d \
./Instancia/configuracionInstancia.d \
./Instancia/handlerCoordinador.d \
./Instancia/instancia.d 


# Each subdirectory must supply rules for building sources it contributes
Instancia/%.o: ../Instancia/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


