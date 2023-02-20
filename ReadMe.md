#Qt Speech-to-Text Demo

This is a simple demo application built with Qt that demonstrates how to integrate Google's speech-to-text API in a desktop application.

Demo screenshot
Getting Started

To run this demo application, you will need to have a Google Cloud account and enable the Speech-to-Text API. You will also need to create a service account and download the credentials file in JSON format.

Once you have your credentials file, copy the token and store it in 'myKey' variable in mainwindow.cpp
Prerequisites

To build and run this application, you will need:

    Qt 5 or later
    Ubuntu 18.04 or later

Building

To build this application, follow these steps:

    Clone this repository.
    Open the Speech-to-text.pro file in Qt Creator.
    Build the project.

Running

To run the application, simply click the "Run" button in Qt Creator, or navigate to the build directory and run the executable.
Usage

    Click the "Speech to text" button to start recording your voice.
    Speak into your microphone.
    Click the "Stop" button to stop recording.
    Wait for the application to transcribe your speech.
    View the transcription in the text box.
    click ctrl+s or File > Save to save your file.

Contributions

Contributions are welcome! If you find a bug or would like to add a feature, please open an issue or submit a pull request.
References

Here are some resources that may be helpful for learning more about Google's Speech-to-Text API and how to use it in Qt:

    Google Cloud Speech-to-Text API Documentation
    Google Cloud Speech-to-Text API Client Libraries
    Qt Documentation
