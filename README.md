# Coding-2-Final


For this final assignment,I wanted to express the idea that each person is the centre of their own star, and that when an object comes close to the centre, the star will shake, and when the object leaves it will gradually calm down. The Arduino is like an external noise, controlling the size of the dots in the background, and there is no way for the external noise to affect ourselves.

This is my design for this final assignment, in terms of principles I will go through three parts
### Mesh
I took an image of a starry sky and changed it to a mesh image and added a shake effect. I first took the original image and selected an 800*800 section, then read through the code associated with the mesh to form the star with a higher pixel count and a more connected section.

![14](https://user-images.githubusercontent.com/119190967/225882216-0ccfdf23-044c-4003-a094-35df8d7b3475.jpeg)
<img width="794" alt="image" src="https://user-images.githubusercontent.com/119190967/225882097-d573c01e-3b72-4874-acaf-4f289e85e21d.png">

### Sound
I control the output sound by controlling myOsc, and I also add my own local audio to influence the output sound.

```
void ofApp::audioOut(ofSoundBuffer& output){
    std::size_t outChannels = output.getNumChannels();
    for (int i = 0; i < output.getNumFrames(); ++i){
        
        myClock.ticker();
        float myOut = mySample.play(15);
        
        output[i * outChannels] = myOsc1.sinewave(10)/100+myOut;
        //output[i * outChannels] =myOut;
     //   output[i * outChannels + 1] = output[i * outChannels];
        
        //Hold the values so the draw method can draw them
        waveform[waveIndex] =  output[i * outChannels];
        if (waveIndex < (ofGetWidth() - 1)) {
            ++waveIndex;
        } else {
            waveIndex = 0;
        }
    }
}
```

### Arduino Link

