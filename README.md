# Workspace
Embedded software development workspace for microcontrollers.

## Supported targets:
- LPC1769
- LPC4337 (M4 and M0 cores)

## Supported boards:
- CIAA-NXP and EDU-CIAA-NXP (www.proyecto-ciaa.com.ar)
- LPCXpresso with LPC1769

## Supported toolchains:
- gcc-arm-none-eabi

## Usage
* Make sure you have an ```arm-none-eabi-*``` toolchain configured in your ```PATH```.
* ```git clone https://github.com/pridolfi/workspace.git && cd workspace```
* ```cp project.mk.template project.mk```
* Define ```PROJECT```, ```TARGET``` and ```BOARD``` (optional) variables in ```project.mk``` according to the project you want to compile.
* Compile with ```make```.
* Clean with ```make clean```.
* Download to target via OpenOCD with ```make download```.
