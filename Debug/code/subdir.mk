################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/CSL.c \
../code/Timer.c \
../code/Uart.c \
../code/app_init.c 

OBJS += \
./code/CSL.o \
./code/Timer.o \
./code/Uart.o \
./code/app_init.o 

C_DEPS += \
./code/CSL.d \
./code/Timer.d \
./code/Uart.d \
./code/app_init.d 


# Each subdirectory must supply rules for building sources it contributes
code/%.o: ../code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -pedantic  -g3 -DRSL10_CID=101 -I"C:\Program Files (x86)\ON Semiconductor\RSL10 SDK\eclipse\..\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


