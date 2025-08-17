#include "IRManager.h"

//==============================================================================
IRManager::IRManager()
{
    // Initialize all IR slots as empty
    for (auto& slot : loadedIRs)
    {
        slot.isLoaded = false;
        slot.buffer = nullptr;
    }
}

IRManager::~IRManager()
{
}

//==============================================================================
void IRManager::setIRDirectory(const juce::File& directory)
{
    juce::ScopedLock lock(folderLock);
    
    irRootDirectory = directory;
    if (irRootDirectory.exists() && irRootDirectory.isDirectory())
    {
        scanForIRs();
    }
}

void IRManager::scanForIRs()
{
    juce::ScopedLock lock(folderLock);
    
    folders.clear();
    
    if (!irRootDirectory.exists() || !irRootDirectory.isDirectory())
    {
        DBG("IRManager: Root directory does not exist: " << irRootDirectory.getFullPathName());
        return;
    }
    
    DBG("IRManager: Scanning directory: " << irRootDirectory.getFullPathName());

    // Scan all subdirectories for IR files
    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(irRootDirectory, false, "*", juce::File::findDirectories))
    {
        auto subDir = entry.getFile();
        if (subDir.isDirectory())
        {
            DBG("IRManager: Found directory: " << subDir.getFileName());
            FolderInfo folderInfo(subDir);
            scanDirectory(subDir, folderInfo);
            
            DBG("IRManager: Directory '" << subDir.getFileName() << "' contains " << folderInfo.irFiles.size() << " IR files");
            
            // Add ALL folders, even if they appear empty (for debugging)
            folders.push_back(std::move(folderInfo));
            DBG("IRManager: Added folder: " << folderInfo.name << " (contains " << folderInfo.irFiles.size() << " IR files)");
            
            // Debug: Also add a note if the folder was empty
            if (folderInfo.irFiles.empty())
            {
                DBG("IRManager: WARNING: Added empty folder: " << subDir.getFileName());
            }
        }
    }

    // Don't scan root directory directly - only subdirectories

    // Sort folders alphabetically
    std::sort(folders.begin(), folders.end(), 
        [](const FolderInfo& a, const FolderInfo& b) {
            return a.name.compareIgnoreCase(b.name) < 0;
        });
}

//==============================================================================
const IRManager::FolderInfo* IRManager::getFolder(int index) const
{
    juce::ScopedLock lock(folderLock);
    
    if (index >= 0 && index < static_cast<int>(folders.size()))
        return &folders[index];
    
    return nullptr;
}

const IRManager::FolderInfo* IRManager::getFolderByName(const juce::String& name) const
{
    juce::ScopedLock lock(folderLock);
    
    for (const auto& folder : folders)
    {
        if (folder.name.equalsIgnoreCase(name))
            return &folder;
    }
    
    return nullptr;
}

//==============================================================================
bool IRManager::loadIR(int slotIndex, const juce::File& irFile)
{
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return false;

    if (!isValidIRFile(irFile))
        return false;

    juce::ScopedLock lock(irLock);
    
    auto& slot = loadedIRs[slotIndex];
    
    // Create new buffer for pristine audio quality
    auto newBuffer = std::make_unique<juce::AudioBuffer<float>>();
    IRInfo newInfo = getIRInfo(irFile);
    
    if (loadIRBuffer(irFile, *newBuffer, newInfo))
    {
        // Process for optimal quality
        validateAndProcessIR(*newBuffer, newInfo);
        
        // Atomically replace the old IR
        slot.buffer = std::move(newBuffer);
        slot.info = newInfo;
        slot.isLoaded = true;
        
        return true;
    }
    
    return false;
}

void IRManager::clearIR(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return;

    juce::ScopedLock lock(irLock);
    
    auto& slot = loadedIRs[slotIndex];
    slot.buffer = nullptr;
    slot.isLoaded = false;
    slot.info = IRInfo();
}

bool IRManager::isIRLoaded(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return false;

    juce::ScopedLock lock(irLock);
    return loadedIRs[slotIndex].isLoaded;
}

const IRManager::IRInfo* IRManager::getLoadedIRInfo(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return nullptr;

    juce::ScopedLock lock(irLock);
    
    const auto& slot = loadedIRs[slotIndex];
    return slot.isLoaded ? &slot.info : nullptr;
}

