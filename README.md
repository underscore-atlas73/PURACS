# PURACS
	The Personal Use Resource Adaptive Computer System.
A hobby operating system turned passion-project. PURACS is an operating system directed towards adaptability and "leaving no one behind" (backwards compatability with resource and device usage). It is (currently) developed by one person.
  
This project will never use AI "code."
  
### Working Name Clarification
PURACS may be referred to as System73, Sys73, SystemLXXIII, or LynXX(III) in some locations. These are all synonyms for PURACS created due to my own indecision.

# Roadmap
   
- [x] A name
   
- [x] Cross-Compiler Setup
- [x] x86_32 to x86_64 Bootstrapper
- [x] Legacy VGA text-mode Display Driver
- [x] Legacy 8259 PIC Driver
- [X] Legacy PIT Sysclock
- [X] Legacy PS/2 Keyboard Driver (Basic functionality finished)
- [ ] RTC Driver
- [ ] Dynamic Memory Allocation (Heap)
- TODO

- [SCRATCH FILE (MORE FREQUENTLY UPDATED)](.org/main.org) (some formatting may be broken due to Github stripping Emacs Org files)
   
# Planned Features
 
- Processes
- Syscalls (custom calling convention)
- Basic ACPI Parsing
- PCI Parsing
- AHCI Driver
- FAT32 driver
- Transition from Legacy VGA Text-Mode to linear framebuffer
- Optional Ring 3/Userspace Processes (By default, all threads will be in kernelspace)
   
- Custom ABI
- Custom Executable/Linkable Format
- USB/3.2 Driver (xHCI)
- PCIe Parsing
   
- POSIX emulation
- Linux subsystem
- GPU Driver porting
   
- End goal: Support for processors as far back as the Intel i386 DX (On "modern" platforms; e.g. at minimum a PS/2 keyboard)
