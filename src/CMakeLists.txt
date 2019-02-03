add_definitions(-DLOGGING_ENABLED)

add_library (nes
                Nes.cpp
                Cpu.cpp
                Mmu.cpp
                Cartridge.cpp
            )
