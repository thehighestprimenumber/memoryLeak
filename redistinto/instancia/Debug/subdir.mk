################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../circular.c \
../compactador.c \
../configuracionInstancia.c \
../handlerCoordinador.c \
../instancia.c 

OBJS += \
./circular.o \
./compactador.o \
./configuracionInstancia.o \
./handlerCoordinador.o \
./instancia.o 

C_DEPS += \
./circular.d \
./compactador.d \
./configuracionInstancia.d \
./handlerCoordinador.d \
./instancia.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


