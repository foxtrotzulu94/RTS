# M4 Sherman Toy Tank

An ESP32-based Toy Tank controlled over Serial Bluetooth

---> **This repo is still a work-in-progress** <---

## Overview

This repo contains all the major subsystems of the project, detailed below

## Mechanical

The `/mech` folder contains everything related to the exterior, papercraft shell including the starting low-poly M4 Sherman model and the ready-to-print Letter-sized .docx and .pdf files. These were taken from the PDO, but is not made available in this repo.

## Electrical

The `/elec` folder contains only a Fritzing project file (.fzz) that provides a breadboard and schematic view of the project.

## Software

The `/software` folder hosts all of the code that goes into the project, specifically

- The native Android Mobile Client in `/software/Android`, written in Kotlin

- The ESP32 driver software available in `/software/esp32`, mostly written in C/C++

At the root of the folder, a specification for human-readable messages is also provided.
