################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/tp-2018-1c-Memory-leak/redistinto/parsi/src/parsi/parser.c 

OBJS += \
./parsi/parser.o 

C_DEPS += \
./parsi/parser.d 


# Each subdirectory must supply rules for building sources it contributes
parsi/parser.o: /home/utnso/tp-2018-1c-Memory-leak/redistinto/parsi/src/parsi/parser.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


