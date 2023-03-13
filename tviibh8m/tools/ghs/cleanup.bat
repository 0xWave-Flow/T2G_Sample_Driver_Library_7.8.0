
for %%m in (flash sram libsdl libmw librtos libethstack) do (
    for %%c in (cm0plus cm4 cm7 cm7_0 cm7_1) do (
        
        rmdir /Q /S %%m\%%c\objs
        del   /Q    %%m\%%c\*.elf
        del   /Q    %%m\%%c\*.dep
        del   /Q    %%m\%%c\*.dla
        del   /Q    %%m\%%c\*.dnm
        del   /Q    %%m\%%c\*.map
        del   /Q    %%m\%%c\*.srec
        del   /Q    %%m\%%c\*.a
        del   /Q    %%m\%%c\*.dba        
        
    )
)


for %%m in (libcrypto) do (
    for %%t in (client server) do (
        for %%c in (cm0plus cm4 cm7 cm7_0 cm7_1) do (
        
            rmdir /Q /S %%m\%%t\%%c\objs
        )
    )
)

