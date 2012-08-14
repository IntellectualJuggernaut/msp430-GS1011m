msp430-GS1011m
==============

GS1011m SPI interface to msp430f2618 using DMA.  This is only a subset of the capabilities of the GS1011m 
(only the operations that I need, not a full library).
DMA channel 0 is dedicated to transmission and channel 1 is dedicated to reception.

The GS1011m is a big baby.  It works well with SPI mode 3, as long as I do not approach more than 1 MHz clock.  
The datasheet says that it can run a bit faster than 1 MHz, but even at 1 MHz, you will still have to deal 
with increasing XOFF characters.

The method that I have found to work well with DMA may allow me to use mode 0, which has a top speed of ~3 MHz.
Tomorrow, we will pickup a new board from our printer (our last design underestimated how many amps the GS1011m actuall draws).
[Hint: it is more than what the datasheet says].

I will update the repo in the next day or two with the strategy that yields the fastest transfer rate, taking into account
the number of XOFF characters received.