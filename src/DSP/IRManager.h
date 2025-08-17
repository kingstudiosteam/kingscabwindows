#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <unordered_set>

//==============================================================================
/**
 * Impulse Response Manager for The King's Cab
 * 
 * Handles:
 * - IR file loading and validation
 * - Folder structure management
 * - IR metadata and organization
 * - Thread-safe access to IR data
 */
class IRManager
{
public:
    //==============================================================================
    struct IRInfo
    {
        juce::File file;
        juce::String name;
        juce::String folder;
        double sampleRate = 0.0;
        int lengthInSamples = 0;
        int numChannels = 0;
        bool isValid = false;

        IRInfo() = default;
        IRInfo(const juce::File& f) : file(f) 
        {
            name = f.getFileNameWithoutExtension();
            folder = f.getParentDirectory().getFileName();
        }
    };

    struct FolderInfo
    {
        juce::String name;
        juce::File directory;
        std::vector<IRInfo> irFiles;

        FolderInfo() = default;
        FolderInfo(const juce::File& dir) : directory(dir)
        {
            name = dir.getFileName();
        }
    };

    //==============================================================================
    IRManager();
    ~IRManager();

    //==============================================================================
    // Directory Management
    void setIRDirectory(const juce::File& directory);
    const juce::File& getIRDirectory() const { return irRootDirectory; }
    void scanForIRs();

    //==============================================================================
    // Folder Access
    const std::vector<FolderInfo>& getFolders() const { return folders; }
    int getNumFolders() const { return static_cast<int>(folders.size()); }
    const FolderInfo* getFolder(int index) const;
    const FolderInfo* getFolderByName(const juce::String& name) const;

    //==============================================================================
    // IR Loading and Management
    bool loadIR(int slotIndex, const juce::File& irFile);
    void clearIR(int slotIndex);
    bool isIRLoaded(int slotIndex) const;

    const IRInfo* getLoadedIRInfo(int slotIndex) const;
    const juce::File& getLoadedIR(int slotIndex) const;
    const juce::AudioBuffer<float>* getIRBuffer(int slotIndex) const;

    //==============================================================================
    // IR Validation
    static bool isValidIRFile(const juce::File& file);
    static IRInfo getIRInfo(const juce::File& file);

    //==============================================================================
    // Constants
    static constexpr int kMaxIRSlots = 6;
    static constexpr int kMaxIRLengthSamples = 192000; // 4 seconds at 48kHz
    static constexpr double kMinValidSampleRate = 44100.0;
    static constexpr double kMaxValidSampleRate = 192000.0;

private:
    //==============================================================================
    struct LoadedIR
    {
        IRInfo info;
        std::unique_ptr<juce::AudioBuffer<float>> buffer;
        bool isLoaded = false;
    };

    //==============================================================================
    // Core data
    juce::File irRootDirectory;
    std::vector<FolderInfo> folders;
    std::array<LoadedIR, kMaxIRSlots> loadedIRs;

    // Thread safety
    mutable juce::CriticalSection folderLock;
    mutable juce::CriticalSection irLock;

    //==============================================================================
    // Helper methods
    void scanDirectory(const juce::File& directory, FolderInfo& folderInfo);
    bool loadIRBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer, IRInfo& info);
    void validateAndProcessIR(juce::AudioBuffer<float>& buffer, IRInfo& info);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRManager)
};
