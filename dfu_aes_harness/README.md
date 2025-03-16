This directory contains a mismash of code from openiboot, s5l8720-securerom-dumper, and maybe other things too.

As such, consider it GPL3 licensed.

Sorry it's not better documented what comes from where, I didn't initially plan on making it public.

In its current state, I believe its set up to try to do AES DFA against the GID key (bring your own hardware glitches!)

There are also some hardcoded offsets for the "new" ipod2g bootrom.

Although right now it doesn't even compile. ymmv.

Ah, it does work, I just needed to install `arm-none-eabi-newlib` too. Seems like my `magic` variable for reporting status back to the exploit script is broken, but AES *is* running according to the square wave that appears on the NOR CS line!!!
