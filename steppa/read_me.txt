[[[Steppa]]]
by Derek Scott -> Dajis Systems
[check the "version_history" subpatch inside "steppa.maxpat" for a detailed changelog]

Quick Start:

1. Set which MIDI device will send note and continuous controller data with the "MIDI Out Device" menu.

2. Set which sync mode Steppa will operate in with the "Sync Mode" menu (note: 'MIDI Sync' mode is also used for standalone operation).

3. Set which MIDI device will send MIDI clock data with the "MIDI Sync Device" menu.

4. If using 'Click Sync' mode, the "Click Sync Offset" dial is used to correct the synchronization.



Sync Mode Descriptions:

MIDI Sync-
MIDI Sync mode is used for standalone operation and for driving the clock of other MIDI devices (ie. drum machines, sequencers, etc). When using this mode, the "Click Sync Offset" control does not affect the synchronization of the clock.

Click Sync-
Click Sync mode is used when a 'click track' is sent to the audio input of the computer and used as a master clock source. In this mode, the click track should already be running before starting the clock in Steppa so it has something to sync to. Once Steppa is started and it is not correctly synchronized with the click track, use the "Click Sync Offset" dial or the "tempo" control on Beatseqr to adjust the offset.



Port descriptions:

rcv port-
Used to set the UDP port to receive OpenSoundControl data

send port-
Used to set the UDP port to send OpenSoundControl data



Disclaimer:
This 'read me' needs to be expanded, but it should be what you need to get going. Look for an expanded 'read me' and Steppa manual in upcoming versions. If you need help, please visit the Beatseqr forum at http://beatseqr.com/forum.


http://beatseqr.com
http://dajis.net
http://hapticsynapses.com
http://doboxrecordings.com
http://somesoundswelike.com
http://somejunkwelike.com
http://sc-fa.com

