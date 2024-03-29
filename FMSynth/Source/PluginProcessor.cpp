/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FMSynthAudioProcessor::FMSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif

    parameters(*this, nullptr, "ParamTreeID", {

    //Modulator parameters

     std::make_unique<juce::AudioParameterFloat>("amount","Mod Amount",100.0f,1000.0f,200.0f),
     std::make_unique<juce::AudioParameterFloat>("ratio","Mod Ratio",0.0f,1000.0f,5.0f),

    //Modulator wavetype selection parameters 
    std::make_unique<juce::AudioParameterChoice>("waveType1","Mod Waveform",juce::StringArray{"Sine","Triangle","Square", "Saw"},0),

    //LFOs parameters 

    std::make_unique<juce::AudioParameterFloat>("lfoFreq1","LFO Ratio",0.05f,20.0f,1.0f),

    //LFO wavetype selection parameter
     std::make_unique<juce::AudioParameterChoice>("lfowaveType1","LFO Shape",juce::StringArray{"Sine","Triangle","Square", "Saw"},0),


    //Envelope parameters

    std::make_unique<juce::AudioParameterFloat>("attack1","Attack",0.0f,1.0f,0.5f),
    std::make_unique<juce::AudioParameterFloat>("decay1","Decay",0.0f,1.0f,0.5f),
    std::make_unique<juce::AudioParameterFloat>("sustain1","Sustain",0.0f,1.0f,1.0f),
    std::make_unique<juce::AudioParameterFloat>("release1","Release",0.0f,3.0f,0.5f),

    //Gain Parameter

    std::make_unique<juce::AudioParameterFloat>("gain","Gain",0.01f,1.0f,0.5f),

        })
{
/// CONSTRUCTOR ///

    gainParam = parameters.getRawParameterValue("gain");

    //Modulator parameters
    amount1Param = parameters.getRawParameterValue("amount");
    ratio1Param = parameters.getRawParameterValue("ratio");

    //Modulator wavetype selection parameter
    waveTypeParam = parameters.getRawParameterValue("waveType1");


    //LFO parameters 

    lfo1Freq = parameters.getRawParameterValue("lfoFreq1");

    //LFO wavetype selection parameter
    lfowaveTypeParam = parameters.getRawParameterValue("lfowaveType1");

    //Envelope parameters
    attack1Param = parameters.getRawParameterValue("attack1");
    decay1Param = parameters.getRawParameterValue("decay1");
    sustain1Param = parameters.getRawParameterValue("sustain1");
    release1Param = parameters.getRawParameterValue("release1");


    //Add a voice to the synth depending on voiceCount
    for (int i = 0; i < voiceCount; i++) 
    {
        synth.addVoice(new FMSynthVoice());
    }

    synth.addSound(new FMSynthSound);

    //For loop to set the parameters for each voice

    for (int i = 0; i < voiceCount; i++)
    {
        FMSynthVoice* v = dynamic_cast<FMSynthVoice*>(synth.getVoice(i));
        v->setEnvelopeParameterPointers(attack1Param, decay1Param, sustain1Param, release1Param);
        v-> setWaveTypeFromParameterPointer(waveTypeParam, lfowaveTypeParam);
        v->setModulatorParametersFromPointers(amount1Param,ratio1Param);
        v->setLFOFrequencyFromParameterPointer(lfo1Freq);
        v->setGainFromParameterPointer(gainParam);
    }


}

FMSynthAudioProcessor::~FMSynthAudioProcessor()
{
}

//==============================================================================
const juce::String FMSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FMSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FMSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FMSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FMSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FMSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FMSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FMSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FMSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void FMSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FMSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    //For loop to initialise sample rate in each voice 

    for (int i = 0; i < voiceCount; i++)
    {
        FMSynthVoice* v = dynamic_cast<FMSynthVoice*>(synth.getVoice(i));
        v->setSampleRate(sampleRate);
    }
}

void FMSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FMSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FMSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples()); 
}

//==============================================================================
bool FMSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FMSynthAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void FMSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FMSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FMSynthAudioProcessor();
}
