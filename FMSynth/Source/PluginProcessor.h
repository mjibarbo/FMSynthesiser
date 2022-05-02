/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FMSynth.h"

//==============================================================================
/**
*/
class FMSynthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FMSynthAudioProcessor();
    ~FMSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    /// Parameters 
    juce::AudioProcessorValueTreeState parameters; 

    //Operator 1 DSP Parameters

    std::atomic<float>* amount1Param;
    std::atomic<float>* ratio1Param;

    //Operator 1 Waveform selection parameters

    std::atomic<float>* waveTypeParam;

    //Operator 2 DSP Parameters

    std::atomic<float>* amount2Param;
    std::atomic<float>* ratio2Param;

    //Operator 2 Waveform selection parameters

    std::atomic<float>* waveType2Param;

    //LFO DSP Parameters

    std::atomic<float>* lfo1Freq;

    //LFO Waveform selection parameters

    std::atomic<float>* lfowaveTypeParam;

    //LFO Route selection parameter

    std::atomic<float>* lfoRouteParam;

    // Envelope Parameters
    std::atomic<float>* attack1Param;
    std::atomic<float>* decay1Param;
    std::atomic<float>* sustain1Param;
    std::atomic<float>* release1Param;

    juce::Synthesiser synth;

    int voiceCount = 16; //Number of voices in the polyphony 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FMSynthAudioProcessor)
};
