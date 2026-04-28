# Logic Analyzer - PulseView

By combining pico-jxgLABO and PulseView, you can observe the internal signals of the Pico board with a logic analyzer without any probe connections.

Of course, you can also use the Pico board exclusively as a logic analyzer to observe external signals, or observe both internal and external signals simultaneously. The method for using it as a dedicated logic analyzer is explained [at the end of this article](#dedicated-la).

This article explains how to install and set up pico-jxgLABO and PulseView, and how to actually observe waveforms on the Pico board. The instructions assume a Windows host PC, but similar steps should work on Linux as well.


#### Integration with PulseView

When you press the `Start` button in PulseView, it performs the equivalent of the `la enable` command on the Pico board. You must set the GPIO pins and other sampling options with the `la` command beforehand.

When sampling starts, pico-jxgLABO polls the buffer memory status and sends sampling data to PulseView when available. Pressing the `Stop` button stops sending this data. Even after data transfer is complete, the buffer memory contents remain, so you can display waveform data or perform protocol analysis with the `print` subcommand of the `la` command.

