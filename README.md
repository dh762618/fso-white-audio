# fso-white-audio
Repo for the FSO White Capstone group - all source code for the project will be maintained here

## Team
- Main Software Lead: Devin Holt
GitHubID: dh762618
- Software Support and Managerial Lead: Daniel Cobb
GitHubID: dcobb013
- Hardware Engineers: Carson Magni and Remi Fordyce

## Project Description
An audio signal will be received using one of the ports on the Transmission Teensy box, which will be filtered and sent to the Teensy.
Once the Teensy receives this data, it will encode it and send this audio via an optical device to the receiver.
The receiver will decode this data and send the output audio to a connected speaker with minimal latency.

## Pictures of Final Implementation
- Transmission Box
<img src="imagefiles\transmission.jpg" alt="Transmission Box" width="281">
- Receiver Box
<img src="imagefiles\receiving.jpg" alt="Receiver Box" width="288">

## Final Design Overview
<img src="imagefiles\finaldesignov.png" alt="Final Design Overview Flowchart" width="1519">