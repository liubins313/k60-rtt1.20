# bsp name
BSP = 'k60dn512'

# RT-Thread root directory
RTT_ROOT = r'C:\Users\dell\Desktop\rt-thread-1.2.1-k60.git\branches\rt-thread-1.2.1-k60'
BSP_ROOT = r'C:\Users\dell\Desktop\rt-thread-1.2.1-k60.git\branches\rt-thread-1.2.1-k60\bsp\kinetis_k60'

# toolchains
#EXEC_PATH = r'C:\Program Files (x86)\CodeSourcery\Sourcery G++ Lite\bin'
EXEC_PATH = r'C:\Program Files (x86)\GNU Tools ARM Embedded\4.8 2014q2\bin'

PLATFORM = 'gcc'
PREFIX = 'arm-none-eabi-'
CC = PREFIX + 'gcc'
CXX = PREFIX + 'g++'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'mo'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

DEVICE = ' -mcpu=cortex-m4'
CFLAGS = DEVICE + ' -mthumb -mlong-calls -O0 -fPIC -fno-exceptions'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
LFLAGS = DEVICE + ' -mthumb -Wl,-z,max-page-size=0x4 -shared -fPIC -e main -nostdlib'

CPATH = ''
LPATH = ''
