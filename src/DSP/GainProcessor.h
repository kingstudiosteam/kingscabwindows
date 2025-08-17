#pragma once
#include <juce_dsp/juce_dsp.h>

// Lightweight equal-power pan + scalar gain. No allocations; safe on audio thread.
namespace dsp
{
  class GainProcessor
  {
  public:
    void prepare(double sampleRate, int, int) noexcept { juce::ignoreUnused(sampleRate); }
    void reset() noexcept {}

    void setParameters(float linearGain, float pan, bool isBypassed) noexcept
    {
      bypassed = isBypassed;
      const float t = juce::jlimit(-1.0f, 1.0f, pan);
      const float theta = 0.25f * juce::MathConstants<float>::pi * (t + 1.0f);
      const float l = std::cos(theta);
      const float r = std::sin(theta);
      leftGain  = linearGain * l;
      rightGain = linearGain * r;
    }

    void process(juce::AudioBuffer<float>& buffer) noexcept
    {
      if (bypassed)
        return;

      const int numChannels = buffer.getNumChannels();
      const int numSamples  = buffer.getNumSamples();

      for (int ch = 0; ch < numChannels; ++ch)
      {
        const float g = (ch % 2 == 0 ? leftGain : rightGain);
        buffer.applyGain(ch, 0, numSamples, g);
      }
    }

  private:
    float leftGain  = 1.0f;
    float rightGain = 1.0f;
    bool  bypassed  = false;
  };
}


