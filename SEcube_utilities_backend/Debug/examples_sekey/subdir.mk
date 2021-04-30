################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../examples_sekey/sekey_example.cpp 

OBJS += \
./examples_sekey/sekey_example.o 

CPP_DEPS += \
./examples_sekey/sekey_example.d 


# Each subdirectory must supply rules for building sources it contributes
examples_sekey/%.o: ../examples_sekey/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