const juce::File& IRManager::getLoadedIR(int slotIndex) const
{
    static juce::File emptyFile;
    
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return emptyFile;

    juce::ScopedLock lock(irLock);
    
    const auto& slot = loadedIRs[slotIndex];
    return slot.isLoaded ? slot.info.file : emptyFile;
}

const juce::AudioBuffer<float>* IRManager::getIRBuffer(int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= kMaxIRSlots)
        return nullptr;

    juce::ScopedLock lock(irLock);
    
    const auto& slot = loadedIRs[slotIndex];
    return slot.isLoaded ? slot.buffer.get() : nullptr;
}

//==============================================================================
bool IRManager::isValidIRFile(const juce::File& file)
{
    if (!file.exists())
        return false;
    
    // Support multiple audio formats common for IRs (case insensitive)
    juce::StringArray validExtensions = { "wav", "aiff", "aif", "flac", "ogg", "m4a", "mp3" };
    bool hasValidExtension = false;
    auto fileExtension = file.getFileExtension().toLowerCase();
    
    for (const auto& ext : validExtensions)
    {
        if (fileExtension == ("." + ext))
        {
            hasValidExtension = true;
            break;
        }
    }
    
    if (!hasValidExtension)
        return false;

    // Quick format check using JUCE's audio format manager
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    
    if (reader == nullptr)
        return false;

    // Validate sample rate and length for IR processing
    if (reader->sampleRate < kMinValidSampleRate || reader->sampleRate > kMaxValidSampleRate)
        return false;

    if (reader->lengthInSamples > kMaxIRLengthSamples || reader->lengthInSamples <= 0)
        return false;

    // Must be mono or stereo
    if (reader->numChannels < 1 || reader->numChannels > 2)
        return false;

    return true;
}

IRManager::IRInfo IRManager::getIRInfo(const juce::File& file)
{
    IRInfo info(file);
    
    if (!file.exists())
        return info;

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    
    if (reader != nullptr)
    {
        info.sampleRate = reader->sampleRate;
        info.lengthInSamples = static_cast<int>(reader->lengthInSamples);
        info.numChannels = static_cast<int>(reader->numChannels);
        info.isValid = isValidIRFile(file);
    }
    
    return info;
}

//==============================================================================
void IRManager::scanDirectory(const juce::File& directory, FolderInfo& folderInfo)
{
    DBG("IRManager: Scanning directory contents: " << directory.getFullPathName());
    
    // Scan for multiple audio file formats common for IRs
    juce::StringArray audioExtensions = { "*.wav", "*.aiff", "*.aif", "*.flac", "*.ogg", "*.m4a", "*.mp3", "*.WAV", "*.AIFF", "*.AIF", "*.FLAC", "*.OGG", "*.M4A", "*.MP3" };
    
    int totalFilesFound = 0;
    int validFilesFound = 0;
    
    // First, scan for files directly in this directory
    std::unordered_set<juce::String> seenCanonicalPaths; // dedupe by canonical path
    for (const auto& extension : audioExtensions)
    {
        for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(directory, false, extension, juce::File::findFiles))
        {
            auto file = entry.getFile();
            auto canonical = file.getFullPathName();
            if (seenCanonicalPaths.find(canonical) != seenCanonicalPaths.end())
                continue; // skip duplicates
            seenCanonicalPaths.insert(canonical);
            totalFilesFound++;
            DBG("IRManager: Found file: " << file.getFileName() << " (extension: " << extension << ")");
            
            if (isValidIRFile(file))
            {
                IRInfo irInfo = getIRInfo(file);
                if (irInfo.isValid)
                {
                    folderInfo.irFiles.push_back(std::move(irInfo));
                    validFilesFound++;
                    DBG("IRManager: Added valid IR: " << file.getFileName());
                }
                else
                {
                    DBG("IRManager: IR info invalid for: " << file.getFileName());
                }
            }
            else
            {
                DBG("IRManager: File not valid IR: " << file.getFileName());
            }
        }
    }
    
    // Then, recursively scan subdirectories for files
    for (juce::DirectoryEntry entry : juce::RangedDirectoryIterator(directory, false, "*", juce::File::findDirectories))
    {
        auto subDir = entry.getFile();
        DBG("IRManager: Found subdirectory: " << subDir.getFileName());
        
        // Recursively scan this subdirectory for IR files
        for (const auto& extension : audioExtensions)
        {
            for (juce::DirectoryEntry subEntry : juce::RangedDirectoryIterator(subDir, false, extension, juce::File::findFiles))
            {
                auto file = subEntry.getFile();
                auto canonical = file.getFullPathName();
                if (seenCanonicalPaths.find(canonical) != seenCanonicalPaths.end())
                    continue; // skip duplicates
                seenCanonicalPaths.insert(canonical);
                totalFilesFound++;
                
                // Create a display name that includes the subdirectory name
                auto displayName = subDir.getFileName() + "/" + file.getFileNameWithoutExtension();
                
                DBG("IRManager: Found nested file: " << displayName << " (" << file.getFileExtension() << ")");
                
                if (isValidIRFile(file))
                {
                    IRInfo irInfo = getIRInfo(file);
                    if (irInfo.isValid)
                    {
                        irInfo.name = displayName; // Override name to include subdirectory
                        folderInfo.irFiles.push_back(std::move(irInfo));
                        validFilesFound++;
                        DBG("IRManager: Added valid nested IR: " << displayName);
                    }
                    else
                    {
                        DBG("IRManager: Nested IR info invalid for: " << displayName);
                    }
                }
                else
                {
                    DBG("IRManager: Nested file not valid IR: " << displayName);
                }
            }
        }
    }
    
    DBG("IRManager: Directory scan complete. Found " << totalFilesFound << " total files, " << validFilesFound << " valid IRs");
    
    // Sort IR files by name for consistent ordering
    std::sort(folderInfo.irFiles.begin(), folderInfo.irFiles.end(),
        [](const IRInfo& a, const IRInfo& b) {
            return a.name.compareIgnoreCase(b.name) < 0;
        });
}

