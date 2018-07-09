################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../conexiones.c \
../configuracion.c \
../coordinador.c \
../logger_coordinador.c \
../tabla_instancias.c 

OBJS += \
./conexiones.o \
./configuracion.o \
./coordinador.o \
./logger_coordinador.o \
./tabla_instancias.o 

C_DEPS += \
./conexiones.d \
./configuracion.d \
./coordinador.d \
./logger_coordinador.d \
./tabla_instancias.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2018-1c-Memory-leak/redistinto/socket" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


