################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../configuracionPlanificador.c \
../consola.c \
../logger_planificador.c \
../planificador.c 

OBJS += \
./configuracionPlanificador.o \
./consola.o \
./logger_planificador.o \
./planificador.o 

C_DEPS += \
./configuracionPlanificador.d \
./consola.d \
./logger_planificador.d \
./planificador.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


