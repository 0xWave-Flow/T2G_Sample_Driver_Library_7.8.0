- Wav Transmitting examples
Before executing "wav_tx" and "wav_tx_dw" example, we have to prepare sound resource.
Please open "SoundResource" folder, and drag and drop your .wav file onto "bin2text.bat". it will make data file "wav_16bit_data.inc"
which is included by the example code, into main src folder.

- Loopback examples
ADC seceives sound from microphone, transmits the received sound back to it's DAC.

How to set up each modeÅC please refer the comments in the source code.

To understand clock routing, please see "clk_routing_when_48kfs.pdf"

