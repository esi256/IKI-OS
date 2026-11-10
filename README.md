# IKI-OS

A small educational RTOS kernel for ARM Cortex-M4 (STM32F4), written in C and ARM assembly. Runs on real hardware or under QEMU.

> **Status:** work in progress. APIs and internals may change.

## Features

- Preemptive, priority-based scheduler with round-robin among equal priorities
- Context switching via PendSV, with SysTick as the system tick
- `sleep()` with a time-ordered blocked list
- System calls through SVC (privileged kernel / unprivileged user processes)
- MPU-based memory protection (per-process stack regions, guard regions)
- Separate kernel and user heaps (simple bump allocators)
- UART (USART1) console output via `write()`
- Central kernel error codes and error handler

## Project layout

```
arch/       hardware-specific code (MPU, SysTick, USART, CMSIS)
src/        kernel core (scheduler, memory, syscalls, startup)
include/    kernel headers
lib/        low-level helpers (bit operations)
user_procs/ user processes and the start() entry point
```

## Getting started

### Prerequisites

- `arm-none-eabi-gcc` (GNU Arm Embedded Toolchain)
- `qemu-system-arm` (for running without hardware)
- CMSIS headers placed in `arch/cmsis`

On Debian/Ubuntu:

```sh
sudo apt install gcc-arm-none-eabi qemu-system-arm
```

### Build and run

```sh
sh 1-gcc.sh        # builds a.out
sh 2-qemu_run.sh   # runs it on QEMU (olimex-stm32-h405)
```

Exit QEMU with `Ctrl+A`, then `X`.

### Writing your own process

Add a function to `user_procs/proc1.c` and register it in `start()`:

```c
setting.func = my_task;
setting.priority = 1;
struct process *p = init_user_proc(&setting);
add_proc_to_global_list(p);
```

## Roadmap

- [ ] Per-process MPU reconfiguration on context switch
- [ ] Validate pointers passed to system calls
- [ ] MemManage / HardFault handlers
- [ ] Freeing of processes, stacks and list nodes
- [ ] Interrupt-driven UART
- [ ] Makefile instead of shell scripts