bool IRManager::loadIRBuffer(const juce::File& file, juce::AudioBuffer<float>& buffer, IRInfo& info)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    
    if (reader == nullptr)
        return false;

    // Allocate buffer with exact precision for pristine quality
    int numChannels = static_cast<int>(reader->numChannels);
    int lengthInSamples = static_cast<int>(reader->lengthInSamples);
    
    buffer.setSize(numChannels, lengthInSamples, false, true, true);
    
    // Read with highest quality settings
    if (!reader->read(&buffer, 0, lengthInSamples, 0, true, true))
        return false;

    // Update info with actual loaded data
    info.sampleRate = reader->sampleRate;
    info.lengthInSamples = lengthInSamples;
    info.numChannels = numChannels;
    info.isValid = true;
    
    return true;
}

void IRManager::validateAndProcessIR(juce::AudioBuffer<float>& buffer, IRInfo& info)
{
    const float kSilenceThreshold = 0.0001f; // -80dB
    const int kMinIRLength = 64; // Minimum viable IR length
    
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    // Find actual end of IR (remove trailing silence for efficiency)
    int actualLength = numSamples;
    for (int sample = numSamples - 1; sample >= kMinIRLength; --sample)
    {
        bool foundSignal = false;
        for (int ch = 0; ch < numChannels; ++ch)
        {
            if (std::abs(buffer.getSample(ch, sample)) > kSilenceThreshold)
            {
                foundSignal = true;
                break;
            }
        }
        
        if (foundSignal)
        {
            actualLength = sample + 1;
            break;
        }
    }
    
    // Trim buffer if significant silence found
    if (actualLength < numSamples * 0.8f && actualLength >= kMinIRLength)
    {
        juce::AudioBuffer<float> trimmedBuffer(numChannels, actualLength);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            trimmedBuffer.copyFrom(ch, 0, buffer, ch, 0, actualLength);
        }
        buffer = std::move(trimmedBuffer);
        info.lengthInSamples = actualLength;
    }
    
    // Apply gentle fade-out to prevent clicks (last 64 samples)
    const int fadeLength = juce::jmin(64, numSamples / 10);
    if (fadeLength > 0)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < fadeLength; ++i)
            {
                int sampleIndex = numSamples - fadeLength + i;
                float fade = 1.0f - (static_cast<float>(i) / static_cast<float>(fadeLength));
                channelData[sampleIndex] *= fade;
            }
        }
    }
    
    // Convert mono to stereo if needed for consistent processing
    if (numChannels == 1)
    {
        juce::AudioBuffer<float> stereoBuffer(2, buffer.getNumSamples());
        stereoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
        stereoBuffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
        buffer = std::move(stereoBuffer);
        info.numChannels = 2;
    }
}
