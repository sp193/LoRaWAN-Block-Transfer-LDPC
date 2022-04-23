FECTest - Example & test of the Low-Density Parity-Check (LDPC) code algorithm
-------------------------------------------------------------------------------

This is an example & test of the Low-Density Parity-Check (LDPC) code algorithm that is described by the LoRaWAN alliance's LoRaWAN Fragmented Data Block Transport Specification v1.0.0 (September 10, 2018).
LDPC is an forward error correction (FEC) system, enabling blocks of data to be transmitted over unreliable channels without needing retransmissions.
LDPC codes are also known as Gallager codes.

With this algorithm, it is possible to transfer a block of data in fragments over LoRaWAN, without needing to make any retransmissions. It is the heart of the official FUOTA workflow.
The Coding Ratio (CR) is decided by the server. With CR=4/5, it is possible to have up to 25% losses before the transfer becomes impossible. If there are too many losses, the process is deemed to have failed and should be restarted.

By not requiring the node to request for data, it is possible to complete the transfer without any uplinks at all.
The process becomes greatly simplified and various weaknesses of LoRaWAN are eliminated. Personally, I can forsee:
- Since the node does not need to request for fragments, no uplinks have to be made and thus there is no risk of collisions during uplinks.
- A lot of waiting time (i.e. gateway waiting for RX1/RX2 windows for transmission, node needs a timeout period before retrying) is eliminated, as the gateway could be made to sequentially transmit the fragments of the data block.
- Multicasting the fragments is possible, since the nodes do not individually request for fragments.
- Since the LNS will send the fragments, it is possible to know and fix the maximum fragment size beforehand, since the LNS decides on the datarate for RX2.

FECTest.exe <input file> <output file> <fragment size> <coding ratio> <losses>
        fragment size:  the size of each fragment, in bytes.
        coding ratio:   the ratio between M and N, such as 1:2, 4:5 etc.
        losses:         the number of frames to randomly lose, to simulate packet loss.
Example: FECTest.exe input.bin output.bin 32 4:5 10

References:
FUOTA Process Summary Technical Recommendation TR002 v1.0.0 (January 30, 2019)
LoRaWAN Fragmented Data Block Transport Specification v1.0.0 (September 10, 2018)
https://www.st.com/resource/en/application_note/an5554-lorawan-firmware-update-over-the-air-with-stm32cubewl-stmicroelectronics.pdf
https://github.com/Lora-net/LoRaMac-node/blob/master/src/apps/LoRaMac/common/LmHandler/packages/FragDecoder.c
